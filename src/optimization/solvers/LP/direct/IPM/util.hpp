/*
   Copyright (c) 2009-2015, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#include "El.hpp"
#include "../../../QP/direct/IPM/util.hpp"

namespace El {
namespace lp {
namespace direct {

// Initialize
// ==========
template<typename Real>
void Initialize
( const Matrix<Real>& A,
  const Matrix<Real>& b,
  const Matrix<Real>& c,
        Matrix<Real>& x,
        Matrix<Real>& y, 
        Matrix<Real>& z,
  bool primalInit, bool dualInit, bool standardShift );
template<typename Real>
void Initialize
( const AbstractDistMatrix<Real>& A,
  const AbstractDistMatrix<Real>& b,
  const AbstractDistMatrix<Real>& c,
        AbstractDistMatrix<Real>& x,
        AbstractDistMatrix<Real>& y, 
        AbstractDistMatrix<Real>& z,
  bool primalInit, bool dualInit, bool standardShift );
template<typename Real>
void Initialize
( const SparseMatrix<Real>& A,
  const Matrix<Real>& b,
  const Matrix<Real>& c,
        Matrix<Real>& x,
        Matrix<Real>& y, 
        Matrix<Real>& z,
        vector<Int>& map,
        vector<Int>& invMap,
        ldl::Separator& rootSep,
        ldl::NodeInfo& info,
  bool primalInit, bool dualInit, bool standardShift, 
  const RegQSDCtrl<Real>& qsdCtrl );
template<typename Real>
void Initialize
( const DistSparseMatrix<Real>& A,
  const DistMultiVec<Real>& b,
  const DistMultiVec<Real>& c,
        DistMultiVec<Real>& x,
        DistMultiVec<Real>& y, 
        DistMultiVec<Real>& z, 
        DistMap& map,
        DistMap& invMap,
        ldl::DistSeparator& rootSep,
        ldl::DistNodeInfo& info,
  bool primalInit, bool dualInit, bool standardShift,
  const RegQSDCtrl<Real>& qsdCtrl );

// Full system
// ===========
template<typename Real>
void KKT
( const Matrix<Real>& A, 
  const Matrix<Real>& x,
  const Matrix<Real>& z,
        Matrix<Real>& J,
  bool onlyLower=true );
template<typename Real>
void KKT
( const AbstractDistMatrix<Real>& A, 
  const AbstractDistMatrix<Real>& x,
  const AbstractDistMatrix<Real>& z,
        AbstractDistMatrix<Real>& J,
  bool onlyLower=true );
template<typename Real>
void KKT
( const SparseMatrix<Real>& A, 
        Real gamma,
        Real delta,
        Real beta,
  const Matrix<Real>& x,
  const Matrix<Real>& z,
        SparseMatrix<Real>& J,
  bool onlyLower=true );
template<typename Real>
void KKT
( const DistSparseMatrix<Real>& A, 
        Real gamma,
        Real delta,
        Real beta,
  const DistMultiVec<Real>& x,
  const DistMultiVec<Real>& z,
        DistSparseMatrix<Real>& J,
  bool onlyLower=true );

using qp::direct::KKTRHS;
using qp::direct::ExpandSolution;

// Augmented system
// ================
template<typename Real>
void AugmentedKKT
( const Matrix<Real>& A,
  const Matrix<Real>& x,
  const Matrix<Real>& z,
        Matrix<Real>& J,
  bool onlyLower=true );
template<typename Real>
void AugmentedKKT
( const AbstractDistMatrix<Real>& A,
  const AbstractDistMatrix<Real>& x,
  const AbstractDistMatrix<Real>& z,
        AbstractDistMatrix<Real>& J,
  bool onlyLower=true );
template<typename Real>
void AugmentedKKT
( const SparseMatrix<Real>& A,
        Real gamma,
        Real delta,
  const Matrix<Real>& x,
  const Matrix<Real>& z,
        SparseMatrix<Real>& J,
  bool onlyLower=true );
template<typename Real>
void AugmentedKKT
( const DistSparseMatrix<Real>& A,
        Real gamma,
        Real delta,
  const DistMultiVec<Real>& x,
  const DistMultiVec<Real>& z,
        DistSparseMatrix<Real>& J,
  bool onlyLower=true );

using qp::direct::AugmentedKKTRHS;
using qp::direct::ExpandAugmentedSolution;

// Normal system
// =============
template<typename Real>
void NormalKKT
( const Matrix<Real>& A,
        Real gamma,
        Real delta,
  const Matrix<Real>& x,
  const Matrix<Real>& z,
        Matrix<Real>& J,
  bool onlyLower=false );
template<typename Real>
void NormalKKT
( const AbstractDistMatrix<Real>& A,
        Real gamma,
        Real delta,
  const AbstractDistMatrix<Real>& x,
  const AbstractDistMatrix<Real>& z,
        AbstractDistMatrix<Real>& J,
  bool onlyLower=false );
template<typename Real>
void NormalKKT
( const SparseMatrix<Real>& A,
        Real gamma,
        Real delta,
  const Matrix<Real>& x,
  const Matrix<Real>& z,
        SparseMatrix<Real>& J,
  bool onlyLower=true );
template<typename Real>
void NormalKKT
( const DistSparseMatrix<Real>& A,
        Real gamma,
        Real delta,
  const DistMultiVec<Real>& x,
  const DistMultiVec<Real>& z,
        DistSparseMatrix<Real>& J,
  bool onlyLower=true );

template<typename Real>
void NormalKKTRHS
( const Matrix<Real>& A,
        Real gamma,
  const Matrix<Real>& x,
  const Matrix<Real>& z,
  const Matrix<Real>& rc,
  const Matrix<Real>& rb,
  const Matrix<Real>& rmu,
        Matrix<Real>& d );
template<typename Real>
void NormalKKTRHS
( const AbstractDistMatrix<Real>& A,
        Real gamma,
  const AbstractDistMatrix<Real>& x,
  const AbstractDistMatrix<Real>& z,
  const AbstractDistMatrix<Real>& rc,
  const AbstractDistMatrix<Real>& rb, 
  const AbstractDistMatrix<Real>& rmu,
        AbstractDistMatrix<Real>& d );
template<typename Real>
void NormalKKTRHS
( const SparseMatrix<Real>& A,
        Real gamma,
  const Matrix<Real>& x,
  const Matrix<Real>& z,
  const Matrix<Real>& rc,
  const Matrix<Real>& rb,
  const Matrix<Real>& rmu,
        Matrix<Real>& d );
template<typename Real>
void NormalKKTRHS
( const DistSparseMatrix<Real>& A,
        Real gamma,
  const DistMultiVec<Real>& x,
  const DistMultiVec<Real>& z,
  const DistMultiVec<Real>& rc,
  const DistMultiVec<Real>& rb,
  const DistMultiVec<Real>& rmu,
        DistMultiVec<Real>& d );

template<typename Real>
void ExpandNormalSolution
( const Matrix<Real>& A,
        Real gamma,
  const Matrix<Real>& x,
  const Matrix<Real>& z,
  const Matrix<Real>& rc,
  const Matrix<Real>& rmu,
        Matrix<Real>& dx, 
  const Matrix<Real>& dy, 
        Matrix<Real>& dz );
template<typename Real>
void ExpandNormalSolution
( const AbstractDistMatrix<Real>& A,
        Real gamma,
  const AbstractDistMatrix<Real>& x,
  const AbstractDistMatrix<Real>& z,
  const AbstractDistMatrix<Real>& rc,
  const AbstractDistMatrix<Real>& rmu,
        AbstractDistMatrix<Real>& dx, 
  const AbstractDistMatrix<Real>& dy, 
        AbstractDistMatrix<Real>& dz );
template<typename Real>
void ExpandNormalSolution
( const SparseMatrix<Real>& A,
        Real gamma,
  const Matrix<Real>& x,
  const Matrix<Real>& z,
  const Matrix<Real>& rc,
  const Matrix<Real>& rmu,
        Matrix<Real>& dx, 
  const Matrix<Real>& dy, 
        Matrix<Real>& dz );
template<typename Real>
void ExpandNormalSolution
( const DistSparseMatrix<Real>& A,
        Real gamma,
  const DistMultiVec<Real>& x,
  const DistMultiVec<Real>& z,
  const DistMultiVec<Real>& rc,
  const DistMultiVec<Real>& rmu,
        DistMultiVec<Real>& dx,
  const DistMultiVec<Real>& dy, 
        DistMultiVec<Real>& dz );

} // namespace direct
} // namespace lp
} // namespace El
