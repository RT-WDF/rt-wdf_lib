/*
 ==============================================================================

 This file is part of the RT-WDF library.
 Copyright (c) 2015,2016 - Maximilian Rest, Ross Dunkel, Kurt Werner.

 Permission is granted to use this software under the terms of either:
 a) the GPL v2 (or any later version)
 b) the Affero GPL v3

 Details of these licenses can be found at: www.gnu.org/licenses

 RT-WDF is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 -----------------------------------------------------------------------------
 To release a closed-source product which uses RT-WDF, commercial licenses are
 available: write to rt-wdf@e-rm.de for more information.

 ==============================================================================

 rt-wdf_nlSolvers.h
 Created: 2 Dec 2015 4:08:19pm
 Author:  mrest

 ==============================================================================
 */

#ifndef RTWDF_NLSOLVERS_H_INCLUDED
#define RTWDF_NLSOLVERS_H_INCLUDED

//==============================================================================
#include <float.h>

#include "rt-wdf_types.h"
#include "rt-wdf_nlModels.h"



//==============================================================================
// Define enums for solver identifiers


// Iterative:
// TODO: introduce enums!
/** Enum to specify a Newton Solver*/
#define NEWTON_SOLVER   1



//==============================================================================
// Newton Solver config parameters

/** tolerance for ||F||_2 */
#define TOL     1.0e-06                     // TODO: evaluate physically meaningful tolerance.
/** limit on function evaluations */
#define ITMAX   50


//==============================================================================
// Forward declarations
class nlSolver;
class nlNewtonSolver;


//==============================================================================
class nlSolver {

public:
    //----------------------------------------------------------------------
    /**
     Parent class for all non-linear solvers.
    */
    nlSolver();

    //----------------------------------------------------------------------
    /**
     Deconstructor.
    */
    virtual ~nlSolver();

    //----------------------------------------------------------------------
    /**
     Function which returns the number of ports on an NL model for memory house-
     keeping

     @returns                   the total number of non-linear ports which are
                                present at the root.
    */
    int getNumPorts();

    //----------------------------------------------------------------------
    /**
     Virtual function that processes a vector of incoming waves and
     returns a vector of outgoing waves according to the specified
     nonlinearities.

     Must be implemented in an NL solver according to the desired behaviour.

     @param *inWaves            is a pointer to a vector of incoming waves
     @param *outWaves           is a pointer to a vector of outgoing waves
     @param *myMatData          is a pointer to the E,F,M,N (and S) matrices
    */
    virtual void nlSolve( vec* inWaves,
                          vec* outWaves ) = 0;

    //----------------------------------------------------------------------
    /**
     Vector of enums that specify the types on non-linearities in the solver
    */
    std::vector<nlModel*> nlModels;
    /**
     Total number of non-linear ports which are present at the root.
    */
    int numNLPorts;

};


//==============================================================================
class nlNewtonSolver : public nlSolver {

protected:
    //----------------------------------------------------------------------
    /** struct which holds all root NLSS matrices including variable conversion */
    matData* myMatData;
    /** latest guess to solve the NLSS system */
    vec* x0;
    /** result of the NL equations */
    vec* fNL;
    /** Jacobian of the NL equations */
    mat* JNL;
    /** F vector for newton method */
    vec* F;
    /** J matrix for the newton method */
    mat* J;
    /** variable to store Fmat * fNL for x0 next prediction */
    vec* Fmat_fNL;
    /** flag to detect first run of the solver for a clean first initial guess */
    bool firstRun = true;

public:
    //----------------------------------------------------------------------
    /**
     Multi-dimensional Newton Solver class.

     Newton solver as introduced by Michael Jorgen Olsen:
     ("Resolving grouped nonlinearities in wave digital filters using
     iterative techniques"), DAFx-16

     Creates a newton solver and it's nonlinearities

     @param nlList              is a vector of enums that specify the types of
                                nonlinearities
     @param *myMatData          is a pointer to the E,F,M,N (and S) matrices
    */
    nlNewtonSolver( std::vector<int> nlList,
                matData* myMatData );

    /**
     Deconstructor.
    */
    ~nlNewtonSolver( );

    //----------------------------------------------------------------------
    /**
     Solver function that processes a vector of incoming waves and
     returns a vector of outgoing waves according to the specified
     nonlinearities.

     The actual solver operates on J and F matrices which are calculated
     by evalNlModels based on fNL, JNL, Emat and Fmat.

     The result outWaves is calculated based on inWaves, x, Nmat, Mmat.

     @param inWaves             is a pointer to a vector of incoming waves
     @param outWaves            is a pointer to a vector of outgoing waves
    */
    void nlSolve( vec* inWaves,
                  vec* outWaves );

    //----------------------------------------------------------------------
    /**
     Evaluates all non-linear model members of a solver and sets J and F
     members properly.

     It calculates J and F matrices based on both fNL, JNL as returned
     from nlModel.calculate(x,..) and Emat, Fmat from myMatData.

     @param *inWaves            is a pointer to a vector of incoming waves
     @param *myMatData          is a pointer to the E,F,M,N matrices
     @param *x                  is a pointer to the input values x
    */
    void evalNlModels(vec* inWaves, matData* myMatData, vec* x);

};


#endif  // RTWDF_NLSOLVERS_H_INCLUDED
