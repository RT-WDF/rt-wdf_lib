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

 rt-wdf_nlModels.cpp
 Created: 2 Dec 2015 4:10:47pm
 Author:  mrest

 ==============================================================================
*/

#include "rt-wdf_nlModels.h"


//==============================================================================
// Parent class for nlModels
//==============================================================================
nlModel::nlModel( int numPorts ) : numPorts (numPorts) {

}

nlModel::~nlModel( ) {

}

//----------------------------------------------------------------------
int nlModel::getNumPorts( ) {
    return numPorts;
}


//==============================================================================
// Diode Models according to Kurt Werner et al
// ("An Improved and Generalized Diode Clipper Model for Wave Digital Filters")
//==============================================================================
#define Is_DIODE    2.52e-9
#define VT_DIODE    0.02585

diodeModel::diodeModel() : nlModel( 1 ) {

}

//----------------------------------------------------------------------
void diodeModel::calculate( vec* fNL,
                            mat* JNL,
                            vec* x,
                            int* currentPort ) {

    const double vd = (*x)(*currentPort);
    const double arg1 = vd/VT_DIODE;

    (*fNL)(*currentPort) = Is_DIODE*(exp(arg1)-1);
    (*JNL)(*currentPort,*currentPort) = (Is_DIODE/VT_DIODE)*exp(arg1);

    (*currentPort) = (*currentPort)+getNumPorts();
}

//==============================================================================
diodeApModel::diodeApModel( ) : nlModel( 1 ) {

}

//----------------------------------------------------------------------
void diodeApModel::calculate( vec* fNL,
                              mat* JNL,
                              vec* x,
                              int* currentPort) {

    const double vd = (*x)(*currentPort);
    const double arg1 = vd/VT_DIODE;

    (*fNL)(*currentPort) = Is_DIODE*(exp(arg1)-1)-Is_DIODE*(exp(-arg1)-1);
    (*JNL)(*currentPort,*currentPort) = (Is_DIODE/VT_DIODE)*(exp(arg1)+exp(-arg1));

    (*currentPort) = (*currentPort)+getNumPorts();
}


//==============================================================================
// Transistor Models using Ebers-Moll equations
// ("Large-signal behavior of junction transistors")
//==============================================================================
#define Is_BJT      5.911e-15
#define VT_BJT      0.02585
#define BETAF       1.434e3
#define BETAR       1.262
#define ALPHAF      (BETAF/(1.0+BETAF))     //TAKE CARE OF ( ) TO COMPILE CORRECTLY!!!!!! ARGHH!!
#define ALPHAR      (BETAR/(1.0+BETAR))     //TAKE CARE OF ( ) TO COMPILE CORRECTLY!!!!!!


npnEmModel::npnEmModel() : nlModel( 2 ) {

}

//----------------------------------------------------------------------
void npnEmModel::calculate( vec* fNL,
                            mat* JNL,
                            vec* x,
                            int* currentPort) {

    const double vBC = (*x)(*currentPort);
    const double vBE = (*x)((*currentPort)+1);

    const double vBC_o_VT_BJT = vBC/VT_BJT;
    const double vBE_o_VT_BJT = vBE/VT_BJT;
    const double Is_BJT_o_VT_BJT = Is_BJT/VT_BJT;
    const double Is_BJT_o_ALPHAR = Is_BJT/ALPHAR;
    const double Is_BJT_o_ALPHAF = Is_BJT/ALPHAF;


    (*fNL)(*currentPort) = -Is_BJT*(exp(vBE_o_VT_BJT )-1)+(Is_BJT_o_ALPHAR)*(exp(vBC_o_VT_BJT)-1);
    (*JNL)((*currentPort),(*currentPort)) = (Is_BJT_o_ALPHAR/VT_BJT)*exp(vBC_o_VT_BJT);
    (*JNL)((*currentPort),((*currentPort)+1)) = (-Is_BJT_o_VT_BJT)*exp(vBE_o_VT_BJT );

    (*fNL)((*currentPort)+1) = (Is_BJT_o_ALPHAF)*(exp(vBE_o_VT_BJT )-1)-Is_BJT*(exp(vBC_o_VT_BJT)-1);
    (*JNL)(((*currentPort)+1),(*currentPort)) = (-Is_BJT_o_VT_BJT)*exp(vBC_o_VT_BJT);
    (*JNL)(((*currentPort)+1),((*currentPort)+1)) = (Is_BJT_o_ALPHAF/VT_BJT)*exp(vBE_o_VT_BJT );

    (*currentPort) = (*currentPort)+getNumPorts();
}


//==============================================================================
// Triode model according to Dempwolf et al
// ("A physically-motivated triode model for circuit simulations")
//==============================================================================
triDwModel::triDwModel() : nlModel( 2 ) {


}

//----------------------------------------------------------------------
void triDwModel::calculate( vec* fNL,
                            mat* JNL,
                            vec* x,
                            int* currentPort) {

    const double G = 2.242E-3;
    const double C = 3.40;
    const double mu = 103.2;
    const double y = 1.26;

    const double Gg = 6.177E-4;
    const double Cg = 9.901;
    const double E = 1.314;
    const double Ig0 = 8.025E-8;

    const double vAC_mu = (*x)(*currentPort) / mu;
    const double vGC = (*x)((*currentPort)+1);


    const double exp_Cg_vGC = exp( Cg * vGC );
    const double log_1_exp_Cg_vGC_Cg = (log( 1 + exp_Cg_vGC ) / Cg);


    // Ig
    (*fNL)((*currentPort)+1) = Gg * pow( log_1_exp_Cg_vGC_Cg, E ) + Ig0;

    // dIg / dvAC
    (*JNL)(((*currentPort)+1),(*currentPort)) = 0;
    // dIg / dvGC
    (*JNL)(((*currentPort)+1),((*currentPort)+1)) = ( Gg * E * exp_Cg_vGC *
                                                      pow( log_1_exp_Cg_vGC_Cg, (E-1)) ) /
                                                    (1 + exp_Cg_vGC);


    const double exp_C_vAC_mu_vGC = exp( C * ( vAC_mu + vGC ));
    const double pow_log_1_exp_C_vAC_mu_vGC_C_y_1 = pow( (log(1 + exp_C_vAC_mu_vGC) / C), (y-1));

    // Ik
    (*fNL)(*currentPort) = G * pow( log( 1 + exp_C_vAC_mu_vGC ) / C , y ) - (*fNL)((*currentPort)+1);

    // dIk / dvAC
    (*JNL)((*currentPort),(*currentPort)) = ( G * y * exp_C_vAC_mu_vGC *
                                              pow_log_1_exp_C_vAC_mu_vGC_C_y_1 ) /
                                            (mu * (1 + exp_C_vAC_mu_vGC));
    // dIk / dvGC
    (*JNL)((*currentPort),((*currentPort)+1)) = ( G * y * exp_C_vAC_mu_vGC *
                                                  pow_log_1_exp_C_vAC_mu_vGC_C_y_1 ) /
                                                (1 + exp_C_vAC_mu_vGC) - (*JNL)(((*currentPort)+1),((*currentPort)+1));


    (*currentPort) = (*currentPort)+getNumPorts();

}
