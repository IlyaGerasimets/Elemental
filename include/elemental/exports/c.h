/*
   Copyright (c) 2009-2011, Jack Poulson
   All rights reserved.

   This file is part of Elemental.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

    - Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    - Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    - Neither the name of the owner nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef ELEMENTAL_EXPORTS_C_H
#define ELEMENTAL_EXPORTS_C_H 1

#include "mpi.h"

/* Make sure that all of our configuration definitions are pulled in */
#include "elemental/config.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WITHOUT_COMPLEX
/* We should not assume C99 support and use <complex.h> */
typedef struct { float real; float imag; } SComplex;
typedef struct { double real; double imag; } DComplex;
#endif /* WITHOUT_COMPLEX */

/* Grid A */
typedef int Grid;

/* Handles for real distributed matrices */
typedef int MC_MR_Single;
typedef int MC_MR_Double;
typedef int MC_Star_Single;
typedef int MC_Star_Double;
typedef int MD_Star_Single;
typedef int MD_Star_Double;
typedef int MR_Star_Single;
typedef int MR_Star_Double;
typedef int Star_MC_Single;
typedef int Star_MC_Double;
typedef int Star_MD_Single;
typedef int Star_MD_Double;
typedef int Star_MR_Single;
typedef int Star_MR_Double;
typedef int Star_Star_Single;
typedef int Star_Star_Double;
typedef int Star_VC_Single;
typedef int Star_VC_Double;
typedef int Star_VR_Single;
typedef int Star_VR_Double;
typedef int VC_Star_Single;
typedef int VC_Star_Double;
typedef int VR_Star_Single;
typedef int VR_Star_Double;

#ifndef WITHOUT_COMPLEX
/* Handles for complex distributed matrices */
typedef int MC_MR_SComplex;
typedef int MC_MR_DComplex;
typedef int MC_Star_SComplex;
typedef int MC_Star_DComplex;
typedef int MD_Star_SComplex;
typedef int MD_Star_DComplex;
typedef int MR_Star_SComplex;
typedef int MR_Star_DComplex;
typedef int Star_MC_SComplex;
typedef int Star_MC_DComplex;
typedef int Star_MD_SComplex;
typedef int Star_MD_DComplex;
typedef int Star_MR_SComplex;
typedef int Star_MR_DComplex;
typedef int Star_Star_SComplex;
typedef int Star_Star_DComplex;
typedef int Star_VC_SComplex;
typedef int Star_VC_DComplex;
typedef int Star_VR_SComplex;
typedef int Star_VR_DComplex;
typedef int VC_Star_SComplex;
typedef int VC_Star_DComplex;
typedef int VR_Star_SComplex;
typedef int VR_Star_DComplex;
#endif /* WITHOUT_COMPLEX */

/* Elemental's environment */
void ElementalInit( int* argc, char** argv[] );
void ElementalFinalize();
int Blocksize();
void SetBlocksize( int blocksize );
void PushBlocksizeStack( int blocksize );
void PopBlocksizeStack();

/* Grid manipulation */
Grid CreateDefaultGrid( MPI_Comm comm );
Grid CreateGrid( MPI_Comm comm, int r, int c );
Grid GridHeight( Grid g );
Grid GridWidth( Grid g );
Grid GridSize( Grid g );
Grid InGrid( Grid g );
Grid GridVCRank( Grid g );
Grid GridVRRank( Grid g );
Grid GridMCRank( Grid g );
Grid GridMRRank( Grid g );
MPI_Comm GridVCComm( Grid g );
MPI_Comm GridVRComm( Grid g );
MPI_Comm GridMCComm( Grid g );
MPI_Comm GridMRComm( Grid g );

/* Clean up */
void ClearGrids();
void ClearDistMatrices();

/* Real double-precision distributed matrices */
MC_MR_Single CreateEmpty_MC_MR_Single( Grid g );
MC_MR_Double CreateEmpty_MC_MR_Double( Grid g );
Star_VR_Single CreateEmpty_Star_VR_Single( Grid g );
Star_VR_Double CreateEmpty_Star_VR_Double( Grid g );

MC_MR_Single Register_MC_MR_Single
( int height, int width, int colAlignment, int rowAlignment,
  float* buffer, int ldim, Grid g );
MC_MR_Double Register_MC_MR_Double
( int height, int width, int colAlignment, int rowAlignment,
  double* buffer, int ldim, Grid g );
Star_VR_Single Register_Star_VR_Single
( int height, int width, int rowAlignment, 
  float* buffer, int ldim, Grid g);
Star_VR_Double Register_Star_VR_Double
( int height, int width, int rowAlignment, 
  double* buffer, int ldim, Grid g );

void Print_MC_MR_Single( char* msg, MC_MR_Single A );
void Print_MC_MR_Double( char* msg, MC_MR_Double A );
void Print_MC_Star_Single( char* msg, MC_Star_Single A );
void Print_MC_Star_Double( char* msg, MC_Star_Double A );
void Print_MD_Star_Single( char* msg, MD_Star_Single A );
void Print_MD_Star_Double( char* msg, MD_Star_Double A );
void Print_MR_Star_Single( char* msg, MR_Star_Single A );
void Print_MR_Star_Double( char* msg, MR_Star_Double A );
void Print_Star_MC_Single( char* msg, Star_MC_Single A );
void Print_Star_MC_Double( char* msg, Star_MC_Double A );
void Print_Star_MR_Single( char* msg, Star_MR_Single A );
void Print_Star_MR_Double( char* msg, Star_MR_Double A );
void Print_Star_Star_Single( char* msg, Star_Star_Single A );
void Print_Star_Star_Double( char* msg, Star_Star_Double A );
void Print_Star_VC_Single( char* msg, Star_VC_Single A );
void Print_Star_VC_Double( char* msg, Star_VC_Double A );
void Print_Star_VR_Single( char* msg, Star_VR_Single A );
void Print_Star_VR_Double( char* msg, Star_VR_Double A );
void Print_VC_Star_Single( char* msg, VC_Star_Single A );
void Print_VC_Star_Double( char* msg, VC_Star_Double A );
void Print_VR_Star_Single( char* msg, VR_Star_Single A );
void Print_VR_Star_Double( char* msg, VR_Star_Double A );

#ifndef WITHOUT_COMPLEX
/* Complex double-precision distributed matrices */
MC_MR_SComplex CreateEmpty_MC_MR_SComplex( Grid g );
MC_MR_DComplex CreateEmpty_MC_MR_DComplex( Grid g );
Star_VR_SComplex CreateEmpty_Star_VR_SComplex( Grid g );
Star_VR_DComplex CreateEmpty_Star_VR_DComplex( Grid g );

MC_MR_SComplex Register_MC_MR_SComplex
( int height, int width, int colAlignment, int rowAlignment,
  SComplex* buffer, int ldim, Grid g );
MC_MR_DComplex Register_MC_MR_DComplex
( int height, int width, int colAlignment, int rowAlignment,
  DComplex* buffer, int ldim, Grid g );  

Star_VR_SComplex Register_Star_VR_SComplex
( int height, int width, int rowAlignment,
  SComplex* buffer, int ldim, Grid g );
Star_VR_DComplex Register_Star_VR_DComplex
( int height, int width, int rowAlignment, 
  DComplex* buffer, int ldim, Grid g );

void Print_MC_MR_SComplex( char* msg, MC_MR_SComplex A );
void Print_MC_MR_DComplex( char* msg, MC_MR_DComplex A );
void Print_MC_Star_SComplex( char* msg, MC_Star_SComplex A );
void Print_MC_Star_DComplex( char* msg, MC_Star_DComplex A );
void Print_MD_Star_SComplex( char* msg, MD_Star_SComplex A );
void Print_MD_Star_DComplex( char* msg, MD_Star_DComplex A );
void Print_MR_Star_SComplex( char* msg, MR_Star_SComplex A );
void Print_MR_Star_DComplex( char* msg, MR_Star_DComplex A );
void Print_Star_MC_SComplex( char* msg, Star_MC_SComplex A );
void Print_Star_MC_DComplex( char* msg, Star_MC_DComplex A );
void Print_Star_MR_SComplex( char* msg, Star_MR_SComplex A );
void Print_Star_MR_DComplex( char* msg, Star_MR_DComplex A );
void Print_Star_Star_SComplex( char* msg, Star_Star_SComplex A );
void Print_Star_Star_DComplex( char* msg, Star_Star_DComplex A );
void Print_Star_VC_SComplex( char* msg, Star_VC_SComplex A );
void Print_Star_VC_DComplex( char* msg, Star_VC_DComplex A );
void Print_Star_VR_SComplex( char* msg, Star_VR_SComplex A );
void Print_Star_VR_DComplex( char* msg, Star_VR_DComplex A );
void Print_VC_Star_SComplex( char* msg, VC_Star_SComplex A );
void Print_VC_Star_DComplex( char* msg, VC_Star_DComplex A );
void Print_VR_Star_SComplex( char* msg, VR_Star_SComplex A );
void Print_VR_Star_DComplex( char* msg, VR_Star_DComplex A );
#endif /* WITHOUT_COMPLEX */

/* Utilities */
int LocalLength
( int globalLength, int myIndex, int alignment, int modulus );

/* LAPACK-level interface */
#ifndef WITHOUT_PMRRR
void
HermitianEigDouble
( char uplo, MC_MR_Double A, Star_VR_Double w, MC_MR_Double Z,
  int tryForHighAccuracy );

void
HermitianEigDouble_IntegerSubset
( char uplo, MC_MR_Double A, Star_VR_Double w, MC_MR_Double Z,
  int a, int b, int tryForHighAccuracy );

void
HermitianEigDouble_IntervalSubset
( char uplo, MC_MR_Double A, Star_VR_Double w, MC_MR_Double Z,
  double a, double b, int tryForHighAccuracy );

void
HermitianEigDouble_OnlyEigvals
( char uplo, MC_MR_Double A, Star_VR_Double w, 
  int tryForHighAccuracy );

void
HermitianEigDouble_OnlyEigvals_IntegerSubset
( char uplo, MC_MR_Double A, Star_VR_Double w,
  int a, int b, int tryForHighAccuracy );

void
HermitianEigDouble_OnlyEigvals_IntervalSubset
( char uplo, MC_MR_Double A, Star_VR_Double w,
  double a, double b, int tryForHighAccuracy );

#ifndef WITHOUT_COMPLEX
void
HermitianEigDComplex
( char uplo, MC_MR_DComplex A, Star_VR_Double w, MC_MR_DComplex Z, 
  int tryForHighAccuracy );

void
HermitianEigDComplex_IntegerSubset
( char uplo, MC_MR_DComplex A, Star_VR_Double w, MC_MR_DComplex Z, 
  int a, int b, int tryForHighAccuracy );

void
HermitianEigDComplex_IntervalSubset
( char uplo, MC_MR_DComplex A, Star_VR_Double w, MC_MR_DComplex Z, 
  double a, double b, int tryForHighAccuracy );

void
HermitianEigDComplex_OnlyEigvals
( char uplo, MC_MR_DComplex A, Star_VR_Double w,
  int tryForHighAccuracy );

void
HermitianEigDComplex_OnlyEigvals_IntegerSubset
( char uplo, MC_MR_DComplex A, Star_VR_Double w,
  int a, int b, int tryForHighAccuracy );

void
HermitianEigDComplex_OnlyEigvals_IntervalSubset
( char uplo, MC_MR_DComplex A, Star_VR_Double w,
  double a, double b, int tryForHighAccuracy );
#endif /* WITHOUT_COMPLEX */
#endif /* WITHOUT_PMRRR */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ELEMENTAL_EXPORTS_C_H */

