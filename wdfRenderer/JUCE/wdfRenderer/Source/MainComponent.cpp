/*
 ==============================================================================

 This file was auto-generated!

 ==============================================================================
 */

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "RenderThread.h"

using namespace juce;

#include "../../../../Libs/r8brain-free-src/CDSPResampler.h"

// Circuits
// ## When changing circuits, remember to init the right tree around line 85!  ##
//#include "../../../../Examples/wdfCCTAx1Tree.hpp"
//#include "../../../../Examples/wdfCCTAx4Tree.hpp"
//#include "../../../../Examples/wdfJTM45Tree.hpp"
//#include "../../../../Examples/wdfSwitchTree.hpp"
#include "../../../../Examples/wdfTonestackTree.hpp"


//==============================================================================
/*
 This component lives inside our window, and this is where you should put all
 your controls and content.
 */
class MainContentComponent   :  public Component,
                                public AudioIODeviceCallback,
                                public ChangeListener,
                                public Button::Listener,
                                public Slider::Listener,
                                public ComboBox::Listener
{
public:
    //==============================================================================
    MainContentComponent() {

        myLookAndFeel.setUsingNativeAlertWindows(true);
        LookAndFeel::setDefaultLookAndFeel(&myLookAndFeel);
        getLookAndFeel().setDefaultSansSerifTypefaceName("Arial");

        startupTime = Time::getCurrentTime().toMilliseconds();

        cout.precision(15);
        cout.setf(ios::fixed, ios::floatfield);
        cout.width(20);
        cout.fill(' ');

        addAndMakeVisible(groupParams);
        addAndMakeVisible(groupLogger);

        addAndMakeVisible (Logger = new TextEditor ("new text editor"));
        Logger->setMultiLine (true);
        Logger->setReturnKeyStartsNewLine (false);
        Logger->setReadOnly (true);
        Logger->setScrollbarsShown (true);
        Logger->setCaretVisible (true);
        Logger->setPopupMenuEnabled (true);
        Logger->setText (String::empty);

        addAndMakeVisible (&openButton);
        openButton.setButtonText ("Open File");
        openButton.addListener (this);

        addAndMakeVisible (&bitDepthSelector);
        bitDepthSelector.addItem("16 bits",1);
        bitDepthSelector.setSelectedId(1);
        myRenderParams.outputBitDepth = 16;
        bitDepthSelector.addItem("24 bits",2);
        bitDepthSelector.addItem("32 bits",3);
        bitDepthSelector.addListener(this);


        addAndMakeVisible (&renderButton);
        renderButton.setButtonText ("Render");
        renderButton.addListener (this);
        renderButton.setEnabled (false);

        
        // ## Change to corresponding tree here ##
        
        //myWdfTree = new wdfCCTAx1Tree();
        //myWdfTree = new wdfCCTAx4Tree();
        //myWdfTree = new wdfJTM45Tree();
        //myWdfTree = new wdfSwitchTree();
        myWdfTree = new wdfTonestackTree();
        myWdfTree->initTree();

        writeLogLine("Created WDF tree");
        groupParams.setText(myWdfTree->getTreeIdentifier());
        writeLogLine(String("wdfTree description: ") + String(myWdfTree->getTreeIdentifier()));

        createParamControls(myWdfTree->getParams());

        setSize (500, 660);
        writeLogLine("RT-WDF wav-file Renderer. Initializing..");

        formatManager.registerBasicFormats();
        transportSource.addChangeListener (this);

        writeLogLine("Ready.");

    }

    ~MainContentComponent() {

        for (Component* comp : paramComponents) {
            delete comp;
        }
        for (Component* comp : paramLabels) {
            delete comp;
        }
        delete myWdfTree;
    }

    void writeLogLine (String logLine) {
        String nowTime = String::formatted(("%08.3f"), (float)((int64)(Time::getCurrentTime().toMilliseconds()) - startupTime) / 1000.0f);
        Logger->insertTextAtCaret("\n"+nowTime+": "+logLine);
    }

    //=======================================================================
    void audioDeviceAboutToStart (AudioIODevice* device) override
    {
        // This function will be called when the audio device is started, or when
        // its settings (i.e. sample rate, block size, etc) are changed.

        // You can use this function to initialise any resources you might need,
        // but be careful - it will be called on the audio thread, not the GUI thread.

        // For more details, see the help for AudioProcessor::prepareToPlay()



    }
    //=======================================================================
    void audioDeviceStopped() override {

    }

    //=======================================================================
    void audioDeviceIOCallback (const float** inputChannelData, int numInputChannels,
                                float** outputChannelData, int numOutputChannels,
                                int numSamples) override {

    }


    //=======================================================================
    void paint (Graphics& g) override {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (Colours::white);

        // You can add your drawing code here!
    }

    //=======================================================================
    void resized() override {
        // This is called when the MainContentComponent is resized.
        // If you add any child components, this is where you should
        // update their positions.

        Rectangle<int> mainScreen = getLocalBounds().reduced(5);

        // Parameter positioning
        Rectangle<int> paramRect = mainScreen.removeFromTop(345);

        paramRect = paramRect.withTrimmedTop(20);
        groupParams.setBounds(paramRect);

        paramRect = paramRect.withTrimmedTop(40);

        size_t paramsPerCol = 5;
        Rectangle<int> paramRowRect = paramRect.removeFromTop(60).reduced(5);


        int paramWidth = paramRect.getWidth()/paramsPerCol;
        size_t numParams = 0;
        for (Component* paramComp : paramComponents) {
            paramComp->setBounds(paramRowRect.removeFromLeft(paramWidth).reduced(8));
            numParams++;
            if ( (numParams % paramsPerCol) == 0 ) {
                paramRowRect = paramRect.removeFromTop(60).reduced(5);
            }

        }

        Rectangle<int> fileButtons = mainScreen.removeFromTop(45);
        int buttonWidth = fileButtons.getWidth()/3;

        openButton.setBounds ( fileButtons.removeFromLeft(buttonWidth).reduced(8) );
        renderButton.setBounds ( fileButtons.removeFromLeft(buttonWidth).reduced(8) );
        bitDepthSelector.setBounds ( fileButtons.removeFromLeft(buttonWidth).reduced(8) );

        // Text box positioning
        groupLogger.setBounds(mainScreen);
        Rectangle<int> textbox = mainScreen.withTrimmedTop(7).reduced(5);
        Logger->setBounds (textbox);
    }

    //=======================================================================
    void sliderValueChanged (Slider* slider) override {
        for (size_t i = 0; i < paramComponents.size(); i++) {
            if (slider == paramComponents[i]) {
                myWdfTree->setParam(i,((Slider*)paramComponents[i])->getValue());
            }
        }
    }

    void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override {
        if (comboBoxThatHasChanged == &bitDepthSelector) {
            int idChosen = comboBoxThatHasChanged->getSelectedId();
            if (idChosen == 1) {
                myRenderParams.outputBitDepth = 16;
            } else if (idChosen == 2) {
                myRenderParams.outputBitDepth = 24;
            } else if (idChosen == 3) {
                myRenderParams.outputBitDepth = 32;
            }
        }
    }


    //=======================================================================
    void changeListenerCallback (ChangeBroadcaster* source) override {

    }

    //=======================================================================
    void buttonClicked (Button* button) override {
        if (button == &openButton) {
            openButtonClicked();
        }
        else if (button == &renderButton) {
            renderButtonClicked();
        }
        else {
            for (size_t i = 0; i < paramComponents.size(); i++) {
                if (button == paramComponents[i]) {
                    myWdfTree->setParam(i,((ToggleButton*)paramComponents[i])->getToggleState());
                }
            }
        }

    }



protected:
    //==============================================================================

    wdfTree* myWdfTree;

    AudioDeviceManager myDeviceManager;

    GroupComponent groupParams;
    GroupComponent groupLogger;
    ScopedPointer<TextEditor> Logger;

    r8b::CDSPResampler24* upSmplr24 = NULL;
    r8b::CDSPResampler24* downSmplr24 = NULL;

    double* upBuf = NULL;
    double* downBuf = NULL;

    int64 startupTime;

    RenderParams myRenderParams;
    RenderThread myRenderer;


    void openButtonClicked() {
        FileChooser chooser ("Select a Wave file to play...",
                             File::nonexistent,
                             "*.wav");

        if (chooser.browseForFileToOpen()) {
            File file (chooser.getResult());
            AudioFormatReader* reader = formatManager.createReaderFor (file);

            if (reader) {
                double Fs = reader->sampleRate;
                myRenderParams.inputSampleRate = Fs;
                myRenderParams.outputSampleRate = Fs;
                transportSource.prepareToPlay (512, Fs); //BS TODO

                ScopedPointer<AudioFormatReaderSource> newSource = new AudioFormatReaderSource (reader, true);
                transportSource.setSource (newSource, 0, nullptr, reader->sampleRate);
                renderButton.setEnabled (true);

                myWdfTree->adaptTree( );
                writeLogLine("'"+String(file.getFileName())+"' opened. Duration: "+String(transportSource.getLengthInSeconds())+"s");

                oversamplingRatio = myWdfTree->getSamplerate() / Fs;
                writeLogLine("Base samplerate: "+String(Fs)+"Hz");
                writeLogLine("wdfTree adapted for effective samplerate: "+String(myWdfTree->getSamplerate())+"Hz.");
                writeLogLine("Resulting OX="+String(oversamplingRatio));
                myRenderParams.treeSampleRate = myWdfTree->getSamplerate();

                if(upSmplr24) {
                    delete upSmplr24;
                    delete[] upBuf;
                }

                if(downSmplr24) {
                    delete downSmplr24;
                    delete[] downBuf;
                }

                upSmplr24 = new r8b::CDSPResampler24(Fs, myWdfTree->getSamplerate(), 512); //BS TODO
                downSmplr24 = new r8b::CDSPResampler24(myWdfTree->getSamplerate(), Fs, 512); //BS TODO

                size_t downBufSize = ceil(oversamplingRatio*512);
                upBuf = new double[512]; //BS TODO
                downBuf = new double[downBufSize]; //BS TODO

                writeLogLine("Resamplers set up.");


                readerSource = newSource.release();
            }
        }
    }

    void renderButtonClicked() {
        myRenderParams.myWdfTree = myWdfTree;
        myRenderParams.upSmplr24 = upSmplr24;
        myRenderParams.downSmplr24 = downSmplr24;
        myRenderParams.transportSource = &transportSource;
        myRenderParams.upBuf = upBuf;
        myRenderParams.downBuf = downBuf;
        myRenderParams.blockSize = 512; //TODO

        myRenderer.setRenderParamsPtr( &myRenderParams );
        writeLogLine("Rendering started");

        if( myRenderer.runThread(10) )
        {
            writeLogLine("Rendering finished");
            double audioTime = transportSource.getLengthInSeconds();
            writeLogLine("Audio Time: " + String(audioTime) + ". Rendering Time: " + String(myRenderParams.renderTime));
            writeLogLine("Realtimes needed: " + String((double)myRenderParams.renderTime/(double)audioTime));
            writeLogLine("Total Blocks: " + String((double)myRenderParams.totalBlocks) + ". Bad Blocks: " + String((double)myRenderParams.badBlocks));
        }
        else
        {
            writeLogLine("Rendering aborted");
        }
        myLookAndFeel.playAlertSound();
    }

    void createParamControls(const std::vector<paramData>& paramsIn) {
        for (paramData param : paramsIn) {
            if (param.type == boolParam) {
                ToggleButton* newButton = new ToggleButton;
                Label* newLabel = new Label;
                newLabel->attachToComponent(newButton, false);
                newLabel->setText(param.name, dontSendNotification);
                newButton->setButtonText(param.units);
                newButton->setToggleState(param.value, dontSendNotification);
                newButton->addListener (this);
                addAndMakeVisible (newButton);
                paramComponents.push_back(newButton);
                paramLabels.push_back(newLabel);
            }
            else if (param.type == doubleParam) {
                Slider* newSlider = new Slider;
                Label* newLabel = new Label;
                newLabel->attachToComponent(newSlider, false);
                newLabel->setText(param.name, dontSendNotification);
                newSlider->setSliderStyle(Slider::RotaryVerticalDrag);
                newSlider->setRange(param.lowLim, param.highLim, 0.001 * (param.highLim - param.lowLim));
                newSlider->setValue(param.value);
                newSlider->setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::black);
                newSlider->addListener (this);
                addAndMakeVisible (newSlider);
                paramComponents.push_back(newSlider);
                paramLabels.push_back(newLabel);
            }
        }
    }



    //==========================================================================
    TextButton openButton;
    TextButton renderButton;
    ComboBox bitDepthSelector;
    std::vector<Component*> paramComponents;
    std::vector<Component*> paramLabels;
    double oversamplingRatio;

    AudioFormatManager formatManager;
    ScopedPointer<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;

    LookAndFeel_V3 myLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
