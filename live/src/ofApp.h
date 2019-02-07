#pragma once

#define NUMSYNTHS 4

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxPDSP.h"
#include "ofxGui.h"
#include "FMMono.h"
#include "ModalTable.h"
#include "StereoDelay.h"
#include "synth/DataSynth.h"
#include "effect/Chorus.h"
#include "effect/Filter.h"
#include "meter/RMS.h"
#include "ofxDotFrag.h"

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

        std::vector<np::synth::FMMono>  synths;

        
        pdsp::ParameterGain reverbGain;
        pdsp::BasiVerb    reverb;
        npl::effect::StereoDelay delays;

        np::tuning::ModalTable table;
        
        ofxPanel synthsGUI;
        ofxPanel fxGUI;
        ofxPanel tuningGUI;
        ofxPanel fragGUI;
        
        ofVideoGrabber webcam;
        int col;
        ofFbo waveplot;
        ofFbo camfbo;
        ofFbo process;
        ofx::dotfrag::Monochrome monochrome;
        ofx::dotfrag::HSB hsb;
        ofx::dotfrag::Twist twist;
        
        ofPixels pixels;

        np::synth::DataSynth dtsynth;
        np::effect::Filter filter;
        np::effect::Chorus chorus;
        std::vector<float> dtriggers;
        
        std::atomic<int> select;
        std::atomic<float> fragamount;
        
        ofx::dotfrag::Live polyicon;
        ofParameter<glm::vec4> envelopes;
        ofParameter<glm::vec4> crossmods;
        
        std::atomic<float> meter_cross[NUMSYNTHS];
        
        bool bDrawGui;
        
        ofParameter<ofColor> bandsColor;
        ofParameter<int> bandsWidth;
        ofParameter<int> bandsSeparation;
        ofParameter<int> camOffset;
        ofParameter<int> iconOffset;
        ofParameter<float> bandsThreshold;
        ofParameterGroup graphics;
        ofParameter<ofColor> waveColor;
        
        np::meter::RMS meterL;
        np::meter::RMS meterR;
};
