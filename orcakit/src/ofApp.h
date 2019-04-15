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
        
        pdsp::Engine        engine;   
        pdsp::osc::Input    osc;
          
        pdsp::ParameterGain fader;

        np::synth::FMSub sub;
        float subgate = 0.0f;
        
        np2::synth::NoiseDevice  noiseN;
        np2::synth::NoiseDevice  noiseM;
        
        np2::synth::SinePercussion kick;
        
        np2::effect::HQSaturator lowEndSaturation;
        
        pdsp::OneMinusInput envInvert;
        pdsp::Amp           noiseduck0;
        pdsp::Amp           noiseduck1;
        
        pdsp::ParameterGain reverbGain;
        pdsp::BasiVerb    reverb;
        npl::effect::StereoDelay delays;

        ofxPanel lowendUI;
        ofxPanel noiseUI;
        ofxPanel fxUI;
        bool bDrawGui;
        
        np::dynamics::Brickwall limiter;
        pdsp::LowCut revcut;
        pdsp::LowCut delaycut;
        pdsp::LowCut dtcut;
        
};
