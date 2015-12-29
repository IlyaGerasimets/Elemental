/*
   Copyright (c) 2009-2015, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#include "El.hpp"
using namespace El;

#ifdef EL_HAVE_MPC
typedef BigFloat F;
#elif defined(EL_HAVE_QUAD)
typedef Quad F;
#else
typedef double F;
#endif
typedef Base<F> Real;

int main( int argc, char* argv[] )
{
    Environment env( argc, argv );

    try
    {
        const string filename =
          Input("--filename","input file",string("SVPChallenge40.txt"));
        const Real delta = Input("--delta","delta for LLL",Real(0.99));
        const Real eta =
          Input
          ("--eta","eta for LLL",
           Real(1)/Real(2) + Pow(limits::Epsilon<Real>(),Real(0.9)));
        const bool weak = Input("--weak","use a weak reduction?",false);
        const bool presort = Input("--presort","presort columns?",true);
        const bool smallestFirst =
          Input("--smallestFirst","sort smallest first?",true);
        const bool progress = Input("--progress","print progress?",false); 
        const bool time = Input("--time","time LLL?",false);
        const bool print = Input("--print","output all matrices?",true);
#ifdef EL_HAVE_MPC
        const mpfr_prec_t prec =
          Input("--prec","MPFR precision",mpfr_prec_t(1024));
#endif
        ProcessInput();
        PrintInputReport();

#ifdef EL_HAVE_MPC
        mpc::SetPrecision( prec );
#endif

        Matrix<Real> BTrans, B;
        Read( BTrans, filename );
        Transpose( BTrans, B ); 
        const Real BOrigOne = OneNorm( B ); 
        Output("|| B_orig ||_1 = ",BOrigOne);
        if( print )
            Print( B, "BOrig" );

        // For now, we simply use LLL to attempt to solve the SVP.
        // Obviously, this will not be competitive before making use
        // of Fincke-Pohst
        LLLCtrl<Real> ctrl;
        ctrl.delta = delta;
        ctrl.eta = eta;
        ctrl.weak = weak;
        ctrl.presort = presort;
        ctrl.smallestFirst = smallestFirst;
        ctrl.progress = progress;
        ctrl.time = time;
        Matrix<Real> U, UInv, R;
        const double startTime = mpi::Time();
        auto info = LLL( B, U, UInv, R, ctrl );
        const double runTime = mpi::Time() - startTime;
        Output("  LLL(",delta,",",eta,") took ",runTime," seconds"); 
        Output("    achieved delta: ",info.delta);
        Output("    achieved eta:   ",info.eta);
        Output("    num swaps:      ",info.numSwaps);
        Output("    log(|det B|):   ",info.logAbsDet);
        Output("    GH(B):          ",
          LatticeGaussianHeuristic(info.rank,info.logAbsDet));
        if( print )
        {
            Print( B, "B" ); 
            Print( U, "U" );
            Print( UInv, "UInv" );
            Print( R, "R" );
        }
        const Real BOneNorm = OneNorm( B );
        Output("|| B ||_1 = ",BOneNorm);

        auto b0 = B( ALL, IR(0) );
        const Real b0Norm = FrobeniusNorm( b0 );
        Output("|| b_0 ||_2 = ",b0Norm);
        if( print )
            Print( b0, "b0" );
    }
    catch( std::exception& e ) { ReportException(e); }
    return 0;
}