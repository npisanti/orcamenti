#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxPDSP.h"
#include "ofxGui.h"

#include "ModalTable.h"

#include "StereoDelay.h"
#include "dynamics/Brickwall.h"
#include "effect/BasiVerb.h"
#include "TSampler.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

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
        
        np::effect::BasiVerb reverb;

        npl::effect::StereoDelay delays;


        std::vector<ofxPanel> samplersUI;
        ofxPanel tuningUI;
        ofxPanel fxUI;

        bool bDrawGui;
        
        np::dynamics::Brickwall limiter;
        pdsp::LowCut revcut;
        pdsp::LowCut delaycut;

        std::vector<np::synth::TSampler> samplers;
        std::vector<std::string> samplersAddresses;
        
        np::tuning::ModalTable table;
        int mode;
        
        ofxPanel global;
        std::string config;
        
};
