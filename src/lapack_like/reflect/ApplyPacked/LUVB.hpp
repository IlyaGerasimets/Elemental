/*
   Copyright (c) 2009-2016, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#ifndef EL_APPLYPACKEDREFLECTORS_LUVB_HPP
#define EL_APPLYPACKEDREFLECTORS_LUVB_HPP

namespace El {
namespace apply_packed_reflectors {

//
// Since applying Householder transforms from vectors stored right-to-left
// implies that we will be forming a generalization of
//
//   (I - tau_0 u_0 u_0^H) (I - tau_1 u_1 u_1^H) = 
//   I - tau_0 u_0 u_0^H - tau_1 u_1 u_1^H + (tau_0 tau_1 u_0^H u_1) u_0 u_1^H =
//   I - [ u_0, u_1 ] [ tau_0, -tau_0 tau_1 u_0^H u_1 ] [ u_0^H ]
//                    [ 0,      tau_1                 ] [ u_1^H ],
//
// which has an upper-triangular center matrix, say S, we will form S as 
// the inverse of a matrix T, which can easily be formed as
// 
//   triu(T,1) = triu( U^H U ),
//   diag(T) = 1/householderScalars or 1/conj(householderScalars),
//
// where U is the matrix of Householder vectors and householderScalars is the
// vector of scalars.
//

template<typename F>
void LUVB
( Conjugation conjugation,
  Int offset, 
  const Matrix<F>& H,
  const Matrix<F>& householderScalars,
        Matrix<F>& A )
{
    DEBUG_CSE
    const Int diagLength = H.DiagonalLength(offset);
    DEBUG_ONLY(
      if( householderScalars.Height() != diagLength )
          LogicError
          ("householderScalars must be the same length as H's offset diag.");
    )
    Matrix<F> HPanCopy, SInv, Z;

    const Int iOff = ( offset>=0 ? 0      : -offset );
    const Int jOff = ( offset>=0 ? offset : 0       );

    const Int bsize = Blocksize();
    const Int kLast = LastOffset( diagLength, bsize );
    for( Int k=kLast; k>=0; k-=bsize )
    {
        const Int nb = Min(bsize,diagLength-k);
        const Int ki = k+iOff;
        const Int kj = k+jOff;

        auto HPan = H( IR(0,ki+nb), IR(kj,kj+nb) );
        auto ATop = A( IR(0,ki+nb), ALL          );
        auto householderScalars1 = householderScalars( IR(k,k+nb), ALL );

        HPanCopy = HPan;
        MakeTrapezoidal( UPPER, HPanCopy, HPanCopy.Width()-HPanCopy.Height() );
        FillDiagonal( HPanCopy, F(1), HPanCopy.Width()-HPanCopy.Height() );

        Herk( UPPER, ADJOINT, Base<F>(1), HPanCopy, SInv );
        FixDiagonal( conjugation, householderScalars1, SInv );

        Gemm( ADJOINT, NORMAL, F(1), HPanCopy, ATop, Z );
        Trsm( LEFT, UPPER, NORMAL, NON_UNIT, F(1), SInv, Z );
        Gemm( NORMAL, NORMAL, F(-1), HPanCopy, Z, F(1), ATop );
    }
}

template<typename F> 
void LUVB
( Conjugation conjugation,
  Int offset, 
  const ElementalMatrix<F>& H,
  const ElementalMatrix<F>& householderScalarsPre, 
        ElementalMatrix<F>& APre )
{
    DEBUG_CSE
    DEBUG_ONLY(AssertSameGrids( H, householderScalarsPre, APre ))

    DistMatrixReadProxy<F,F,MC,STAR>
      householderScalarsProx( householderScalarsPre );
    auto& householderScalars = householderScalarsProx.GetLocked();

    DistMatrixReadWriteProxy<F,F,MC,MR> AProx( APre );
    auto& A = AProx.Get();

    const Int diagLength = H.DiagonalLength(offset);
    DEBUG_ONLY(
      if( householderScalars.Height() != diagLength )
          LogicError
          ("householderScalars must be the same length as H's offset diag.");
    )
    const Grid& g = H.Grid();
    auto HPan = unique_ptr<ElementalMatrix<F>>( H.Construct(g,H.Root()) );
    DistMatrix<F> HPanCopy(g);
    DistMatrix<F,VC,  STAR> HPan_VC_STAR(g);
    DistMatrix<F,MC,  STAR> HPan_MC_STAR(g);
    DistMatrix<F,STAR,STAR> householderScalars1_STAR_STAR(g);
    DistMatrix<F,STAR,STAR> SInv_STAR_STAR(g);
    DistMatrix<F,STAR,MR  > Z_STAR_MR(g);
    DistMatrix<F,STAR,VR  > Z_STAR_VR(g);

    const Int iOff = ( offset>=0 ? 0      : -offset );
    const Int jOff = ( offset>=0 ? offset : 0       );

    const Int bsize = Blocksize();
    const Int kLast = LastOffset( diagLength, bsize );
    for( Int k=kLast; k>=0; k-=bsize )
    {
        const Int nb = Min(bsize,diagLength-k);
        const Int ki = k+iOff;
        const Int kj = k+jOff;

        auto ATop = A( IR(0,ki+nb), ALL );
        auto householderScalars1 = householderScalars( IR(k,k+nb), ALL );

        LockedView( *HPan, H, IR(0,ki+nb), IR(kj,kj+nb) );
        Copy( *HPan, HPanCopy );
        MakeTrapezoidal( UPPER, HPanCopy, HPanCopy.Width()-HPanCopy.Height() );
        FillDiagonal( HPanCopy, F(1), HPanCopy.Width()-HPanCopy.Height() );

        HPan_VC_STAR = HPanCopy;
        Zeros( SInv_STAR_STAR, nb, nb );
        Herk
        ( UPPER, ADJOINT, 
          Base<F>(1), HPan_VC_STAR.LockedMatrix(),
          Base<F>(0), SInv_STAR_STAR.Matrix() );
        El::AllReduce( SInv_STAR_STAR, HPan_VC_STAR.ColComm() );
        householderScalars1_STAR_STAR = householderScalars1;
        FixDiagonal
        ( conjugation, householderScalars1_STAR_STAR, SInv_STAR_STAR );

        HPan_MC_STAR.AlignWith( ATop );
        HPan_MC_STAR = HPanCopy;
        Z_STAR_MR.AlignWith( ATop );
        LocalGemm( ADJOINT, NORMAL, F(1), HPan_MC_STAR, ATop, Z_STAR_MR );
        Z_STAR_VR.AlignWith( ATop );
        Contract( Z_STAR_MR, Z_STAR_VR );
        
        LocalTrsm
        ( LEFT, UPPER, NORMAL, NON_UNIT, F(1), SInv_STAR_STAR, Z_STAR_VR );

        Z_STAR_MR = Z_STAR_VR;
        LocalGemm( NORMAL, NORMAL, F(-1), HPan_MC_STAR, Z_STAR_MR, F(1), ATop );
    }
}

} // namespace apply_packed_reflectors
} // namespace El

#endif // ifndef EL_APPLYPACKEDREFLECTORS_LUVB_HPP
