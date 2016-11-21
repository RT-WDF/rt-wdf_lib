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

 rt-wdf_nlModels.h
 Created: 2 Dec 2015 4:10:47pm
 Author:  mrest

 ==============================================================================
*/

#ifndef RTWDF_NLMODELS_H_INCLUDED
#define RTWDF_NLMODELS_H_INCLUDED

//==============================================================================
#include "rt-wdf_types.h"

//==============================================================================
// Defines for NL identifiers
// TODO: introduce enums


// Diodes:
/** Enum to specify single diode model in wdfRootNL */
#define DIODE       00
/** Enum to specify anti-parallel diode pair model in wdfRootNL */
#define DIODE_AP    01

// Bipolar Transistors:
/** Enum to specify Ebers-Moll bipolar npn-transistor model in wdfRootNL */
#define NPN_EM      10

// Triode Tubes:
/** Enum to specify Dempwolf triode tube model in wdfRootNL */
#define TRI_DW      20



//==============================================================================
// Forward declarations
class nlModel;
class diodeModel;
class diodeApModel;
class bjtEmModel;
class triDwModel;



//==============================================================================
class nlModel {

public:
    //----------------------------------------------------------------------
    /**
     Parent class for all non-linear models.

     @param numPorts            defines the number of ports that are used to
                                implement a specific NL model.
    */
    nlModel( int numPorts );

    //----------------------------------------------------------------------
    /**
     Deconstructor.
    */
    virtual ~nlModel( );

    //----------------------------------------------------------------------
    /**
     Virtual function to calculate the non-linear current(s) fNL and
     Jacobian JNL according to a vector of input voltage(s).

     @param *fNL             is a pointer to store result of fNL(x).
                             This is a current.
     @param *JNL             is a pointer to store the result of the
                             Jacobian of fNL(x).
     @param *x               is a pointer to read the input value x.
                             This is a voltage.
     @param *currentPort     is a pointer to the first reading /
                             writing position in fNL, JNL and x.
    */
    virtual void calculate( vec* fNL,
                            mat* JNL,
                            vec* x,
                            int* currentPort ) = 0;

    //----------------------------------------------------------------------
    /**
     Function which returns the number of ports on an NL model for memory house-
     keeping

     @returns                   the number of ports implemented in a particular
                                NL model.
    */
    int getNumPorts( );

private:
    //----------------------------------------------------------------------
    /** Stores the number of ports of a model */
    unsigned int numPorts;

};


//==============================================================================
class diodeModel : public nlModel {

public:
    //----------------------------------------------------------------------
    /**
     Single diode model class.

     Single Diode Model according to Kurt Werner et al
     ("An Improved and Generalized Diode Clipper Model for Wave Digital Filters")
    */
    diodeModel( );

    //----------------------------------------------------------------------
    /**
     Calculate the non-linear current and it's Jacobian according to an
     input voltage.

     @param *fNL             is a pointer to store result of fNL(x).
                             This is a current.
     @param *JNL             is a pointer to store the result of the
                             Jacobian of fNL(x).
     @param *x               is a pointer to read the input value x.
                             This is a voltage.
     @param *currentPort     is a pointer to the first reading /
                             writing position in fNL, JNL and x.
    */
    void calculate( vec* fNL,
                    mat* JNL,
                    vec* x,
                    int* currentPort );

};


//==============================================================================
class diodeApModel : public nlModel {

public:
    //----------------------------------------------------------------------
    /**
     Anti-parallel diode pair model class.

     Anti-Parallel Diode Model according to Kurt Werner et al
     ("An Improved and Generalized Diode Clipper Model for Wave Digital Filters")
    */
    diodeApModel();

    //----------------------------------------------------------------------
    /**
     Calculate the non-linear current and it's Jacobian according to an
     input voltage.

     @param *fNL             is a pointer to store result of fNL(x).
                             This is a current.
     @param *JNL             is a pointer to store the result of the
                             Jacobian of fNL(x).
     @param *x               is a pointer to read the input value x.
                             This is a voltage.
     @param *currentPort     is a pointer to the first reading /
                             writing position in fNL, JNL and x.
    */
    void calculate( vec* fNL,
                    mat* JNL,
                    vec* x,
                    int* currentPort );

};


//==============================================================================
class npnEmModel : public nlModel {

public:
    //----------------------------------------------------------------------
    /**
     Ebers-Moll npn-transistor model class.

     Transistor Models using Ebers-Moll equations
     ("Large-signal behavior of junction transistors")
    */
    npnEmModel();

    //----------------------------------------------------------------------
    /**
     Calculate the non-linear current and it's Jacobian according to an
     input voltage.

     @param *fNL             is a pointer to store result of fNL(x).
                             This is a current.
     @param *JNL             is a pointer to store the result of the
                             Jacobian of fNL(x).
     @param *x               is a pointer to read the input value x.
                             This is a voltage.
     @param *currentPort     is a pointer to the first reading /
                             writing position in fNL, JNL and x.
    */
    void calculate( vec* fNL,
                    mat* JNL,
                    vec* x,
                    int* currentPort );

};


//==============================================================================
class triDwModel : public nlModel {

public:
    //----------------------------------------------------------------------
    /**
     Triode tube model class.

     Triode model according to Dempwolf et al
     ("A physically-motivated triode model for circuit simulations")
    */
    triDwModel();

    //----------------------------------------------------------------------
    /**
     Calculate the non-linear current and it's Jacobian according to an
     input voltage.

     @param *fNL             is a pointer to store result of fNL(x).
                             This is a current.
     @param *JNL             is a pointer to store the result of the
                             Jacobian of fNL(x).
     @param *x               is a pointer to read the input value x.
                             This is a voltage.
     @param *currentPort     is a pointer to the first reading /
                             writing position in fNL, JNL and x.
    */
    void calculate( vec* fNL,
                    mat* JNL,
                    vec* x,
                    int* currentPort );

};



#endif  // RTWDF_NLMODELS_H_INCLUDED
