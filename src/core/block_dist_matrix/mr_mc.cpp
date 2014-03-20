/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#include "elemental-lite.hpp"

#define ColDist MR
#define RowDist MC

#include "./setup.hpp"

namespace elem {

// Public section
// ##############

// Assignment and reconfiguration
// ==============================

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,MC,MR>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [MC,MR]"))
    LogicError("This routine is not yet written");
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,MC,STAR>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [MC,STAR]"))
    std::unique_ptr<BlockDistMatrix<T,VC,STAR>> A_VC_STAR
    ( new BlockDistMatrix<T,VC,STAR>(A) );

    std::unique_ptr<BlockDistMatrix<T,VR,STAR>> A_VR_STAR
    ( new BlockDistMatrix<T,VR,STAR>(this->Grid()) );
    A_VR_STAR->AlignColsWith(*this);
    *A_VR_STAR = *A_VC_STAR;
    delete A_VC_STAR.release(); // lowers memory highwater

    *this = *A_VR_STAR;
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,STAR,MR>& A )
{ 
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [STAR,MR]"))
    std::unique_ptr<BlockDistMatrix<T,STAR,VR>> A_STAR_VR
    ( new BlockDistMatrix<T,STAR,VR>(A) );

    std::unique_ptr<BlockDistMatrix<T,STAR,VC>> A_STAR_VC
    ( new BlockDistMatrix<T,STAR,VC>(this->Grid()) );
    A_STAR_VR->AlignRowsWith(*this);
    *A_STAR_VC = *A_STAR_VR;
    delete A_STAR_VR.release(); // lowers memory highwater

    *this = *A_STAR_VC;
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,MD,STAR>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [MD,STAR]"))
    // TODO: More efficient implementation?
    BlockDistMatrix<T,STAR,STAR> A_STAR_STAR( A );
    *this = A_STAR_STAR;
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,STAR,MD>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [STAR,MD]"))
    // TODO: More efficient implementation?
    BlockDistMatrix<T,STAR,STAR> A_STAR_STAR( A );
    *this = A_STAR_STAR;
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BDM& A )
{ 
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [MR,MC]"))
    LogicError("This routine is not yet written");
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,MR,STAR>& A )
{ 
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [MR,STAR]"))
    this->RowFilterFrom( A );
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,STAR,MC>& A )
{ 
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [STAR,MC]"))
    this->ColFilterFrom( A );
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,VC,STAR>& A )
{ 
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [VC,STAR]"))
    BlockDistMatrix<T,VR,STAR> A_VR_STAR( A );
    *this = A_VR_STAR;
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,STAR,VC>& A )
{ 
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [STAR,VC]"))
    A.PartialRowAllToAll( *this );
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,VR,STAR>& A )
{ 
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [VR,STAR]"))
    A.PartialColAllToAll( *this );
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,STAR,VR>& A )
{ 
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [STAR,VR]"))
    BlockDistMatrix<T,STAR,VC> A_STAR_VC( A );
    *this = A_STAR_VC;
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,STAR,STAR>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [STAR,STAR]"))
    this->FilterFrom( A );
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,CIRC,CIRC>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [CIRC,CIRC]"))
    LogicError("This routine is not yet written");
    return *this;
}

// Realignment
// -----------

template<typename T>
void
BDM::AlignWith( const elem::BlockDistData& data )
{
    DEBUG_ONLY(CallStackEntry cse("[MR,MC]::AlignWith"))
    this->SetGrid( *data.grid );
    if( data.colDist == MR && data.rowDist == MC )
        this->Align
        ( data.blockHeight, data.blockWidth, 
          data.colAlign, data.rowAlign, data.colCut, data.rowCut );
    else if( data.colDist == MR && data.rowDist == STAR )
        this->AlignCols( data.blockHeight, data.colAlign, data.colCut );
    else if( data.colDist == MC && data.rowDist == MR )
        this->Align
        ( data.blockWidth, data.blockHeight, 
          data.rowAlign, data.colAlign, data.rowCut, data.colCut );
    else if( data.colDist == MC && data.rowDist == STAR )
        this->AlignRows( data.blockHeight, data.colAlign, data.colCut );
    else if( data.colDist == STAR && data.rowDist == MR )
        this->AlignCols( data.blockWidth, data.rowAlign, data.rowCut );
    else if( data.colDist == STAR && data.rowDist == MC )
        this->AlignRows( data.blockWidth, data.rowAlign, data.rowCut );
    else if( data.colDist == STAR && data.rowDist == VR )
        this->AlignCols
        ( data.blockWidth, data.rowAlign % this->ColStride(), data.rowCut );
    else if( data.colDist == STAR && data.rowDist == VC )
        this->AlignRows
        ( data.blockWidth, data.rowAlign % this->RowStride(), data.rowCut );
    else if( data.colDist == VR && data.rowDist == STAR )
        this->AlignCols
        ( data.blockHeight, data.colAlign % this->ColStride(), data.colCut );
    else if( data.colDist == VC && data.rowDist == STAR )
        this->AlignRows
        ( data.blockHeight, data.colAlign % this->RowStride(), data.colCut );
    DEBUG_ONLY(else LogicError("Nonsensical alignment"))
}

template<typename T>
void
BDM::AlignColsWith( const elem::BlockDistData& data )
{
    DEBUG_ONLY(CallStackEntry cse("[MR,MC]::AlignColsWith"))
    this->SetGrid( *data.grid );
    if( data.colDist == MR )
        this->AlignCols( data.blockHeight, data.colAlign, data.colCut );
    else if( data.rowDist == MR )
        this->AlignCols( data.blockWidth, data.rowAlign, data.rowCut );
    else if( data.colDist == VR )
        this->AlignCols
        ( data.blockHeight, data.colAlign % this->ColStride(), data.colCut );
    else if( data.rowDist == VR )
        this->AlignCols
        ( data.blockWidth, data.rowAlign % this->ColStride(), data.rowCut );
    DEBUG_ONLY(else LogicError("Nonsensical alignment"))
}

template<typename T>
void
BDM::AlignRowsWith( const elem::BlockDistData& data )
{
    DEBUG_ONLY(CallStackEntry cse("[MR,MC]::AlignRowsWith"))
    this->SetGrid( *data.grid );
    if( data.colDist == MC )
        this->AlignRows( data.blockHeight, data.colAlign, data.colCut );
    else if( data.rowDist == MC )
        this->AlignRows( data.blockWidth, data.rowAlign, data.rowCut );
    else if( data.colDist == VC )
        this->AlignRows
        ( data.blockHeight, data.colAlign % this->RowStride(), data.colCut );
    else if( data.rowDist == VC )
        this->AlignRows
        ( data.blockWidth, data.rowAlign % this->RowStride(), data.rowCut );
    DEBUG_ONLY(else LogicError("Nonsensical alignment"))
}

// Basic queries
// =============

template<typename T>
mpi::Comm BDM::DistComm() const { return this->grid_->VRComm(); }
template<typename T>
mpi::Comm BDM::CrossComm() const { return mpi::COMM_SELF; }
template<typename T>
mpi::Comm BDM::RedundantComm() const { return mpi::COMM_SELF; }
template<typename T>
mpi::Comm BDM::ColComm() const { return this->grid_->MRComm(); }
template<typename T>
mpi::Comm BDM::RowComm() const { return this->grid_->MCComm(); }

template<typename T>
Int BDM::ColStride() const { return this->grid_->MRSize(); }
template<typename T>
Int BDM::RowStride() const { return this->grid_->MCSize(); }

// Private section
// ###############

// Instantiate {Int,Real,Complex<Real>} for each Real in {float,double}
// ####################################################################

#define PROTO(T) template class BlockDistMatrix<T,ColDist,RowDist>
#define COPY(T,U,V) \
  template BlockDistMatrix<T,ColDist,RowDist>::BlockDistMatrix\
  ( const BlockDistMatrix<T,U,V>& A );\
  template BlockDistMatrix<T,ColDist,RowDist>::BlockDistMatrix\
  ( const DistMatrix<T,U,V>& A );
#define FULL(T) \
  PROTO(T); \
  COPY(T,CIRC,CIRC); \
  COPY(T,MC,  MR  ); \
  COPY(T,MC,  STAR); \
  COPY(T,MD,  STAR); \
  COPY(T,MR,  STAR); \
  COPY(T,STAR,MC  ); \
  COPY(T,STAR,MD  ); \
  COPY(T,STAR,MR  ); \
  COPY(T,STAR,STAR); \
  COPY(T,STAR,VC  ); \
  COPY(T,STAR,VR  ); \
  COPY(T,VC,  STAR); \
  COPY(T,VR,  STAR);

FULL(Int);
#ifndef DISABLE_FLOAT
FULL(float);
#endif
FULL(double);

#ifndef DISABLE_COMPLEX
#ifndef DISABLE_FLOAT
FULL(Complex<float>);
#endif
FULL(Complex<double>);
#endif 

} // namespace elem
