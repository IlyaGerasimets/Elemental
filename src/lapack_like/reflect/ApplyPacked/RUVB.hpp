/*
   Copyright (c) 2009-2016, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#ifndef EL_APPLYPACKEDREFLECTORS_RUVB_HPP
#define EL_APPLYPACKEDREFLECTORS_RUVB_HPP

namespace El {
namespace apply_packed_reflectors {

//
// Since applying Householder transforms from vectors stored right-to-left
// implies that we will be forming a generalization of
//
//   (I - tau_1 u_1 u_1^H) (I - tau_0 u_0 u_0^H) = 
//   I - tau_0 u_0 u_0^H - tau_1 u_1 u_1^H + (tau_0 tau_1 u_1^H u_0) u_1 u_0^H =
//   I - [ u_0, u_1 ] [  tau_0,                 0     ] [ u_0^H ]
//                    [ -tau_0 tau_1 u_1^H u_0, tau_1 ] [ u_1^H ],
//
// which has a lower-triangular center matrix, say S, we will form S as 
// the inverse of a matrix T, which can easily be formed as
// 
//   tril(T,-1) = tril( U^H U ),
//   diag(T) = 1/householderScalars or 1/conj(householderScalars),
//
// where U is the matrix of Householder vectors and householderScalars is the
// vector of Householder reflection coefficients.
//

template<typename F>
void
RUVB
( Conjugation conjugation,
  Int offset, 
  const Matrix<F>& H,
  const Matrix<F>& householderScalars,
        Matrix<F>& A )
{
    DEBUG_CSE
    DEBUG_ONLY(
      if( A.Width() != H.Height() )
          LogicError("A's width must match H's height");
    )
    const Int diagLength = H.DiagonalLength(offset);
    DEBUG_ONLY(
      if( householderScalars.Height() != diagLength )
          LogicError
          ("householderScalars must be the same length as H's offset diag");
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

        auto HPan  = H( IR(0,ki+nb), IR(kj,kj+nb) );
        auto ALeft = A( ALL,         IR(0,ki+nb)  );
        auto householderScalars1 = householderScalars( IR(k,k+nb), ALL );

        HPanCopy = HPan;
        MakeTrapezoidal( UPPER, HPanCopy, HPanCopy.Width()-HPanCopy.Height() );
        FillDiagonal( HPanCopy, F(1), HPanCopy.Width()-HPanCopy.Height() );

        Herk( LOWER, ADJOINT, Base<F>(1), HPanCopy, SInv );
        FixDiagonal( conjugation, householderScalars1, SInv );

        Gemm( NORMAL, NORMAL, F(1), ALeft, HPanCopy, Z );
        Trsm( RIGHT, LOWER, NORMAL, NON_UNIT, F(1), SInv, Z );
        Gemm( NORMAL, ADJOINT, F(-1), Z, HPanCopy, F(1), ALeft );
    }
}

template<typename F> 
void
RUVB
( Conjugation conjugation,
  Int offset,
  const ElementalMatrix<F>& HPre,
  const ElementalMatrix<F>& householderScalarsPre,
        ElementalMatrix<F>& APre )
{
    DEBUG_CSE
    DEBUG_ONLY(
      if( APre.Width() != HPre.Height() )
          LogicError("A's width must match H's height");
      AssertSameGrids( HPre, householderScalarsPre, APre );
    )

    DistMatrixReadProxy<F,F,MC,MR> HProx( HPre );
    DistMatrixReadProxy<F,F,MC,STAR>
      householderScalarsProx( householderScalarsPre );
    DistMatrixReadWriteProxy<F,F,MC,MR  > AProx( APre );
    auto& H = HProx.GetLocked();
    auto& householderScalars = householderScalarsProx.GetLocked();
    auto& A = AProx.Get();

    const Int diagLength = H.DiagonalLength(offset);
    DEBUG_ONLY(
      if( householderScalars.Height() != diagLength )
          LogicError
          ("householderScalars must be the same length as H's offset diag");
    )
    const Grid& g = H.Grid();
    DistMatrix<F> HPanCopy(g);
    DistMatrix<F,VC,  STAR> HPan_VC_STAR(g);
    DistMatrix<F,MR,  STAR> HPan_MR_STAR(g);
    DistMatrix<F,STAR,STAR> householderScalars1_STAR_STAR(g);
    DistMatrix<F,STAR,STAR> SInv_STAR_STAR(g);
    DistMatrix<F,STAR,MC  > ZAdj_STAR_MC(g);
    DistMatrix<F,STAR,VC  > ZAdj_STAR_VC(g);

    const Int iOff = ( offset>=0 ? 0      : -offset );
    const Int jOff = ( offset>=0 ? offset : 0       );

    const Int bsize = Blocksize();
    const Int kLast = LastOffset( diagLength, bsize );
    for( Int k=kLast; k>=0; k-=bsize )
    {
        const Int nb = Min(bsize,diagLength-k);
        const Int ki = k+iOff;
        const Int kj = k+jOff;

        auto HPan  = H( IR(0,ki+nb), IR(kj,kj+nb) );
        auto ALeft = A( ALL,         IR(0,ki+nb)  );
        auto householderScalars1 = householderScalars( IR(k,k+nb), ALL );

        HPanCopy = HPan;
        MakeTrapezoidal( UPPER, HPanCopy, HPanCopy.Width()-HPanCopy.Height() );
        FillDiagonal( HPanCopy, F(1), HPanCopy.Width()-HPanCopy.Height() );

        HPan_VC_STAR = HPanCopy;
        Zeros( SInv_STAR_STAR, nb, nb );
        Herk
        ( LOWER, ADJOINT,
          Base<F>(1), HPan_VC_STAR.LockedMatrix(),
          Base<F>(0), SInv_STAR_STAR.Matrix() );
        El::AllReduce( SInv_STAR_STAR, HPan_VC_STAR.ColComm() );
        householderScalars1_STAR_STAR = householderScalars1;
        FixDiagonal
        ( conjugation, householderScalars1_STAR_STAR, SInv_STAR_STAR );

        HPan_MR_STAR.AlignWith( ALeft );
        HPan_MR_STAR = HPan_VC_STAR;
        ZAdj_STAR_MC.AlignWith( ALeft );
        LocalGemm( ADJOINT, ADJOINT, F(1), HPan_MR_STAR, ALeft, ZAdj_STAR_MC );
        ZAdj_STAR_VC.AlignWith( ALeft );
        Contract( ZAdj_STAR_MC, ZAdj_STAR_VC );
        
        LocalTrsm
        ( LEFT, LOWER, ADJOINT, NON_UNIT, F(1), SInv_STAR_STAR, ZAdj_STAR_VC );

        ZAdj_STAR_MC = ZAdj_STAR_VC;
        LocalGemm
        ( ADJOINT, ADJOINT, F(-1), ZAdj_STAR_MC, HPan_MR_STAR, F(1), ALeft );
    }
}

} // namespace apply_packed_reflectors
} // namespace El

#endif // ifndef EL_APPLYPACKEDREFLECTORS_RUVB_HPP
