#pragma once

#define NUMSYNTHS 4

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxPDSP.h"
#include "ofxGui.h"
#include "FMSub.h"
#include "SinePercussion.h"
#include "NoiseDevice.h"
#include "StereoDelay.h"
#include "dynamics/Brickwall.h"
#include "ofxDotFrag.h"
#include "HQSaturator.h"
#include "ResoSampler.h"

#include "synth/DataSynth.h"
#include "effect/Chorus.h"
#include "effect/Filter.h"
#include "ofxDotFrag.h"
//#include "meter/RMS.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        
        void drawMeter(float value, float min, float max, int x, int y, int w, int h);
        
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        
        void oscMapping();
                
        std::function<float(float)> triggerCheck;
        
        pdsp::Engine        engine;   
        pdsp::osc::Input    osc;
          
        pdsp::ParameterGain fader;

        
        np::synth::FMSub sub;
        pdsp::ParameterGain subSend;
        
        np2::synth::NoiseDevice  noiseN;
        np2::synth::NoiseDevice  noiseM;
        
        np2::synth::SinePercussion kick;
        
        np2::effect::HQSaturator lowEndSaturation;
        
        pdsp::OneMinusInput envInvert;
        pdsp::Amp           noiseduck0;
        pdsp::Amp           noiseduck1;
        
        pdsp::ParameterGain reverbGain;
        pdsp::BasiVerb    reverb;

        ofxPanel lowendUI;
        ofxPanel noiseUI;
        ofxPanel samplersUI;
        ofxPanel fragsUI;
        ofxPanel fxUI;
        bool bDrawGui;
        
        np::dynamics::Brickwall limiter;
        pdsp::LowCut revcut;
        pdsp::LowCut delaycut;
        pdsp::LowCut dtcut;
        
        std::vector<np::synth::ResoSampler> samplers;
        std::vector<std::string> samplersAddresses;
        
        int mode;
        
        
        // webcam synth 
        ofVideoGrabber webcam;
        int col;
        ofFbo waveplot;
        ofFbo camfbo;
        ofFbo process;
        ofx::dotfrag::Monochrome monochrome;
        ofx::dotfrag::HSB hsb;
        ofx::dotfrag::Twist twist;
        
        ofPixels pixels;
        pdsp::Parameter dtsynthBasePitch;

        np::synth::DataSynth dtsynth;
        np::effect::Filter filter;
        np::effect::Chorus chorus;
        std::vector<float> dtriggers;
        
        std::atomic<int> select;
        std::atomic<float> fragamount;
        
        ofParameterGroup graphics;
        ofParameter<ofColor> bandsColor;
        ofParameter<ofColor> waveColor;
        
        ofParameterGroup calibration;
        ofParameter<int> offX;
        ofParameter<int> offY;
        ofParameter<int> linemin;
        ofParameter<int> linestep;
        
        void drawVideoDrone( int y );

};
