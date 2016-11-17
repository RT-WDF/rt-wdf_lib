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

 rt-wdf.h
 Created: 26 Oct 2015 11:00:00am
 Author:  mrest

 ==============================================================================
 */

#ifndef __RTWDF_H__
#define __RTWDF_H__

//==============================================================================
#include <memory>



//==============================================================================
// include nlSolverClass for nonlinear solvers
#include "rt-wdf_types.h"
#include "rt-wdf_nlSolvers.h"


//==============================================================================
// Forward declarations

// WDF TREE
class wdfTree;

// ROOTS:
class wdfRoot;
    class wdfRootRtype;
    class wdfRootNL;
    class wdfRootSimple;

// UTILITIES:
class wdfPort;

// WDF TREE ELEMENTS
class wdfTreeNode;
    class wdfTerminatedAdapter;
        class wdfTerminatedRtype;
        class wdfTerminatedSeries;
        class wdfTerminatedParallel;
        class wdfInverter;
    class wdfTerminatedLeaf;
        class wdfTerminatedCap;
        class wdfTerminatedInd;
        class wdfTerminatedRes;
        class wdfTerminatedResVSource;

// WDF ROOT ELEMENTS
class wdfRootNode;
    class wdfUnterminatedSwitch;
    class wdfUnterminatedCap;
    class wdfUnterminatedInd;
    class wdfUnterminatedRes;
    class wdfIdealVSource;
    class wdfIdealCSource;


#pragma mark - Tree
//==============================================================================
//                                  T R E E
//==============================================================================
class wdfTree {

public:
    //----------------------------------------------------------------------
    /**
     Base class for a WDF tree.

     This class is always user-extended by a subclass that specifies the
     implementation of a WDF structure. It provides a standard interface for
     initialization and operation of a WDF structure towards the owner of
     the tree object.
     */
    wdfTree( );

    //----------------------------------------------------------------------
    /**
     Deconstructor
     */
    virtual ~wdfTree( );

protected:
    //----------------------------------------------------------------------
    /**
     Pointer to the root object of this tree.
     */
    std::unique_ptr<wdfRoot> root;

    //----------------------------------------------------------------------
    /**
     Array of pointers to the first nodes in the subtrees that hang off
     the root.
     */
    wdfTreeNode **subtreeEntryNodes;

    //----------------------------------------------------------------------
    /**
     Pointer to a vector of ascending waves towards the root.
     */
    std::unique_ptr<vec> ascendingWaves;

    //----------------------------------------------------------------------
    /**
     Pointer to a vector of descending waves from the root.
     */
    std::unique_ptr<vec> descendingWaves;

    //----------------------------------------------------------------------
    /**
     Samplerate of the tree.

     Must be set correctly in the user-specific wdfTree application by
     setSamplerate()
     */
    double treeSampleRate;

    //----------------------------------------------------------------------
    /**
     Inverse of treeSampleRate
     */
    double T;

    //----------------------------------------------------------------------
    /**
     Number of subtrees
     */
    size_t subtreeCount;

    //----------------------------------------------------------------------
    /**
      Vector of port resistances at the root
     */
    double *Rp;

    //----------------------------------------------------------------------
    /**
      Vector of parameters.

      Parameters must be defined in the user-specific wdfTree extension as
      paramData objects and added to this vector.
     */
    std::vector<paramData> params;

public:
    //----------------------------------------------------------------------
    /**
     High level function to initialize all subtrees and the root.

     Sets parents in all children in subtrees and creates ports in their nodes.
     */
    void initTree( );

    //----------------------------------------------------------------------
    /**
     Function to set the sample rate.

     Samplerate needs to be set before adaptTree() is called!

     @param fs                  samplerate to use for adaptation of reactive
                                elements.
     */
    void setSamplerate( double fs );

    //----------------------------------------------------------------------
    /**
     Function to get the sample rate.

     @returns                   a double with the tree's sample rate
     */
    double getSamplerate( );

    //----------------------------------------------------------------------
    /**
     High level function to adapt all ports in subtrees.

     Recursively adapts all subtrees towards the root according to sample
     period T. Updates root matrix data afterwards if applicable to take
     new port resistances into account.
     */
    int adaptTree( );

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to update a root's matrix
     element according to the port resistances of the subtrees.

     This function gets a roots' matData* pointer and inits/updates
     it's elements accordingly if applicable for this subtype of root.

     Must be implemented in the user specific wdfTree extension.

     @param rootMatrixData      is a pointer to the matData object of the
                                root
     @param Rp                  is a vector of port resistances of all
                                subtrees of the root.

     @returns                   0 for success, -1 for error
     */
    virtual int setRootMatrData( matData* rootMatrixData,
                                 double *Rp ) = 0;

    //----------------------------------------------------------------------
    /**
     High level function that is called to evaluate the WDF structure for
     one sample.

     It pulls the waves from all connected subtrees, passes them to the
     root and finally pushes the roots' answer down back into the subtrees.
     */
    void cycleWave( );

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to set the circuit's input
     value at the correct source.

     Must be implemented in the user specific wdfTree extension.

     @param signalIn            voltage or current of the input source
     */
    virtual void setInputValue( double signalIn ) = 0;

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to get the circuit's output
     value.

     Must be implemented in the user specific wdfTree extension.

     @returns                   a double type output value
     */
    virtual double getOutputValue( ) = 0;

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to return an identifier string
     for the tree.

     Must be implemented in the user specific wdfTree extension.

     @returns                   a const char* as an identifier string for the
                                tree
     */
    virtual const char* getTreeIdentifier( ) = 0;

    //----------------------------------------------------------------------
    /**
     Function that returns a reference to the tree's param data vector.

     @returns                   a reference to the tree's param data vector
     */
    const std::vector<paramData>& getParams( );

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to set a specific parameter to
     a new value.

     Must be implemented in the user specific wdfTree extension.

     @param paramID             parameter ID as specified by the user in wdfTree
                                application class.
     @param paramValue          value to set the parameter to.
     */
    virtual void setParam( size_t paramID,
                           double paramValue ) = 0;

};

#pragma mark - Roots
//==============================================================================
//                                 R O O T S
//==============================================================================
class wdfRoot {

public:
    //----------------------------------------------------------------------
    /**
     Base class for all root subclasses.
     */
    wdfRoot( );

    //----------------------------------------------------------------------
    /**
     Deconstructor
     */
    virtual ~wdfRoot( );

    //----------------------------------------------------------------------
    /**
     Function that does nothing if not overwritten by a method in a subclass.

     Acts as an interface towards the tree to do anything with the subtree's
     upfacing port resistances in the root. Is called during adaptation of
     the WDF structure.

     @param Rp                  is a vector of port resistances of all
                                subtrees of the root.
     */
    void setPortResistances( double * Rp );

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to process a vector of ascending
     waves in the root and return the result.

     Acts a an interface to the root for wdfTree and it's cycleWave()
     function.

     @param *ascendingWaves     is a pointer to a vector of waves that were
                                collected at the upfacing ports of the subtrees,
                                traveling into the root.
     @param *descendingWaves    is a pointer to a vector to store the resulting
                                waves that leave the root afterprocessing into
                                the subtrees
     */
    virtual void processAscendingWaves( vec* ascendingWaves,
                                        vec* descendingWaves ) = 0;

    //----------------------------------------------------------------------
    /**
     Virtual function that may return a pointer to a root's rootMatrixData
     element.

     Returns a valid pointer or NULL if not applicable for the
     specific root subclass. The valid pointer is used to populate or
     update a root's matrix coefficients by setRootMatrData( ) of the tree.

     @returns                   matData* to the rootMatrixData element
     */
    virtual matData* getRootMatrPtr( );

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to return a String
     describing the type of this root.

     @returns                   a String describing the type of this root
     */
    virtual std::string getType( ) const = 0;

};

//==============================================================================
class wdfRootRtype : public wdfRoot {

private:
    //----------------------------------------------------------------------
    /**
     Pointer to an object which holds all matrices of coefficients of the
     root.
     */
    std::unique_ptr<matData> rootMatrixData;

    //----------------------------------------------------------------------
    /**
     Number of subtrees that are connected to the root.
     */
    int numSubtrees;

public:
    //----------------------------------------------------------------------
    /**
     R-type root implementation.

     @param numSubtrees         specifies the number of subtrees which are
                                connected to the root to correctly allocate
                                memory for rootMatrixData
     */
    wdfRootRtype( int numSubtrees );

    //----------------------------------------------------------------------
    /**
     Deconstructor
     */
    ~wdfRootRtype( );

    //----------------------------------------------------------------------
    /**
     Function to process a vector of ascending waves in the root and return
     the descending result.

     Scatters the incoming waves according to the S-Matrix of rootMatrixData.

     @param *ascendingWaves     is a pointer to a vector of waves that were
                                collected at the upfacing ports of the subtrees,
                                traveling into the root.
     @param *descendingWaves    is a pointer to a vector to store the resulting
                                waves that leave the root afterprocessing into
                                the subtrees
     */
    virtual void processAscendingWaves( vec* ascendingWaves,
                                        vec* descendingWaves );

    //----------------------------------------------------------------------
    /**
     Function that returns a pointer to the rootMatrixData element.

     The pointer is be used to populate or update the S-Matrix of the
     R-type adapter by the setRootMatrData() function of the tree.

     @returns                   matData* to the rootMatrixData element
     */
    virtual matData* getRootMatrPtr( );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this root.

     @returns                   a String describing the type of this root as
                                "Root (R-type)"
     */
    virtual std::string getType( ) const;

};



//==============================================================================
class wdfRootNL : public wdfRoot {

private:
    //----------------------------------------------------------------------
    /**
     Pointer to an object which holds all matrices of coefficients of the
     root.
     */
    std::unique_ptr<matData> rootMatrixData;

    //----------------------------------------------------------------------
    /**
     Number of subtrees which are connected to the root.
     */
    int numSubtrees;

    //----------------------------------------------------------------------
    /**
     Pointer to a NL Solver which solves the implicit scattering loop
     around the non-linearities in the root.
     */
    std::unique_ptr<nlNewtonSolver> NlSolver;

public:
    //----------------------------------------------------------------------
    /**
     Multiple/Multiport Non-Linearities root implementation.

     @param numSubtrees         specifies the number of subtrees which are
                                connected to the root to correctly allocate
                                memory for rootMatrixData
     @param nlList              is a vector defines that map to available
                                NL models. See nlModelClass.h for all models
     @param solverType          sets the solver to use in this root. See
                                nlSolverClass.h for all solvers.
     */
    wdfRootNL( int numSubtrees,
               std::vector<int> nlList,
               int solverType );

    //----------------------------------------------------------------------
    /**
     Deconstructor
     */
    ~wdfRootNL( );

    //----------------------------------------------------------------------
    /**
     Function to process a vector of ascending waves in the root and return
     the descending result.

     Scatters the waves according to the R-type connector towards the
     non-linearities, solves the inner loop with a NLSolver and returns
     the result.

     @param *ascendingWaves     is a pointer to a vector of waves that were
                                collected at the upfacing ports of the subtrees,
                                traveling into the root.
     @param *descendingWaves    is a pointer to a vector to store the resulting
                                waves that leave the root afterprocessing into
                                the subtrees
     */
    virtual void processAscendingWaves( vec* ascendingWaves,
                                        vec* descendingWaves );

    //----------------------------------------------------------------------
    /**
     Function that returns a pointer to the rootMatrixData element.

     The pointer is be used to populate or update the E,-F-,M-,N-Matrices
     of the root loop by the setRootMatrData() function of the tree.

     @returns                   matData* to the rootMatrixData element
     */
    virtual matData* getRootMatrPtr( );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this root.

     @returns                   a String describing the type of this root as
                                "Root (NL-type)"
     */
    virtual std::string getType( ) const;

};



//==============================================================================
class wdfRootSimple : public wdfRoot {

private:
    //----------------------------------------------------------------------
    /**
     Pointer to a single unadapted root element.
     */
    wdfRootNode* rootElement;

public:
    //----------------------------------------------------------------------
    /**
     Single unadapted closed-form element root implementation.

     @param *rootElement         pointer

     */
    wdfRootSimple ( wdfRootNode* rootElement );

    //----------------------------------------------------------------------
    /**
     Sets correct port resistance from the subtrees in RootNode.

     @param Rp               is a vector of port resistances of all
     subtrees of the root.
     */
    void setPortResistances(double * Rp);

    //----------------------------------------------------------------------
    /**
     Function to process a vector (of length 1) of the ascending wave
     towards a single unadapted closed-form root node.

     @param *ascendingWaves     is a pointer to a vector of waves that were
                                collected at the upfacing ports of the subtrees,
                                traveling into the root.
     @param *descendingWaves    is a pointer to a vector to store the resulting
                                waves that leave the root afterprocessing into
                                the subtrees
     */
    virtual void processAscendingWaves( vec* ascendingWaves,
                                        vec* descendingWaves );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this root.

     @returns                   a String describing the type of this root  as
                                "Root (Simple-type)"
     */
    virtual std::string getType( ) const ;

};

# pragma mark - Basic Tree Classes
//==============================================================================
//                                  P O R T
//==============================================================================
/**
 Port class to describe a wave digital filter port.
 */
class wdfPort {

public:
    //----------------------------------------------------------------------
    /**
     Creates a WDF port on connectedNode.

     @param *connectedNode      is a pointer to the node that connects to that
                                port.
     */
    wdfPort( wdfTreeNode* connectedNode );

    //----------------------------------------------------------------------
    /**
     Calculates the port voltage based on wave variables a and b

     @returns                   port voltage in Volt
     */
    double getPortVoltage( );

    //----------------------------------------------------------------------
    /**
     Calculates the port current based on wave variables a and b

     @returns                   port current in Ampere
     */
    double getPortCurrent( );

    //----------------------------------------------------------------------
    /**
     The WDF port resistance in Ohm
     */
    double Rp;

    //----------------------------------------------------------------------
    /**
     The inverse WDF port resistance in Siemens
     */
    double Gp;

    //----------------------------------------------------------------------
    /**
     Incident wave (incoming wave)
     */
    double a;

    //----------------------------------------------------------------------
    /**
     Reflected wave (outgoing wave)
     */
    double b;

    //----------------------------------------------------------------------
    /**
     Pointer to the node that connects to that port.
     */
    wdfTreeNode* connectedNode;

};


//==============================================================================
//                             T R E E   N O D E
//==============================================================================
/**
 Tree Node class to describe a wave digital filter node in a subtree below
 the root
 */
class wdfTreeNode {

public:
    //----------------------------------------------------------------------
    /**
     Base class of a WDF Tree Node.

     This is the base class for all adapted WDF elements in subtrees with more
     than n=2 ports that hang off the root.

     @param *childrenNodes      holds a vector of pointers to all children
                                connected to this element in the tree
     */
    wdfTreeNode( std::vector<wdfTreeNode*> childrenIn );

    /**
     Base class of a WDF Tree Node.

     This version of the constructor is used as the base class for three port
     nodes in the tree (series and parallel adaptors)

     @param *left               holds a pointer to the left child of the node
     @param *right              holds a pointer to the right child of the node
     */
    wdfTreeNode( wdfTreeNode *left,
                 wdfTreeNode *right );

    /**
     Base class of a WDF Tree Node.

     This version of the constructor is used as the base class for all adapted
     WDF leafs.
     */
    wdfTreeNode( );

    //----------------------------------------------------------------------
    /**
     Destructor
     */
    virtual ~wdfTreeNode( ) {}

    //----------------------------------------------------------------------
    /**
     Recursively sets pointers to parent node in children.

     This recursion should be initiated from the base of the tree. It
     sets a pointer to *this parent node in all connected children.
     */
    void setParentInChildren( );

    //----------------------------------------------------------------------
    /**
     Recursively creates WDF ports in the tree.

     This recursion should be initiated from the base of the tree. It
     creates downfacing ports for all children and an upfacing port in
     each child while setting the correct pointers to the nodes that
     these ports are connected to.
     */
    void createPorts( );

    //----------------------------------------------------------------------
    /**
     Recursively adapts all ports in the tree.

     This recursion should be initiated from the base of the tree. It sets
     port resistances of all upfacing ports according to component values
     and/or adaptation rules and copies it onto connected downfacing ports
     in the parent.

     @param T                   sample period as specified by setSamplerate()
     @returns                   a double type up-facing port resistance of that
                                WDF element
     */
    double adaptPorts( double T );

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to return the nodes' upfacing
     port resistance.

     This function is called from adaptPorts(). It returns the upfacing
     port resistance of a tree node to fulfill adaptation according to
     the respective adaptation law of this node.

     @param T                   sample period T = 1/fs as needed to adapt
                                capacitors/inductors
     @returns                   a double type port resistance of that element
                                in Ohms
     */
    virtual double calculateUpRes( double T ) = 0;

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to recursively calculate
     scattering coefficients of adapters.

     This recursion should be initiated from the base of the tree. It
     calculates the scattering coefficients according to the port
     resistances of an element.
     */
    virtual void calculateScatterCoeffs( ) = 0;

    //----------------------------------------------------------------------
    /**
     Recursively pulls the wave from the leafs towards the root.

     This recursion should be initiated from the base of the tree. As all
     tree nodes are adapted, it basically just pulls up correctly
     scattered states of reactive elements and input values from adapted
     sources.

     @returns                   the upward traveling wave of a node
     */
    virtual double pullWaveUp( );

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to return the upfacing wave
     component towards the root.

     This function is called from pullWave() to calculate the wave that
     travels towards the base according to the element type.

     @returns                   the upward traveling wave of a node
     */
    virtual double calculateUpB( ) = 0;

    //----------------------------------------------------------------------
    /**
     Recursively pushes the wave from the root towards the leafs.

     This recursion should be initiated from the base of the tree. It
     pushes the respective wave component of a branch from the root to
     the leafs by scattering them correctly along the way.

     @param  descendingWave     descending wave component that shall be
                                scattered to the downfacing ports of the node or
                                processed in a leaf accordingly
     */
    virtual void pushWaveDown( double descendingWave );

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to set the wave components in
     the downfacing ports.

     This function processes the input wave correctly for the downfacing
     ports and sets them in the port objects.

     @param  descendingWave     incoming wave component on the upfacing port
     */
    virtual void calculateDownB( double descendingWave ) = 0;

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to return a String describing
     the type of this node.

     @returns                   a String describing the type of this root
     */
    virtual std::string getType( ) const = 0;

    //----------------------------------------------------------------------
    /**
     Pointer to the upfacing port object of this node.
     */
    std::unique_ptr<wdfPort> upPort;

    //----------------------------------------------------------------------
    /**
     Pointer to the connected parent node of this node.
     */
    wdfTreeNode* parentNode;

protected:
    //----------------------------------------------------------------------
    /**
     Vector of pointers to downfacing port objects of this tree.
     */
    std::vector<wdfPort*> downPorts;

    //----------------------------------------------------------------------
    /**
     Vector of pointers to connected children nodes of this node.
     */
    std::vector<wdfTreeNode*> childrenNodes;

};

#pragma mark - Terminated Adapters
//==============================================================================
//              T E R M I N A T E D   A D A P T E R S   (S, P, R)
//==============================================================================
class wdfTerminatedAdapter: public wdfTreeNode {

public:
    //----------------------------------------------------------------------
    /**
     Base class for terminated 2 port adapters in WDF subtrees/branches.

     This includes series and parallel adapters with one terminated
     upfacing port and 2 downfacing ports.

     @param left                pointer to the adapter's left children
     @param right               pointer to the adapter's right children
     */
    wdfTerminatedAdapter( wdfTreeNode *left,
                          wdfTreeNode *right );
    /**
     Base class for terminated n port adapters in WDF subtrees/branches.

     This includes r-type adapters with one terminated upfacing port and n
     downfacing ports.

     @param *childrenNodes      holds a vector of pointers to the n adapter's
                                children
     */
    wdfTerminatedAdapter( std::vector<wdfTreeNode*> childrenIn );


};

//==============================================================================
class wdfTerminatedRtype: public wdfTerminatedAdapter {

public:
    //----------------------------------------------------------------------
    /**
     Adapter class for terminated n-port R-type nodes.

     The adapter owns one terminated upfacing port and n downfacing ports

     @param *childrenNodes      holds a vector of pointers to the n adapters'
                                children
     */
    wdfTerminatedRtype( std::vector<wdfTreeNode*> childrenIn );

    //----------------------------------------------------------------------
    /**
     Destructor

     */
    ~wdfTerminatedRtype();

    //----------------------------------------------------------------------
    /**
     Returns the r-type adaptors upfacing port resistance.

     This function is called from adaptPorts(). It returns the upfacing
     port resistance of the node to fulfill termination according to
     the respective adaptation law of this node.

     Must be implemented in the user-defined subclass of wdfTerminatedRtype in
     the WDF application tree.

     @param T                   sample period T = 1/fs as needed to adapt
                                capacitors/inductors
     @returns                   a double type port resistance of that element
                                in Ohms
     */
    virtual double calculateUpRes( double T ) = 0;

    //----------------------------------------------------------------------
    /**
     Recursively calculates scattering coefficients of adapters.

     This recursion should be initiated from the base of the tree. It
     calculates the scattering coefficients according to the port resistances
     of the adaptor.

     Must be implemented in the user-defined subclass of wdfTerminatedRtype in
     the WDF application tree. Make sure to continue the recursion by calling
     downPort->connectedNode->calculateScatterCoeffs( ) on all down ports.
     */
    virtual void calculateScatterCoeffs( ) = 0;

    //----------------------------------------------------------------------
    /**
     Returns the upfacing wave component towards the root.

     This function is called from pullWave() to calculate the wave that
     travels towards the base.

     It collects the wave components from the downfacing ports and weights them
     with the coefficients from the scattering matrix S.

     @returns                   the upward traveling wave of a node
     */
    virtual double calculateUpB( );

    //----------------------------------------------------------------------
    /**
     Sets the wave components in the downfacing ports.

     This function weights the input wave and the incident waves on the
     downports with the coefficients in the scattering matrix S and sets the
     reflected waves in the downfacing port objects.

     @param  descendingWave     incoming wave component on the upfacing port
     */
    virtual void calculateDownB( double descendingWave );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this adaptor.

     @returns                   a String describing the type of this adaptor as
                                "R-type Adapter (TOP adapted)"
     */
    virtual std::string getType( ) const;

protected:
    //----------------------------------------------------------------------
    /**
     Scattering matrix of the R-type adapter

     Size:  (childNodeCount+1) x (childNodeCount+1)
            ("+1" for upfacing port)
     */
    std::unique_ptr<mat> S;

};

//==============================================================================
class wdfTerminatedSeries : public wdfTerminatedAdapter {

private:
    //----------------------------------------------------------------------
    /**
     Upfacing scattering coefficient of the series adapter
     */
    double yu;
    /**
     Left scattering coefficient of the series adapter
     */
    double yl;
    /**
     Right scattering coefficient of the series adapter
     */
    double yr;

public:
    //----------------------------------------------------------------------
    /**
     Adapter class for terminated three port series nodes.

     The adapter owns one terminated upfacing port and two downfacing ports

     @param *left               holds a pointer to the left child of the node
     @param *right              holds a pointer to the right child of the node
     */
    wdfTerminatedSeries( wdfTreeNode *left,
                         wdfTreeNode *right );

    //----------------------------------------------------------------------
    /**
     Returns the series adaptors upfacing port resistance.

     This function is called from adaptPorts(). It returns the upfacing
     port resistance of the node to fulfill termination according to
     the series adaptation law Rup = Rleft + Rright.

     @param T                   sample period T = 1/fs as needed to adapt
                                capacitors/inductors
     @returns                   a double type port resistance of that element
                                in Ohms
     */
    virtual double calculateUpRes( double T );

    //----------------------------------------------------------------------
    /**
     Recursively calculates scattering coefficients of adapters.

     This recursion should be initiated from the base of the tree. It
     calculates the scattering coefficients according to the port resistances
     of the adaptor and the series adaptor scattering law.
     */
    virtual void calculateScatterCoeffs( );

    //----------------------------------------------------------------------
    /**
     Returns the upfacing wave component towards the root.

     This function is called from pullWave() to calculate the wave that
     travels towards the base.

     It collects the wave components from the downfacing ports and returns the
     reflected wave.

     @returns                   the upward traveling wave of the node
     */
    virtual double calculateUpB( );

    //----------------------------------------------------------------------
    /**
     Sets the wave components in the downfacing ports.

     This function weights the input wave and the incident waves on the
     downports with the coefficients yu, yl and yr and sets the reflected waves
     in the downfacing port objects.

     @param  descendingWave     incoming wave component on the upfacing port
     */
    virtual void calculateDownB( double descendingWave );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this adaptor.

     @returns                   a String describing the type of this adaptor as
                                "Series Adapter (TOP adapted)"
     */
    virtual std::string getType( ) const;

};

//==============================================================================
class wdfTerminatedParallel : public wdfTerminatedAdapter {

private:
    //----------------------------------------------------------------------
    /**
     Upfacing scattering coefficient of the parallel adapter
     */
    double du;
    /**
     Left scattering coefficient of the parallel adapter
     */
    double dl;
    /**
     Right scattering coefficient of the parallel adapter
     */
    double dr;

public:
    //----------------------------------------------------------------------
    /**
     Adapter class for terminated three port parallel nodes.

     The adapter owns one adapted upfacing port and two downfacing ports

     @param *left               holds a pointer to the left child of the node
     @param *right              holds a pointer to the right child of the node
     */
    wdfTerminatedParallel( wdfTreeNode *left,
                           wdfTreeNode *right );

    //----------------------------------------------------------------------
    /**
     Returns the parallel adaptors upfacing port resistance.

     This function is called from adaptPorts(). It returns the upfacing
     port resistance of the node to fulfill termination according to
     the series adaptation law Gup = Gleft + Gright.

     @param T                   sample period T = 1/fs as needed to adapt
                                capacitors/inductors
     @returns                   a double type port resistance of that element
                                in Ohms
     */
    virtual double calculateUpRes( double T );

    //----------------------------------------------------------------------
    /**
     Recursively calculates scattering coefficients of adapters.

     This recursion should be initiated from the base of the tree. It
     calculates the scattering coefficients according to the port resistances
     of the adaptor and the parallel adaptor scattering law.
     */
    virtual void calculateScatterCoeffs( );

    //----------------------------------------------------------------------
    /**
     Returns the upfacing wave component towards the root.

     This function is called from pullWave() to calculate the wave that
     travels towards the base.

     It collects the wave components from the downfacing ports and returns the
     reflected wave.

     @returns                   the upward traveling wave of the node
     */
    virtual double calculateUpB();

    //----------------------------------------------------------------------
    /**
     Sets the wave components in the downfacing ports.

     This function weights the input wave and the incident waves on the
     downports with the coefficients du, dl and dr and sets the reflected waves
     in the downfacing port objects.

     @param  descendingWave     incoming wave component on the upfacing port
     */
    virtual void calculateDownB( double descendingWave );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this adaptor.

     @returns                   a String describing the type of this adaptor as
                                "Parallel Adapter (TOP adapted)"
     */
    virtual std::string getType( ) const;

};

class wdfInverter : public wdfTerminatedAdapter {

public:
    //----------------------------------------------------------------------
    /**
     Adapter class for terminated wave inverter nodes.

     The adapter owns one terminated upfacing port and one downfacing port

     @param *child              holds a pointer to the child of the inverter
     */
    wdfInverter( wdfTreeNode *child );

    //----------------------------------------------------------------------
    /**
     Returns the inverters upfacing port resistance.

     This function is called from adaptPorts(). It returns the upfacing
     port resistance of the node to fulfill termination according to
     the inverters adaptation law Rup = Rchild.

     @param T                   sample period T = 1/fs as needed to adapt
                                capacitors/inductors
     @returns                   a double type port resistance of that element
                                in Ohms
     */
    virtual double calculateUpRes( double T );

    //----------------------------------------------------------------------
    /**
     Recursively calculates scattering coefficients of adapters.

     This recursion should be initiated from the base of the tree. It
     does nothing for the inverter except continuing the recursion as the
     coefficients are hard-coded in the calculateUpB() and calculateDownB()
     functions.
     */
    virtual void calculateScatterCoeffs( );

    //----------------------------------------------------------------------
    /**
     Returns the upfacing wave component towards the root.

     This function is called from pullWave() to calculate the wave that
     travels towards the base.

     It collects the wave from the downfacing port and returns it sign-flipped
     as the reflected wave.

     @returns                   the upward traveling wave of the node
     */
    virtual double calculateUpB( );

    //----------------------------------------------------------------------
    /**
     Sets the wave component in the downfacing port.

     This function sets the sign-flipped incident wave on the downport.

     @param  descendingWave     incoming wave component on the upfacing port
     */
    virtual void calculateDownB( double descendingWave );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this adaptor.

     @returns                   a String describing the type of this adaptor as
                                "Inverter (TOP adapted)"
     */
    virtual std::string getType( ) const;

};

#pragma mark - Terminated Leafs
//==============================================================================
//                    T E R M I N A T E D   L E A F S   (Q)
//==============================================================================
class wdfTerminatedLeaf: public wdfTreeNode {

public:
    //----------------------------------------------------------------------
    /**
     Base class for adapted leafs in WDF subtrees/branches.

     This includes all linear one-port elements with one terminated upfacing
     port and non-implicit behaviour.
     */
    wdfTerminatedLeaf();
    
    //----------------------------------------------------------------------
    /**
     This variant of the base classes' calculateScatterCoeffs() function
     is an empty function to break the recursion that only applies
     to adapters and stops here at the leafs.
     */
    virtual void calculateScatterCoeffs( );

};

//==============================================================================
class wdfTerminatedCap : public wdfTerminatedLeaf {

public:
    //----------------------------------------------------------------------
    /**
     Adapted capacitor model class.

     Creates a capacitor with capacitance C.

     @param C                  physical capacitance of the component in Farads
     @param T                  sample period T = 1/fs in seconds
     */
    wdfTerminatedCap( double C,
                      double T );

    //----------------------------------------------------------------------
    /**
     Returns the capacitors upfacing port resistance.

     This function is called from adaptPorts(). It returns the upfacing
     port resistance of the node to fulfill termination according to
     the capacitors adaptation law Rup = T / ( 2.0 * C ).

     @param T                   sample period T = 1/fs in seconds as needed to
                                adapt the capacitor
     @returns                   a double type port resistance of that element
                                in Ohms
     */
    virtual double calculateUpRes( double T );

    //----------------------------------------------------------------------
    /**
     Returns the upfacing wave component towards the root.

     This function is called from pullWave() to calculate the wave that
     travels towards the base.

     It returns the incident wave delayed by one sample as the reflected wave.

     @returns                   the upward traveling wave of the node
     */
    virtual double calculateUpB( );

    //----------------------------------------------------------------------
    /**
     Sets the wave component in the downfacing port.

     This function feeds a one sample delay with the incident wave to return
     it as the next reflected wave.

     @param  descendingWave     incoming wave component on the upfacing port
     */
    virtual void calculateDownB( double descendingWave );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this leaf.

     @returns                   a String describing the type of this leaf as
                                "C (adapted)"
     */
    virtual std::string getType( ) const;

    //----------------------------------------------------------------------
    /**
     Capacitance in Farad
     */
    double C;
    /**
     Sample period in seconds
     */
    double T;
    /**
     One sample delay element
     */
    double prevA;

};

//==============================================================================
class wdfTerminatedInd : public wdfTerminatedLeaf {

public:
    //----------------------------------------------------------------------
    /**
     Adapted inductor model class.

     Creates an inductor with inductance L.

     @param L                  physical inductance of the component in Henry
     @param T                  sample period T = 1/fs in seconds
     */
    wdfTerminatedInd( double L,
                      double T );

    //----------------------------------------------------------------------
    /**
     Returns the inductors upfacing port resistance.

     This function is called from adaptPorts(). It returns the upfacing
     port resistance of the node to fulfill termination according to
     the inverters adaptation law Rup = R = ( 2.0 * L ) / T.

     @param T                   sample period T = 1/fs in seconds as needed to
                                adapt the capacitor
     @returns                   a double type port resistance of that element
                                in Ohms
     */
    virtual double calculateUpRes( double T );

    //----------------------------------------------------------------------
    /**
     Returns the upfacing wave component towards the root.

     This function is called from pullWave() to calculate the wave that
     travels towards the base.

     It returns the sign-flipped incident wave delayed by one sample as the
     reflected wave.

     @returns                   the upward traveling wave of the node
     */
    virtual double calculateUpB( );

    //----------------------------------------------------------------------
    /**
     Sets the wave component in the downfacing port.

     This function feeds a one sample delay with the incident wave to return
     it sign-flipped as the next reflected wave.

     @param  descendingWave     incoming wave component on the upfacing port
     */
    virtual void calculateDownB( double descendingWave );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this leaf.

     @returns                   a String describing the type of this leaf as
                                "L (adapted)"
     */
    virtual std::string getType( ) const;

    //----------------------------------------------------------------------
    /**
     Inductance in Henry
     */
    double L;
    /**
     Sample period in seconds
     */
    double T;
    /**
     One sample delay element
     */
    double prevA;

};

//==============================================================================
class wdfTerminatedRes : public wdfTerminatedLeaf {

public:
    //----------------------------------------------------------------------
    /**
     Adapted resistor model class.

     Creates a resistor with resistance R.

     @param R                   physical resistance of the component in Ohm
     */
    wdfTerminatedRes( double R );

    //----------------------------------------------------------------------
    /**
     Returns the resistors upfacing port resistance.

     This function is called from adaptPorts(). It returns the upfacing
     port resistance of the node to fulfill termination according to
     the resistors adaptation law Rup = R.

     @param T                   sample period T = 1/fs in seconds as needed to
                                adapt the capacitor
     @returns                   a double type port resistance of that element
                                in Ohms
     */
    virtual double calculateUpRes( double T );

    //----------------------------------------------------------------------
    /**
     Returns the upfacing wave component towards the root.

     This function is called from pullWave() to calculate the wave that
     travels towards the base.

     It always returns 0.0 for the adapted resistor.

     @returns                   the upward traveling wave of the node
     */
    virtual double calculateUpB( );

    //----------------------------------------------------------------------
    /**
     Sets the wave component in the downfacing port.

     This function does nothing for the adapted resistor.

     @param  descendingWave     incoming wave component on the upfacing port
     */
    virtual void calculateDownB( double descendingWave );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this leaf.

     @returns                   a String describing the type of this leaf as
                                "R (adapted)"
     */
    virtual std::string getType( ) const;

    //----------------------------------------------------------------------
    /**
     Resistance in Ohm
     */
    double R;

};

//==============================================================================
class wdfTerminatedResVSource : public wdfTerminatedLeaf {

public:
    //----------------------------------------------------------------------
    /**
     Adapted resistive voltage source model class.

     Creates a voltage source with series resistance Rser.

     @param Vs                  initial source voltage in Volts
     @param Rser                physical resistance of the series resistor in
                                Ohm
     */
    wdfTerminatedResVSource( double Vs,
                             double RSer );

    //----------------------------------------------------------------------
    /**
     Returns the voltage source's upfacing port resistance.

     This function is called from adaptPorts(). It returns the upfacing
     port resistance of the node to fulfill termination according to
     the adaptation law Rup = Rser.

     @param T                   sample period T = 1/fs in seconds as needed to
                                adapt the capacitor
     @returns                   a double type port resistance of that element
                                in Ohms
     */
    virtual double calculateUpRes( double T );

    //----------------------------------------------------------------------
    /**
     Returns the upfacing wave component towards the root.

     This function is called from pullWave() to calculate the wave that
     travels towards the base.

     It returns the source voltage value as the reflected wave.

     @returns                   the upward traveling wave of the node
     */
    virtual double calculateUpB( );

    //----------------------------------------------------------------------
    /**
     Sets the wave component in the downfacing port.

     This function does nothing for the adapted voltage source.

     @param  descendingWave     incoming wave component on the upfacing port
     */
    virtual void calculateDownB( double descendingWave );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this leaf.

     @returns                   a String describing the type of this leaf as
                                "Vs (incl. Rp = RSer -> adapted)"
     */
    virtual std::string getType( ) const;

    //----------------------------------------------------------------------
    /**
     Source voltage in volts. Change this public variable during operation if
     necessary according to input samples.
     */
    double Vs;
    /**
     Series resistance in Ohms.
     */
    double RSer;

};

#pragma mark - Root Nodes
//==============================================================================
//                             R O O T   N O D E
//==============================================================================
class wdfRootNode {

protected:
    //----------------------------------------------------------------------
    /**
     The WDF port resistance in Ohm.
     */
    double Rp;
    /**
     The number of ports on a root node element.
     */
    size_t numPorts;

public:
    //----------------------------------------------------------------------
    /**
     Base class of a WDF Root Node.

     This is the base class for all unadapted WDF elements that can form the
     closed-form (non-implicit) root of a tree.

     @param numPorts            number of ports on this root node
     */
    wdfRootNode( int numPorts );

    //----------------------------------------------------------------------
    /**
     Deconstructor
     */
    virtual ~wdfRootNode( );

    //----------------------------------------------------------------------
    /**
     Sets the nodes port resistance according to the port it is connected
     to.

     As these nodes sit at the root, their port resistance is dependant on the
     upfacing port of the subtree they are connected to.

     @param Rp                  port resistance in Ohm
     */
    virtual void setPortResistance( double Rp );

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to calculate the reflection of
     the incoming, ascending wave.

     This is typically an unadapted expression which involves the component
     value of the node, the port resistance Rp, past states and the ascending
     wave itself.

     @param *ascendingWaves     Pointer to a vector of ascending waves
     @param *descendingWaves    Pointer to a vector to store descending waves
     @param *portIndex          Pointer to a counter with the port index to use
                                in the vectors of ascending and descending
                                waves. Needs to be incremented by numPorts in
                                the node's implementation.
     */
    virtual void calculateDownB( vec* ascendingWaves,
                                 vec* descendingWaves,
                                 size_t* portIndex ) = 0;

    //----------------------------------------------------------------------
    /**
     Return the number of ports of that root node.

     @returns                   the value of the numPorts member.
     */
    int getNumPorts( );

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to return a String describing
     the type of this root node.

     @returns                   a String describing the type of this root node
     */
    virtual std::string getType( ) const = 0;

};


//==============================================================================
//                  U N T E R M I N A T E D   E L E M E N T S
//==============================================================================
class wdfUnterminatedSwitch : public wdfRootNode {

private:
    //----------------------------------------------------------------------
    /**
     Position of the switch (open/closed)
    */
    int position;

public:
    //----------------------------------------------------------------------
    /**
     Unterminated switch (open/short) model class.

     @param position            can be either 1 for a closed switch or 0
                                for an open one.
     */
    wdfUnterminatedSwitch( int position );

    //----------------------------------------------------------------------
    /**
     Calculates the reflection of the incoming, ascending wave.

     This is either a complete reflection or it's inverse, depending on
     the switch position.

     @param *ascendingWaves     Pointer to a vector of ascending waves
     @param *descendingWaves    Pointer to a vector to store descending waves
     @param *portIndex          Pointer to a counter with the port index to use

                                in the vectors of ascending and descending
                                waves. Needs to be incremented by numPorts in
                                the node's implementation.
     */
    virtual void calculateDownB( vec* ascendingWaves,
                                 vec* descendingWaves,
                                 size_t* portIndex );

    //----------------------------------------------------------------------
    /**
     Sets the switch position (0/1)

     @param position            can be either 1 for a closed switch or 0
                                for an open one.
     */
    void setSwitch( int position );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this root node.

     @returns                   a String describing the type of this leaf as
                                "Unadapted controllable switch"
     */
    virtual std::string getType( ) const;

};

#pragma mark - Unterminated Leafs
//==============================================================================
class wdfUnterminatedCap : public wdfRootNode {

protected:
    //----------------------------------------------------------------------
    /**
     Sample period in seconds
    */
    double T;
    /**
     Place to store the previous incident wave component.
    */
    double prevA;
    /**
     Place to store the previous reflected wave component.
    */
    double prevB;
    /**
     Reflection coefficient, depending on the inherited port resistance,
     component value and sample rate.

     This coefficient is pre-calculated in the setPortResistance function.
    */
    double reflectionCoeff;

public:
    //----------------------------------------------------------------------
    /**
     Unadapted capacitor model class.

     Creates an unadapted capacitor with capacitance C.

     @param C                  physical capacitance of the component in Farads
     @param T                  sample period T = 1/fs in seconds
    */
    wdfUnterminatedCap( double C,
                        double T );

    //----------------------------------------------------------------------
    /**

     Calculates the reflection of the incoming, ascending wave.

     This is composed of the recent ascending wave component and former
     components on this port.

     @param *ascendingWaves     Pointer to a vector of ascending waves
     @param *descendingWaves    Pointer to a vector to store descending waves
     @param *portIndex          Pointer to a counter with the port index to use
                                in the vectors of ascending and descending
                                waves. Needs to be incremented by numPorts in
                                the node's implementation.
    */
    virtual void calculateDownB( vec* ascendingWaves,
                                 vec* descendingWaves,
                                 size_t* portIndex );

    //----------------------------------------------------------------------
    /**

     Sets the nodes port resistance according to the port it is connected
     to.

     TODO: In the case of the unadapted capacitor, it also pre-calculates the
     reflection coefficient reflectionCoeff, which depends on the
     port resistance too.

     @param Rp                  port resistance in Ohm
     */
    void setPortResistance( double Rp );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this root node.

     @returns                   a String describing the type of this leaf as
                                "C (unadapted)"
     */
    virtual std::string getType( ) const;

    //----------------------------------------------------------------------
    /**
     Capacitance in Farad
    */
    double C;

};

//==============================================================================
class wdfUnterminatedInd : public wdfRootNode {

protected:
    //----------------------------------------------------------------------
    /**
     Sample period in seconds
    */
    double T;
    /**
     Place to store the previous incident wave component.
    */
    double prevA;
    /**
     Place to store the previous reflected wave component.
    */
    double prevB;
    /**
     Reflection coefficient, depending on the inherited port resistance,
     component value and sample rate.

     This coefficient is pre-calculated in the setPortResistance function.
    */
    double reflectionCoeff;

public:
    //----------------------------------------------------------------------
    /**
     Unadapted inductor model class.

     Creates an unadapted inductor with capacitance L.

     @param L                  physical inductance of the component in Henry
     @param T                  sample period T = 1/fs in seconds
    */
    wdfUnterminatedInd( double L,
                        double T );

    //----------------------------------------------------------------------
    /**
     Calculates the reflection of the incoming, ascending wave.

     This is composed of the recent ascending wave component and former
     components on this port.

     @param *ascendingWaves     Pointer to a vector of ascending waves
     @param *descendingWaves    Pointer to a vector to store descending waves
     @param *portIndex          Pointer to a counter with the port index to use
                                in the vectors of ascending and descending
                                waves. Needs to be incremented by numPorts in
                                the node's implementation.
    */
    virtual void calculateDownB( vec* ascendingWaves,
                                 vec* descendingWaves,
                                 size_t* portIndex );

    //----------------------------------------------------------------------
    /**
     Sets the nodes port resistance according to the port it is connected
     to.

     TODO: In the case of the unadapted inductor, it also pre-calculates the
     reflection coefficient reflectionCoeff, which depends on the
     port resistance too.

     @param Rp                  port resistance in Ohm
     */
    void setPortResistance( double Rp );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this root node.

     @returns                   a String describing the type of this leaf as
                                "L (unadapted)"
     */
    virtual std::string getType( ) const;

    //----------------------------------------------------------------------
    /**
     Inductance in Henry
    */
    double L;

};

//==============================================================================
class wdfUnterminatedRes: public wdfRootNode {

protected:
    //----------------------------------------------------------------------
    /**
     Reflection coefficient, depending on the inherited port resistance and
     component value.

     This coefficient is pre-calculated in the setPortResistance function.
    */
    double reflectionCoeff;

public:
    //----------------------------------------------------------------------
    /**
     Unadapted resistor model class.

     Creates an unadapted resistor with resistance R.

     @param R                  physical inductance of the component in Henry
    */
    wdfUnterminatedRes( double R );

    //----------------------------------------------------------------------
    /**
     Calculates the reflection of the incoming, ascending wave.

     @param *ascendingWaves     Pointer to a vector of ascending waves
     @param *descendingWaves    Pointer to a vector to store descending waves
     @param *portIndex          Pointer to a counter with the port index to use
                                in the vectors of ascending and descending
                                waves. Needs to be incremented by numPorts in
                                the node's implementation.
    */
    virtual void calculateDownB( vec* ascendingWaves,
                                 vec* descendingWaves,
                                 size_t* portIndex );

    //----------------------------------------------------------------------
    /**
     Sets the nodes port resistance according to the port it is connected
     to.

     TODO: In the case of the unadapted resistor, it also pre-calculates the
     reflection coefficient reflectionCoeff, which depends on the
     port resistance too.

     @param Rp                  port resistance in Ohm
     */
    void setPortResistance( double Rp );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this root node.

     @returns                   a String describing the type of this leaf as
                                "R (unadapted)"
     */
    virtual std::string getType( ) const;

    //----------------------------------------------------------------------
    /**
     Resistance in Ohms
    */
    double R;

};

//==============================================================================
class wdfIdealVSource: public wdfRootNode {

public:
    //----------------------------------------------------------------------
    /**
     Unadapted, ideal voltage source class.

     Creates an ideal voltage source with initial voltage Vs

     @param Vs                  initial source voltage in Volts
    */
    wdfIdealVSource( double Vs );

    //----------------------------------------------------------------------
    /**
     Calculates the reflection of the incoming, ascending wave.

     @param *ascendingWaves     Pointer to a vector of ascending waves
     @param *descendingWaves    Pointer to a vector to store descending waves
     @param *portIndex          Pointer to a counter with the port index to use
                                in the vectors of ascending and descending
                                waves. Needs to be incremented by numPorts in
                                the node's implementation.
    */
    virtual void calculateDownB( vec* ascendingWaves,
                                 vec* descendingWaves,
                                 size_t* portIndex );

    //----------------------------------------------------------------------
    /**
     Sets the nodes port resistance according to the port it is connected
     to.

     @param Rp                  port resistance in Ohm
     */
    void setPortResistance( double Rp );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this root node.

     @returns                   a String describing the type of this leaf as
                                "Vs (ideal -> unadapted)"
     */
    virtual std::string getType( ) const;

    //----------------------------------------------------------------------
    /**
     Source voltage in Volts
    */
    double Vs;

};

//==============================================================================
class wdfIdealCSource: public wdfRootNode {

public:
    //----------------------------------------------------------------------
    /**
     Unadapted, ideal current source class.

     Creates an ideal current source with initial current Is

     @param Vs                  initial source voltage in Volts
    */
    wdfIdealCSource(double I_in);

    //----------------------------------------------------------------------
    /**
     Calculates the reflection of the incoming, ascending wave.

     @param *ascendingWaves     Pointer to a vector of ascending waves
     @param *descendingWaves    Pointer to a vector to store descending waves
     @param *portIndex          Pointer to a counter with the port index to use
                                in the vectors of ascending and descending
                                waves. Needs to be incremented by numPorts in
                                the node's implementation.
    */
    virtual void calculateDownB( vec* ascendingWaves,
                                 vec* descendingWaves,
                                 size_t* portIndex );

    //----------------------------------------------------------------------
    /**
     Sets the nodes port resistance according to the port it is connected
     to.

     @param Rp                  port resistance in Ohm
     */
    void setPortResistance( double Rp );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this root node.

     @returns                   a String describing the type of this leaf as
                                "Is (ideal -> unadapted)"
     */
    virtual std::string getType( ) const;

    //----------------------------------------------------------------------
    /**
     Source current in Ampere
    */
    double Is;

};

#endif  // RTWDF_H_INCLUDED
