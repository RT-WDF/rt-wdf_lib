/*
 ==============================================================================

 This file is part of the RT-WDF library.
 Copyright (c) 2015,2016 - Maximilian Rest

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

 wdfSwitchTree.h
 Created:  1 May 2016 1:14:57pm
 Author:  mrest

 ==============================================================================
*/


#pragma once
#include "../Libs/rt-wdf/rt-wdf.h"

using namespace arma;

class wdfTerminatedRtype_RT1 : public wdfTerminatedRtype
{

public:
    //----------------------------------------------------------------------
    wdfTerminatedRtype_RT1 ( wdfTreeNode* left,
                             wdfTreeNode *right ) : wdfTerminatedRtype( {left, right} ) {

    }

    //----------------------------------------------------------------------
    double calculateUpRes( double T )
    {
        const double Rleft  = downPorts[0]->Rp;
        const double Rright = downPorts[1]->Rp;
        const double Rup    = Rleft + Rright;
        return ( Rup );
    }

    //----------------------------------------------------------------------
    void calculateScatterCoeffs( )
    {
        const double Ru = upPort->Rp;
        const double Rl = downPorts[0]->Rp;
        const double Rr = downPorts[1]->Rp;

        const double yu = 1.0;
        const double yl = 2.0 * Rl / ( Ru + Rl + Rr );
        const double yr = 1.0 - yl;

        S->at(0,0) = 1-yu;
        S->at(0,1) =  -yu;
        S->at(0,2) =  -yu;

        S->at(1,0) =  -yl;
        S->at(1,1) = 1-yl;
        S->at(1,2) =  -yl;

        S->at(2,0) =  -yr;
        S->at(2,1) =  -yr;
        S->at(2,2) = 1-yr;


        for( wdfPort* downPort : downPorts ) {
            downPort->connectedNode->calculateScatterCoeffs( );
        }
    }
};


class wdfSwitchTree : public wdfTree {

private:
    //----------------------------------------------------------------------
    const float R1 = 250e3;
    const float R2 = 250e3;

    std::unique_ptr<wdfTerminatedRes>         Res1;
    std::unique_ptr<wdfTerminatedRes>         Res2;
    std::unique_ptr<wdfTerminatedResVSource>  Vres;
    std::unique_ptr<wdfTerminatedSeries>      S1;
    std::unique_ptr<wdfTerminatedParallel>    P1;
    std::unique_ptr<wdfUnterminatedSwitch>    SW1;
    std::unique_ptr<wdfTerminatedRtype>       RT1;

    std::string treeName = "Switchable Attenuator";

public:
    //----------------------------------------------------------------------
    ~wdfSwitchTree() {

    }

    //----------------------------------------------------------------------
    wdfSwitchTree() {

        setSamplerate( 44100 );

        subtreeCount = 1;

        // build up the circuit.
        Vres.reset( new wdfTerminatedResVSource( 0, 1 ) );
        Res1.reset( new wdfTerminatedRes( R1 ) );
        Res2.reset( new wdfTerminatedRes( R2 ) );
        RT1.reset( new wdfTerminatedRtype_RT1( Vres.get(), Res1.get() ) );
        S1.reset( new wdfTerminatedSeries( Vres.get(), Res1.get() ) );
        P1.reset( new wdfTerminatedParallel( S1.get(), Res2.get() ) );
        SW1.reset( new wdfUnterminatedSwitch(0) );
        SW1->setSwitch(1);

        subtreeEntryNodes    = new wdfTreeNode*[subtreeCount];
        subtreeEntryNodes[0] = P1.get();

        // tree stuff
        root.reset( new wdfRootSimple( SW1.get() ) );
        Rp   = new double[subtreeCount]( );

        // params
        paramData SW1Param;
        SW1Param.name    = "Attenuator";
        SW1Param.ID      = 0;
        SW1Param.type    = boolParam;
        SW1Param.value   = 0;
        SW1Param.units   = "On/off";
        SW1Param.lowLim  = 0;
        SW1Param.highLim = 1;
        params.push_back(SW1Param);

        paramData attenParam;
        attenParam.name    = "Attenuation";
        attenParam.ID      = 1;
        attenParam.type    = doubleParam;
        attenParam.value   = 0.5;
        attenParam.units   = "Ratio";
        attenParam.lowLim  = 0;
        attenParam.highLim = 1;
        params.push_back(attenParam);
    }

    //----------------------------------------------------------------------
    const char* getTreeIdentifier( ) {
        return treeName.c_str( );
    }

    //----------------------------------------------------------------------
    int setRootMatrData( matData* rootMats,
                         double* Rp ) {
        return 0;
    }

    //----------------------------------------------------------------------
    void setInputValue( double signalIn ) {
        Vres->Vs = signalIn;
    }

    //----------------------------------------------------------------------
    double getOutputValue( ) {
        return -Res1->upPort->getPortVoltage( );
    }

    //----------------------------------------------------------------------
    void setParam( size_t paramID,
                   double paramValue ) {

        if ( paramID == 0 ) {
            params[0].value = (bool) paramValue;
            SW1->setSwitch( !params[0].value );
        }
        if ( paramID == 1 ) {
            params[1].value = paramValue;
            Res1->R = (R1+R2) * (1 - params[1].value);
            Res2->R = (R1+R2) * params[1].value;
            adaptTree( );
        }
    }

};
