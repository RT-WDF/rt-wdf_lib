/*
  ==============================================================================

    RenderThread.h
    Created: 2 May 2016 12:45:53pm
    Author:  Maximilian Rest

  ==============================================================================
*/

#ifndef RENDERTHREAD_H_INCLUDED
#define RENDERTHREAD_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../../../Libs/rt-wdf/rt-wdf.h"
#include "../../../../Libs/r8brain-free-src/CDSPResampler.h"


class RenderThread;
class RenderParams;


class RenderThread : public ThreadWithProgressWindow {
    
public:
    RenderThread( );
    void run( );
    void setRenderParamsPtr( RenderParams* myRenderParams );

    
private:
    RenderParams* myRenderParams;

};


class RenderParams {
    
public:
    wdfTree* myWdfTree;
    AudioTransportSource* transportSource;
    r8b::CDSPResampler24* upSmplr24;
    r8b::CDSPResampler24* downSmplr24;
    double* upBuf;
    double* downBuf;
    
    int blockSize;
    double outputSampleRate;
    double inputSampleRate;
    double treeSampleRate;
    double outputBitDepth;
    
    double renderTime;
    size_t badBlocks;
    size_t totalBlocks;
};


#endif  // RENDERTHREAD_H_INCLUDED
