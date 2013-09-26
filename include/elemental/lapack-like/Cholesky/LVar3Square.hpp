/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef ELEM_LAPACK_CHOLESKY_LVAR3SQUARE_HPP
#define ELEM_LAPACK_CHOLESKY_LVAR3SQUARE_HPP

#include "elemental/blas-like/level1/Conjugate.hpp"
#include "elemental/blas-like/level3/Herk.hpp"
#include "elemental/blas-like/level3/Trsm.hpp"

// TODO: Reverse version

namespace elem {
namespace cholesky {

template<typename F>
inline void
LVar3Square( DistMatrix<F>& A )
{
#ifndef RELEASE
    CallStackEntry entry("cholesky::LVar3Square");
    if( A.Height() != A.Width() )
        LogicError("Can only compute Cholesky factor of square matrices");
    if( A.Grid().Height() != A.Grid().Width() )
        LogicError("CholeskyLVar3Square requires a square process grid");
#endif
    // Find the process holding our transposed data
    const Grid& g = A.Grid();
    const Int r = g.Height();
    Int transposeRank;
    {
        const Int colAlign = A.ColAlign();
        const Int rowAlign = A.RowAlign();
        const Int colShift = A.ColShift();
        const Int rowShift = A.RowShift();

        const Int transposeRow = (colAlign+rowShift) % r;
        const Int transposeCol = (rowAlign+colShift) % r;
        transposeRank = transposeRow + r*transposeCol;
    }
    const bool onDiagonal = ( transposeRank == g.VCRank() );

    DistMatrix<F,STAR,STAR> A11_STAR_STAR(g);
    DistMatrix<F,VC,  STAR> A21_VC_STAR(g);
    DistMatrix<F,STAR,MC  > A21Trans_STAR_MC(g);
    DistMatrix<F,STAR,MR  > A21Adj_STAR_MR(g);

    const Int n = A.Height();
    const Int bsize = Blocksize();
    for( Int k=0; k<n; k+=bsize )
    {
        const Int nb = Min(bsize,n-k);
        auto A11 = ViewRange( A, k,    k,    k+nb, k+nb );
        auto A21 = ViewRange( A, k+nb, k,    n,    k+nb );
        auto A22 = ViewRange( A, k+nb, k+nb, n,    n    );

        A11_STAR_STAR = A11;
        LocalCholesky( LOWER, A11_STAR_STAR );
        A11 = A11_STAR_STAR;

        A21_VC_STAR.AlignWith( A22 );
        A21_VC_STAR = A21;
        LocalTrsm
        ( RIGHT, LOWER, ADJOINT, NON_UNIT, F(1), A11_STAR_STAR, A21_VC_STAR );

        A21Trans_STAR_MC.AlignWith( A22 );
        A21Trans_STAR_MC.TransposeFrom( A21_VC_STAR );
        // SendRecv to form A21^T[* ,MR] from A21^T[* ,MC], then conjugate
        // the buffer to form A21^H[* ,MR]
        A21Adj_STAR_MR.AlignWith( A22 );
        A21Adj_STAR_MR.ResizeTo( A21.Width(), A21.Height() ); 
        {
            if( onDiagonal )
            { 
                const Int size = A11.Height()*A22.LocalWidth();
                MemCopy
                ( A21Adj_STAR_MR.Buffer(), 
                  A21Trans_STAR_MC.Buffer(), size );
            }
            else
            {
                const Int sendSize = A21.LocalHeight()*A21.Width();
                const Int recvSize = A11.Height()*A22.LocalWidth();
                // We know that the ldim is the height since we have manually 
                // created both temporary matrices.
                mpi::SendRecv 
                ( A21Trans_STAR_MC.Buffer(), sendSize, transposeRank,
                  A21Adj_STAR_MR.Buffer(),   recvSize, transposeRank,
                  g.VCComm() );
            }
            Conjugate( A21Adj_STAR_MR );
        }

        // (A21^T[* ,MC])^T A21^H[* ,MR] = A21[MC,* ] A21^H[* ,MR]
        //                               = (A21 A21^H)[MC,MR]
        LocalTrrk
        ( LOWER, TRANSPOSE, 
          F(-1), A21Trans_STAR_MC, A21Adj_STAR_MR, F(1), A22 );

        A21.TransposeFrom( A21Trans_STAR_MC );
    }
} 

} // namespace cholesky
} // namespace elem

#endif // ifndef ELEM_LAPACK_CHOLESKY_LVAR3SQUARE_HPP
