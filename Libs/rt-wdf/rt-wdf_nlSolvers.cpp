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

 rt-wdf_nlSolvers.cpp
 Created: 2 Dec 2015 4:08:19pm
 Author:  mrest

 ==============================================================================
 */

#include "rt-wdf_nlSolvers.h"

//==============================================================================
// Parent class for nlSolvers
//==============================================================================
nlSolver::nlSolver( ) : numNLPorts( 0 ) {

}

nlSolver::~nlSolver( ) {

}

//----------------------------------------------------------------------
int nlSolver::getNumPorts( ) {
    return numNLPorts;
}


//==============================================================================
// Newton Solver
//==============================================================================
nlNewtonSolver::nlNewtonSolver( std::vector<int> nlList,
                        matData* myMatData ) : myMatData ( myMatData ) {

    // set up Vec<nlModel> nlModels properly according to std::vector<int> nlList
    for( int nlModel : nlList )
    {
        switch( nlModel ) {
            // Diodes:
            case DIODE:             // single diode
            {
                nlModels.push_back(new diodeModel);
                break;
            }
            case DIODE_AP:          // antiparallel diode pair
            {
                nlModels.push_back(new diodeApModel);
                break;
            }
            // Bipolar Transistors:
            case NPN_EM:            // Ebers-Moll npn BJT
            {
                nlModels.push_back(new npnEmModel);
                break;
            }
            // Triode Tubes:
            case TRI_DW:            // Dempwolf triode model
            {
                nlModels.push_back(new triDwModel);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    numNLPorts = 0;
    for ( nlModel* model : nlModels ) {
        numNLPorts += model->getNumPorts();
    }

    x0       = new vec(numNLPorts, fill::zeros);
    F        = new vec(numNLPorts, fill::zeros);
    J        = new mat(numNLPorts,numNLPorts, fill::zeros);
    fNL      = new vec(numNLPorts, fill::zeros);
    JNL      = new mat(numNLPorts,numNLPorts, fill::zeros);
    Fmat_fNL = new vec(numNLPorts, fill::zeros);

}

nlNewtonSolver::~nlNewtonSolver( ) {
    size_t modelCount = nlModels.size();
    for( size_t i = 0; i < modelCount; i++ ) {
        delete nlModels[i];
    }
    delete x0;
    delete F;
    delete J;
    delete fNL;
    delete JNL;
    delete Fmat_fNL;
}

//----------------------------------------------------------------------
void nlNewtonSolver::nlSolve( vec* inWaves,
                          vec* outWaves ) {

    double iter = 0;            // # of iteration
    double alpha = 0;

    (*J).zeros();

    if ( firstRun ) {
        firstRun = false;
    }
    else {
        (*x0) = (*Fmat_fNL) + (myMatData->Emat)*(*inWaves);
    }

    evalNlModels( inWaves, myMatData, x0 );


    double normF = norm(*F);
    //printf("iter alpha         ||F||_2\n");
    //printf(" %3g %9.2e %14.7e\n", iter, alpha, normF);

    vec xnew;
    double normFnew;

    while ( (normF >= TOL) && (iter < ITMAX) )
    {
        vec p = - (*J).i() * (*F);
        alpha = 1;
        xnew = (*x0) + alpha * p;
        evalNlModels(inWaves, myMatData, &xnew);
        normFnew = norm(*F);

        (*x0) = xnew;
        normF = normFnew;
        iter++;

    //        printf(" %3g %9.2e %14.7e\n", iter, alpha, normF);
    }

    (*outWaves) = (myMatData->Mmat) * (*inWaves) + (myMatData->Nmat) * (*fNL);

}

//----------------------------------------------------------------------
void nlNewtonSolver::evalNlModels( vec* inWaves,
                               matData* myMatData,
                               vec* x ) {
    int currentPort = 0;
    (*JNL).zeros();


    for ( nlModel* model : nlModels ) {
        model->calculate( fNL, JNL, x, &currentPort );
    }

    (*Fmat_fNL) = myMatData->Fmat*(*fNL);
    (*F) = (myMatData->Emat)*(*inWaves) + (*Fmat_fNL) - (*x);
    (*J) = (myMatData->Fmat)*(*JNL) - eye(size(*JNL));

}

