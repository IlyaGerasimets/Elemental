/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/

// This file should be included into each of the BlockDistMatrix specializations
// as a workaround for the fact that C++11 constructor inheritance is not 
// yet widely supported.

namespace elem {

#define DM DistMatrix<T,ColDist,RowDist>
#define GDM GeneralDistMatrix<T,ColDist,RowDist>

// Public section
// ##############

// Constructors and destructors
// ============================

template<typename T>
DM::DistMatrix( const elem::Grid& grid, Int root )
: GDM(grid,root)
{ this->SetShifts(); }

template<typename T>
DM::DistMatrix( Int height, Int width, const elem::Grid& grid, Int root )
: GDM(grid,root)
{ this->SetShifts(); this->Resize(height,width); }

template<typename T>
DM::DistMatrix( const DM& A )
: GDM(A.Grid())
{
    DEBUG_ONLY(CallStackEntry cse("DistMatrix::DistMatrix"))
    this->SetShifts();
    if( &A != this )
        *this = A;
    else
        LogicError("Tried to construct DistMatrix with itself");
}

template<typename T>
template<Dist U,Dist V>
DM::DistMatrix( const DistMatrix<T,U,V>& A )
: GDM(A.Grid())
{
    DEBUG_ONLY(CallStackEntry cse("DistMatrix::DistMatrix"))
    this->SetShifts();
    if( ColDist != U || RowDist != V ||
        reinterpret_cast<const DM*>(&A) != this )
        *this = A;
    else
        LogicError("Tried to construct DistMatrix with itself");
}

template<typename T>
DM::DistMatrix( DM&& A ) noexcept : GDM(std::move(A)) { }

template<typename T> DM::~DistMatrix() { }

template<typename T>
DM&
DM::operator=( DM&& A )
{
    if( this->Viewing() && !A.Viewing() )
        this->operator=( (const DM&)A );
    else
        GDM::operator=( std::move(A) );
    return *this;
}

template<typename T>
elem::DistData DM::DistData() const { return elem::DistData(*this); }

} // namespace elem
