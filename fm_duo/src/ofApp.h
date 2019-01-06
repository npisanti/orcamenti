#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxPDSP.h"
#include "ofxGui.h"

#include "ofxLaunchControllers.h"

#include "effect/BasiVerb.h"
#include "effect/Chorus.h"
#include "effect/StereoDelay.h"

#include "FMMono.h"
#include "ModalTable.h"

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
        
        void mapControls();
        
        pdsp::Engine        engine;   
        pdsp::osc::Input    osc;
    
        float lastTrigA;
        float lastTrigB;
    
        ofxLaunchControl lc;
          
        pdsp::ParameterGain fader;
        
        ofParameter<int> timeSelect;
            void onTime( int & value );

        std::vector<np::synth::FMMono>  synths;
        
        np::effect::BasiVerb    reverb;
        np::effect::StereoDelay delays;
        
        std::vector<pdsp::ParameterAmp> enableSynth;
        pdsp::ParameterAmp sendSwitch;

        np::tuning::ModalTable table;
        
        ofxPanel synthsGUI;
        ofxPanel fxGUI;
        ofxPanel tuningGUI;
};
