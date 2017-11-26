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

 rt-wdf.cpp
 Created: 26 Oct 2015 11:00:00am
 Author:  mrest

 ==============================================================================
 */

//==============================================================================
#include "rt-wdf.h"
#include <assert.h>

#pragma mark - Tree
//==============================================================================
//                                  T R E E
//==============================================================================
wdfTree::wdfTree( ) {
    root.reset();
    ascendingWaves.reset();
    descendingWaves.reset();
    treeSampleRate  = 1;
}

wdfTree::~wdfTree( ) {

}

//----------------------------------------------------------------------
void wdfTree::cycleWave( ) {
    for( unsigned int i = 0; i < subtreeCount; i++ ) {
        double wave = subtreeEntryNodes[i]->pullWaveUp( );
        (*ascendingWaves)[i] = wave;
    }

    root->processAscendingWaves( ascendingWaves.get(), descendingWaves.get() );

    for( unsigned int i = 0; i < subtreeCount; i++ ) {
        subtreeEntryNodes[i]->pushWaveDown( (*descendingWaves)[i] );
    }
}

//----------------------------------------------------------------------
void wdfTree::initTree( ) {
    ascendingWaves.reset( new vec( subtreeCount ) );
    descendingWaves.reset( new vec( subtreeCount ) );

    for( unsigned int i = 0; i < subtreeCount; i++ ) {
        subtreeEntryNodes[i]->setParentInChildren( );
        subtreeEntryNodes[i]->createPorts( );
    }
}

//----------------------------------------------------------------------
void wdfTree::setSamplerate( double fs ) {
    treeSampleRate  = fs;
}

//----------------------------------------------------------------------
double wdfTree::getSamplerate( ) {
    return treeSampleRate;
}

//----------------------------------------------------------------------
int wdfTree::adaptTree( ) {
    for( unsigned int i = 0; i < subtreeCount; i++ ) {
        subtreeEntryNodes[i]->adaptPorts( treeSampleRate );
        Rp[i] = subtreeEntryNodes[i]->upPort->Rp;
        subtreeEntryNodes[i]->calculateScatterCoeffs( );
    }

    root->setPortResistances( Rp );

    matData* rootMatrixData = root->getRootMatrPtr( );
    if( rootMatrixData != NULL ){
        return setRootMatrData( rootMatrixData, Rp );
    }

    return 0;
}

//----------------------------------------------------------------------
const std::vector<paramData>& wdfTree::getParams( ) {
    return params;
}


#pragma mark - Roots
//==============================================================================
//                                 R O O T S
//==============================================================================
wdfRoot::wdfRoot( ) {

}

wdfRoot::~wdfRoot( ) {

}

//----------------------------------------------------------------------
void wdfRoot::setPortResistances( double *Rp ) {
    //do nothing here, might be implemented by a subclass of wdfRoot..
}

matData* wdfRoot::getRootMatrPtr( ) {
    return NULL;
}

#pragma mark R-type Root
//==============================================================================
wdfRootRtype::wdfRootRtype( int numSubtrees ) : wdfRoot(),
                                                numSubtrees(numSubtrees) {
    rootMatrixData.reset( new matData );
    rootMatrixData->Smat.set_size( numSubtrees, numSubtrees );
    rootMatrixData->Emat.set_size(0, 0);
    rootMatrixData->Fmat.set_size(0, 0);
    rootMatrixData->Mmat.set_size(0, 0);
    rootMatrixData->Nmat.set_size(0, 0);
}

wdfRootRtype::~wdfRootRtype( ) {
    rootMatrixData.reset();
}

//----------------------------------------------------------------------
void wdfRootRtype::processAscendingWaves( vec* ascendingWaves,
                                          vec* descendingWaves ) {
    (*descendingWaves) = rootMatrixData->Smat * (*ascendingWaves);
}

//----------------------------------------------------------------------
matData* wdfRootRtype::getRootMatrPtr( ) {
    return rootMatrixData.get();
}

//----------------------------------------------------------------------
std::string wdfRootRtype::getType( ) const {
    return "Root (R-type)";
}


#pragma mark Non-Linear Root
//==============================================================================
wdfRootNL::wdfRootNL( int numSubtrees,
                      std::vector<int> nlList,
                      int solverType ) : wdfRoot( ),
                                         numSubtrees( numSubtrees ) {
    rootMatrixData.reset( new matData );

    // TODO make ENUM / MAP variant with different nlSolvers (!!)
    NlSolver.reset( new nlNewtonSolver( nlList, rootMatrixData.get() ) );
    int numNonlinearities = NlSolver->getNumPorts( );

    rootMatrixData->Smat.set_size( numSubtrees+numNonlinearities, numSubtrees+numNonlinearities );
    rootMatrixData->Emat.set_size( numNonlinearities, numSubtrees );
    rootMatrixData->Fmat.set_size( numNonlinearities, numNonlinearities );
    rootMatrixData->Mmat.set_size( numSubtrees, numSubtrees );
    rootMatrixData->Nmat.set_size( numSubtrees, numNonlinearities );
}

wdfRootNL::~wdfRootNL( ) {

}

//----------------------------------------------------------------------
void wdfRootNL::processAscendingWaves( vec* ascendingWaves,
                                       vec* descendingWaves ) {
    NlSolver->nlSolve( ascendingWaves, descendingWaves );
}

//----------------------------------------------------------------------
matData* wdfRootNL::getRootMatrPtr( ) {
    return rootMatrixData.get();
}

//----------------------------------------------------------------------
std::string wdfRootNL::getType( ) const {
    return "Root (NL-type)";
}


#pragma mark Simple Root
//==============================================================================
wdfRootSimple::wdfRootSimple( wdfRootNode* rootElement ): wdfRoot( ),
                                                          rootElement( rootElement ) {

}

//----------------------------------------------------------------------
void wdfRootSimple::setPortResistances( double * Rp ) {
    rootElement->setPortResistance( Rp[0] );
}

//----------------------------------------------------------------------
void wdfRootSimple::processAscendingWaves( vec* ascendingWaves,
                                           vec* descendingWaves ) {
    size_t idx = 0;
    rootElement->calculateDownB( ascendingWaves, descendingWaves, &idx );
}

//----------------------------------------------------------------------
std::string wdfRootSimple::getType( ) const {
    return "Root (Simple-type)";
}


#pragma mark - Wave Port -
//==============================================================================
//                                  P O R T
//==============================================================================
wdfPort::wdfPort( wdfTreeNode *connectedNode ) : Rp( 0 ),
                                                 Gp( 0 ),
                                                 a( 0 ),
                                                 b( 0 ),
                                                 connectedNode( connectedNode ) {

}

//----------------------------------------------------------------------
double wdfPort::getPortVoltage( ) {
    return ( a + b ) / 2.0;
}

//----------------------------------------------------------------------
double wdfPort::getPortCurrent( ){
    return ( a - b ) / ( 2.0 * Rp );
}



#pragma mark - Tree Node -
//==============================================================================
//                             T R E E   N O D E
//==============================================================================

wdfTreeNode::wdfTreeNode( ) {
    upPort.reset( new wdfPort( NULL ) );
}

wdfTreeNode::wdfTreeNode( wdfTreeNode *left,
                          wdfTreeNode *right ) {
    childrenNodes.push_back( left );
    childrenNodes.push_back( right );
    upPort.reset( new wdfPort( NULL ) );
}

wdfTreeNode::wdfTreeNode( std::vector<wdfTreeNode*> childrenIn ) {
    for ( wdfTreeNode* child : childrenIn ) {
        childrenNodes.push_back( child );
    }
    upPort.reset( new wdfPort( NULL ) );
}

//----------------------------------------------------------------------
void wdfTreeNode::setParentInChildren( ) {
    for (wdfTreeNode* child : childrenNodes) {
        child->parentNode = this ;
        child->setParentInChildren( );
    }
}

//----------------------------------------------------------------------
void wdfTreeNode::createPorts( ) {
    for( unsigned int i = 0; i < childrenNodes.size(); i++) {
        downPorts.push_back( new wdfPort( childrenNodes[i] ) );
        childrenNodes[i]->upPort->connectedNode = this;
        childrenNodes[i]->createPorts( );
    }
}

//----------------------------------------------------------------------
double wdfTreeNode::adaptPorts( double sampleRate ) {
    for( wdfPort* downPort : downPorts ) {
        downPort->Rp = downPort->connectedNode->adaptPorts( sampleRate );
    }

    upPort->Rp = calculateUpRes( sampleRate );
    return upPort->Rp;
}

//----------------------------------------------------------------------
double wdfTreeNode::pullWaveUp( ) {
    for( wdfPort* downPort : downPorts ) {
        downPort->a = downPort->connectedNode->pullWaveUp( );
    }

    upPort->b = calculateUpB( );
    return upPort->b;
}

//----------------------------------------------------------------------
void wdfTreeNode::pushWaveDown( double descendingWave ) {
    upPort->a = descendingWave;
    calculateDownB( descendingWave );
    for( wdfPort* downPort : downPorts ) {
        downPort->connectedNode->pushWaveDown( downPort->b );
    }
}


#pragma mark - Terminated Adapters -
//==============================================================================
//              T E R M I N A T E D   A D A P T E R S   (S, P, R)
//==============================================================================
wdfTerminatedAdapter::wdfTerminatedAdapter( wdfTreeNode *left,
                                            wdfTreeNode *right ) : wdfTreeNode( left, right ) {
}

wdfTerminatedAdapter::wdfTerminatedAdapter(std::vector<wdfTreeNode*> childrenIn) : wdfTreeNode( childrenIn ) {

}


#pragma mark Terminated R-type Adapter
//==============================================================================
wdfTerminatedRtype::wdfTerminatedRtype( std::vector<wdfTreeNode*> childrenIn ) : wdfTerminatedAdapter( childrenIn ) {
    S.reset( new mat( childrenIn.size()+1, childrenIn.size()+1 ) );
}


wdfTerminatedRtype::~wdfTerminatedRtype( ) {

}

//  IMPLEMET THESE METHODS IN THE CIRCUIT TREE
// //----------------------------------------------------------------------
// double wdfTerminatedRtype::calculateUpRes( double sampleRate )
// {
//     const double Rup = YOUR CODE HERE
//     return ( Rup );
// }
//
// //----------------------------------------------------------------------
// void wdfTerminatedRtype::calculateScatterCoeffs( )
// {
//     YOUR CODE HERE
//
//     for (int i = 0; i < childNodeCount; i++) {
//         downPorts[i]->connectedNode->calculateScatterCoeffs( );
//     }
// }

//----------------------------------------------------------------------
double wdfTerminatedRtype::calculateUpB( )
{
    colvec inWaves( childrenNodes.size()+1 );
    inWaves(0) = 0;
    for( unsigned int i = 0; i < childrenNodes.size(); i++ ) {
        inWaves(i+1) = downPorts[i]->a;
    }
    double upB = as_scalar( S->row(0) * inWaves );

    return upB;
}

//----------------------------------------------------------------------
void wdfTerminatedRtype::calculateDownB( double descendingWave )
{
    colvec inWaves( childrenNodes.size()+1 );
    inWaves(0) = descendingWave;
    for( unsigned int i = 0; i < childrenNodes.size(); i++ ) {
        inWaves(i+1) = downPorts[i]->a;
    }

    colvec downWaves = (*S) * inWaves;

    for( unsigned int i = 0; i < childrenNodes.size(); i++ ) {
        downPorts[i]->b = downWaves(i+1);
    }
    int i=1;
    i++;
}

//----------------------------------------------------------------------
std::string wdfTerminatedRtype::getType( ) const
{
    return "R-type Adapter (TOP adapted)";
}


#pragma mark Terminated Series Adapter
//==============================================================================
wdfTerminatedSeries::wdfTerminatedSeries( wdfTreeNode *left,
                                          wdfTreeNode *right ) : wdfTerminatedAdapter(left, right),
                                                                 yu( 0 ),
                                                                 yl( 0 ),
                                                                 yr( 0 ) {

}

//----------------------------------------------------------------------
double wdfTerminatedSeries::calculateUpRes( double sampleRate ) {
    const double Rleft  = downPorts[0]->Rp;
    const double Rright = downPorts[1]->Rp;
    const double Rser   = Rleft + Rright;
    return ( Rser );
}

//----------------------------------------------------------------------
void wdfTerminatedSeries::calculateScatterCoeffs( ) {
    const double Ru = upPort->Rp;
    const double Rl = downPorts[0]->Rp;
    const double Rr = downPorts[1]->Rp;

    yu = 1.0;
    yl = 2.0 * Rl / ( Ru + Rl + Rr );
    yr = 1.0 - yl;

    for ( wdfPort* downPort : downPorts ) {
        downPort->connectedNode->calculateScatterCoeffs( );
    }
}

//----------------------------------------------------------------------
double wdfTerminatedSeries::calculateUpB( ) {
    double upB = -( downPorts[0]->a + downPorts[1]->a );
    return upB;
}

//----------------------------------------------------------------------
void wdfTerminatedSeries::calculateDownB( double descendingWave ) {
    downPorts[0]->b = yl * ( downPorts[0]->a * ((1.0 / yl) - 1) - downPorts[1]->a - descendingWave );
    downPorts[1]->b = yr * ( downPorts[1]->a * ((1.0 / yr) - 1) - downPorts[0]->a - descendingWave );
}

//----------------------------------------------------------------------
std::string wdfTerminatedSeries::getType( ) const {
    return "Series Adapter (TOP adapted)";
}


#pragma mark Terminated Parallel Adapter
//==============================================================================
wdfTerminatedParallel::wdfTerminatedParallel( wdfTreeNode *left,
                                              wdfTreeNode *right ) : wdfTerminatedAdapter(left, right),
                                                                     du( 0 ),
                                                                     dl( 0 ),
                                                                     dr( 0 ) {

}

//----------------------------------------------------------------------
double wdfTerminatedParallel::calculateUpRes( double sampleRate ) {
    const double Rleft  = downPorts[0]->Rp;
    const double Rright = downPorts[1]->Rp;

    assert(Rleft > 0 && "Port resistance must be a nonzero positive number.");
    assert(Rright > 0 && "Port resistance must be a nonzero positive number.");
    
    const double Rpar   = ( Rleft * Rright ) / ( Rleft + Rright );
    return Rpar;
}

//----------------------------------------------------------------------
void wdfTerminatedParallel::calculateScatterCoeffs( ) {
    const double Gu = 1.0 / upPort->Rp;
    const double Gl = 1.0 / downPorts[0]->Rp;
    const double Gr = 1.0 / downPorts[1]->Rp;

    du = 1.0;
    dl = 2.0 * Gl / ( Gu + Gl + Gr );
    dr = 1.0 - dl;

    for ( wdfPort* downPort : downPorts ) {
        downPort->connectedNode->calculateScatterCoeffs( );
    }
}

//----------------------------------------------------------------------
double wdfTerminatedParallel::calculateUpB( ) {
    return ( dl * downPorts[0]->a + dr * downPorts[1]->a );
}

//----------------------------------------------------------------------
void wdfTerminatedParallel::calculateDownB( double descendingWave ) {
    downPorts[0]->b = ( ( dl - 1 ) * downPorts[0]->a + dr * downPorts[1]->a + du * descendingWave );
    downPorts[1]->b = ( dl * downPorts[0]->a + ( dr - 1 ) * downPorts[1]->a + du * descendingWave );
}

//----------------------------------------------------------------------
std::string wdfTerminatedParallel::getType( ) const {
    return "Parallel Adapter (TOP adapted)";
}

#pragma mark Inverter
//==============================================================================
wdfInverter::wdfInverter( wdfTreeNode *child ) :  wdfTerminatedAdapter( { child } ) {
}

//----------------------------------------------------------------------
double wdfInverter::calculateUpRes( double sampleRate ) {
    return downPorts[0]->Rp;
}

//----------------------------------------------------------------------
void wdfInverter::calculateScatterCoeffs( ) {
    for ( wdfPort* downPort : downPorts ) {
        downPort->connectedNode->calculateScatterCoeffs( );
    }
}

//----------------------------------------------------------------------
double wdfInverter::calculateUpB( ) {
    return -1 * downPorts[0]->a ;
}

//----------------------------------------------------------------------
void wdfInverter::calculateDownB( double descendingWave ) {
    downPorts[0]->b = -1 * descendingWave;
}

//----------------------------------------------------------------------
std::string wdfInverter::getType( ) const {
    return "Inverter (TOP adapted)";
}

#pragma mark - Terminated Leafs -
//==============================================================================
//                    T E R M I N A T E D   L E A F S   (Q)
//==============================================================================
wdfTerminatedLeaf::wdfTerminatedLeaf( ) : wdfTreeNode( ) {
}

//----------------------------------------------------------------------
void wdfTerminatedLeaf::calculateScatterCoeffs( ) {
    // this is the end of the recursive updateScatter() descent within the tree
};


#pragma mark Terminated Capacitor
//==============================================================================
wdfTerminatedCap::wdfTerminatedCap( double C,
                                    double sampleRate ) : wdfTerminatedLeaf( ),
                                                 C( C ),
                                                 sampleRate( sampleRate ),
                                                 prevA( 0 ) {

}

//----------------------------------------------------------------------
double wdfTerminatedCap::calculateUpRes( double sampleRate ) {
    assert(sampleRate > 0 && "sampleRate must be a nonzero positive number.");
    assert(C > 0 && "capacitance must be a nonzero positive number.");

    this->sampleRate = sampleRate;
    const double R = 1 / ( 2.0 * sampleRate * C );
    return R;
}

//----------------------------------------------------------------------
double wdfTerminatedCap::calculateUpB( ) {
    return prevA;
}

//----------------------------------------------------------------------
void wdfTerminatedCap::calculateDownB( double descendingWave ) {
    prevA = descendingWave;
}

//----------------------------------------------------------------------
std::string wdfTerminatedCap::getType( ) const {
    return "C (adapted)";
}

#pragma mark Terminated Inductor
//==============================================================================
wdfTerminatedInd::wdfTerminatedInd( double L,
                                    double sampleRate ) : wdfTerminatedLeaf( ),
                                                 L( L ),
                                                 sampleRate( sampleRate ),
                                                 prevA( 0 ) {

}

//----------------------------------------------------------------------
double wdfTerminatedInd::calculateUpRes( double sampleRate ) {
    assert(sampleRate > 0 && "sampleRate must be a nonzero positive number.");
    assert(L > 0 && "inductance must be a nonzero positive number.");

    this->sampleRate = sampleRate;
    const double R = 2.0 * sampleRate * L ;
    return R;
}

//----------------------------------------------------------------------
double wdfTerminatedInd::calculateUpB( ) {
    return prevA;
}

//----------------------------------------------------------------------
void wdfTerminatedInd::calculateDownB( double descendingWave ) {
    prevA = -1.0 * descendingWave;
}

//----------------------------------------------------------------------
std::string wdfTerminatedInd::getType( ) const {
    return "L (adapted)";
}

#pragma mark Terminated Resistor
//==============================================================================
wdfTerminatedRes::wdfTerminatedRes( double R ) : wdfTerminatedLeaf( ),
                                                 R( R ) {

}

//----------------------------------------------------------------------
double wdfTerminatedRes::calculateUpRes( double sampleRate ) {
    return R;
}

//----------------------------------------------------------------------
double wdfTerminatedRes::calculateUpB( ) {
    return 0.0;
}

//----------------------------------------------------------------------
void wdfTerminatedRes::calculateDownB( double descendingWave ) {
    // do nothing, R is terminated/adapted!
}

//----------------------------------------------------------------------
std::string wdfTerminatedRes::getType( ) const {
    return "R (adapted)";
}

#pragma mark Terminated Resistive VSource
//==============================================================================
wdfTerminatedResVSource::wdfTerminatedResVSource( double Vs,
                                                  double RSer ) : wdfTerminatedLeaf( ),
                                                                  Vs( Vs ),
                                                                  RSer( RSer ) {

}

//----------------------------------------------------------------------
double wdfTerminatedResVSource::calculateUpRes( double sampleRate ) {
    return RSer;
}

//----------------------------------------------------------------------
double wdfTerminatedResVSource::calculateUpB( ) {
    return Vs;
}

//----------------------------------------------------------------------
void wdfTerminatedResVSource::calculateDownB( double descendingWave ) {
    // do nothing, VResVolt is terminated/adapted!
}

//----------------------------------------------------------------------
std::string wdfTerminatedResVSource::getType( ) const {
    return "Vs (incl. Rp = RSer -> adapted)";
}

#pragma mark Terminated Resistive CSource
//==============================================================================
wdfTerminatedResCSource::wdfTerminatedResCSource( double Is,
                                                  double RPar ) : wdfTerminatedLeaf( ),
                                                                  Is( Is ),
                                                                  RPar ( RPar ) {

}

//----------------------------------------------------------------------
double wdfTerminatedResCSource::calculateUpRes( double sampleRate ) {
    return RPar;
}

//----------------------------------------------------------------------
double wdfTerminatedResCSource::calculateUpB( ) {
    return RPar * Is;
}

//----------------------------------------------------------------------
void wdfTerminatedResCSource::calculateDownB( double descendingWave ) {
    // do nothing, node is terminated/adapted!
}

//----------------------------------------------------------------------
std::string wdfTerminatedResCSource::getType( ) const {
    return "Cs (incl. Rp = RPar -> adapted)";
}

#pragma mark - Unterminated Root Node -
//==============================================================================
//                             R O O T   N O D E
//==============================================================================
wdfRootNode::wdfRootNode( int numPorts ) {
    this->numPorts = numPorts;
}
//----------------------------------------------------------------------
wdfRootNode::~wdfRootNode( ) {

}

//----------------------------------------------------------------------
void wdfRootNode::setPortResistance( double Rp ) {
    this->Rp = Rp;
}

int wdfRootNode::getNumPorts( ){
    return numPorts;
}

#pragma mark Unterminated Switch
//==============================================================================
//                  U N T E R M I N A T E D   E L E M E N T S
//==============================================================================
wdfUnterminatedSwitch::wdfUnterminatedSwitch( int position ) : wdfRootNode( 1 ),
                                                               position( position ) {

}

//----------------------------------------------------------------------
void wdfUnterminatedSwitch::calculateDownB( vec* ascendingWaves,
                                            vec* descendingWaves,
                                            size_t* portIndex ) {
    size_t idx   = (*portIndex);
    (*portIndex) = idx+numPorts;

    if( position == 0 ) // open
    {
        (*descendingWaves)[idx] = +1.0 * (*ascendingWaves)[idx];
    }
    else                // closed
    {
        (*descendingWaves)[idx] = -1.0 * (*ascendingWaves)[idx];
    }
}

//----------------------------------------------------------------------
void wdfUnterminatedSwitch::setSwitch( int position ) {
    this->position = position;
}

//----------------------------------------------------------------------
std::string wdfUnterminatedSwitch::getType( ) const {
    return "SW (unadapted)";
}

#pragma mark Unterminated Capacitor
//==============================================================================
wdfUnterminatedCap::wdfUnterminatedCap(double C,
                                       double sampleRate ) : wdfRootNode(1),
                                                    sampleRate(sampleRate),
                                                    prevA(0),
                                                    prevB(0),
                                                    C(C) {

}


//----------------------------------------------------------------------
void wdfUnterminatedCap::calculateDownB( vec* ascendingWaves,
                                         vec* descendingWaves,
                                         size_t* portIndex) {
    descendingWaves->at(*portIndex) = reflectionCoeff * prevB - reflectionCoeff * ascendingWaves->at(*portIndex) + prevA;
    prevB = descendingWaves->at(*portIndex);
    prevA = ascendingWaves->at(*portIndex);
    (*portIndex) += numPorts;
}

//----------------------------------------------------------------------
std::string wdfUnterminatedCap::getType( ) const {
    return "C (unadapted)";
}

void wdfUnterminatedCap::setPortResistance( double Rp ) {
    this->Rp = Rp;
    reflectionCoeff = (Rp - 1 / (2 * sampleRate * C)) / (Rp + (1 / (2 * sampleRate * C)));
}

#pragma mark Unterminated Inductor
//==============================================================================
wdfUnterminatedInd::wdfUnterminatedInd( double L,
                                        double sampleRate ) : wdfRootNode(1),
                                                     sampleRate(sampleRate),
                                                     prevA(0),
                                                     prevB(0),
                                                     L(L) {

}


//----------------------------------------------------------------------
void wdfUnterminatedInd::calculateDownB( vec* ascendingWaves,
                                         vec* descendingWaves,
                                         size_t* portIndex) {
    descendingWaves->at(*portIndex) = -reflectionCoeff * prevB - reflectionCoeff * ascendingWaves->at(*portIndex) - prevA;
    prevB = descendingWaves->at(*portIndex);
    prevA = ascendingWaves->at(*portIndex);
    (*portIndex) += numPorts;
}

//----------------------------------------------------------------------
std::string wdfUnterminatedInd::getType( ) const {
    return "L (unadapted)";
}

void wdfUnterminatedInd::setPortResistance( double Rp ) {
    this->Rp = Rp;
    reflectionCoeff = (Rp - 2 * sampleRate * L) / (Rp + 2 * sampleRate * L);
}


#pragma mark Unterminated Resistor
//==============================================================================
wdfUnterminatedRes::wdfUnterminatedRes( double R ) : wdfRootNode(1),
                                                     R(R) {

}

//----------------------------------------------------------------------
void wdfUnterminatedRes::calculateDownB( vec* ascendingWaves,
                                         vec* descendingWaves,
                                         size_t* portIndex) {
    descendingWaves->at(*portIndex) = reflectionCoeff * ascendingWaves->at(*portIndex);
    (*portIndex) += numPorts;
}

//----------------------------------------------------------------------
std::string wdfUnterminatedRes::getType( ) const {
    return "R (unadapted)";
}

void wdfUnterminatedRes::setPortResistance( double Rp ) {
    this->Rp = Rp;
    reflectionCoeff = (R - Rp) / (R + Rp);
}


#pragma mark Ideal Voltage Source
//==============================================================================
wdfIdealVSource::wdfIdealVSource( double Vs ) : wdfRootNode(1),
                                                Vs(Vs) {

}

//----------------------------------------------------------------------
void wdfIdealVSource::calculateDownB( vec* ascendingWaves,
                                            vec* descendingWaves,
                                            size_t* portIndex) {
    descendingWaves->at(*portIndex) = 2 * Vs - ascendingWaves->at(*portIndex);
    (*portIndex) += numPorts;
}

//----------------------------------------------------------------------
std::string wdfIdealVSource::getType( ) const {
    return "Vs (ideal -> unadapted)";
}

//----------------------------------------------------------------------
void wdfIdealVSource::setPortResistance( double Rp ) {
    this->Rp = Rp;
}

#pragma mark Ideal Current Source
//==============================================================================
wdfIdealCSource::wdfIdealCSource( double Is ) : wdfRootNode(1),
                                                  Is(Is) {

}

//----------------------------------------------------------------------
void wdfIdealCSource::calculateDownB( vec* ascendingWaves,
                                      vec* descendingWaves,
                                      size_t* portIndex) {
    descendingWaves->at(*portIndex) = 2 * Rp * Is + ascendingWaves->at(*portIndex);
    (*portIndex) += numPorts;
}

//----------------------------------------------------------------------
std::string wdfIdealCSource::getType( ) const {
    return "Is (ideal -> unadapted)";
}

//----------------------------------------------------------------------
void wdfIdealCSource::setPortResistance( double Rp ) {
    this->Rp = Rp;
}

