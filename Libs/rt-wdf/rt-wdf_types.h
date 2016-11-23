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

    rt-wdf_types.h
    Created: 15 Dec 2015 3:54:27pm
    Author:  mrest

  ==============================================================================
*/

#ifndef RTWDF_TYPES_H_INCLUDED
#define RTWDF_TYPES_H_INCLUDED


#include <armadillo>
using namespace arma;


//==============================================================================
/** A struct that holds matrices for R-type and NL root nodes.

    wdfRootRtype only uses the S matrix of this struct.
    wdfRootNL only uses the E,F,M,N matrices of this struct.

    @see wdfRootRtype, wdfRootNL
*/
typedef struct matData{

    /** S-Matrix as used in wdfRootRtype.
        Size: (numBrPorts) x (numBrPorts)
    */
    mat Smat;

    /** E-Matrix as used in wdfRootNL.
        Size: (numNlPorts) x (numBrPorts)
    */
    mat Emat;

    /** F-Matrix as used in wdfRootNL.
        Size: (numNlPorts) x (numNlPorts)
    */
    mat Fmat;

    /** M-Matrix as used in wdfRootNL.
        Size: (numBrPorts) x (numBrPorts)
    */
    mat Mmat;

    /** N-Matrix as used in wdfRootNL.
        Size: (numBrPorts) x (numNlPorts)
    */
    mat Nmat;

    /** T-Matrix as used in wdfRootLinear and wdfRootMixed.
     Size: (numBrPorts) x (numNlPorts)
     */
    mat Tmat;

} matData;

typedef enum paramType {
    boolParam,
    doubleParam
} paramType;

typedef struct paramData {
    std::string name;
    size_t ID;
    paramType type;
    double value;
    std::string units;
    double lowLim;
    double highLim;
} paramData;

#endif  // RTWDF_TYPES_H_INCLUDED
