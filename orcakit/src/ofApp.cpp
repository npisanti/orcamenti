
#include "ofApp.h"

/* ORCAKIT TODO
        + k kickzap : trigger / pitch 
        + n noise : trigger / hold / pitch / degrade / cutoff / send 
        + r reese sub : trigger / hold / pitch / cutoff
        + uio 3x crossmod sine perc 
            + trigger / release / pitch / send
            + always 100% crossmod 
        + wavesynth
            + y 8x voice control
            + p 8x trigger control
            + w cutoff / wave control 
                        ( 35 waves + wave0=empty )
        + xyz karplus 3 voices : trigger / decay / pitch / send
        + abcd 4 samplers : trigger / hold / pitch / start / send
        + e delay time/fb control    
    + orcakit configfile.xml 
        - loads config file 
*/

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetWindowTitle("|::|");
    engine.score.setTempo( 120.0f); // the delay times are clocked

    bDrawGui = false;

    osc.setVerbose( true );
    osc.openPort( 4444 );

    table.setup( 6, "modal table" );
    
    sub.setup();
    
    kick >> limiter.ch(0);
    kick >> limiter.ch(1);
    
    noise.ch(0) >> limiter.ch(0);
    noise.ch(1) >> limiter.ch(1);


    kick.out_env() >> sub.invert;
    sub.gain >> limiter.ch(0);
    sub.gain >> limiter.ch(1);

    // ---------- master FX -------------
    
    reverbGain >> revcut >> reverb; 

    delays.ch(0) >> reverb;
    delays.ch(1) >> reverb;
    
    reverb.ch(0) >> limiter.ch(0);
    reverb.ch(1) >> limiter.ch(1);
 
    delays.ch(0) >> limiter.ch(0);
    delays.ch(1) >> limiter.ch(1);
   
    limiter.ch(0) >> engine.audio_out(0);
    limiter.ch(1) >> engine.audio_out(1);


    100.0f >> revcut.in_freq();
    100.0f >> delaycut.in_freq();


    // OSC mapping -----------------------------
    osc.linkTempo( "/orca/bpm" );
    oscMapping();
    
    // graphic setup---------------------------
    ofSetVerticalSync(true);
    ofDisableAntiAliasing();
    ofBackground(0);
    ofSetColor(ofColor(0,100,100));
    ofNoFill();
    ofSetLineWidth(1.0f);
    ofSetFrameRate(24);

    // GUI -----------------------------------

    gui.setup("SETTINGS", "settings.xml", 220, 10);
        gui.add( sub.parameters );
        gui.add( kick.parameters );
        gui.add( noise.parameters );
        gui.add( limiter.parameters );
        gui.add( delays.parameters );
        gui.add( reverbGain.set("reverb gain", -12, -36, 0 ) );
        gui.add( table.parameters );
    gui.loadFromFile("settings.xml");
    gui.minimizeAll();
    
    
    // audio setup----------------------------
    engine.sequencer.play();
    
    engine.addOscInput ( osc );
    
    engine.listDevices();
    engine.setDeviceID(0); 
    
    engine.setup( 44100, 512, 3);     
}

        
//--------------------------------------------------------------
void ofApp::oscMapping(){

    osc.out_value("/k", 0) >> kick.in("pitch");  
    osc.out_trig("/k", 1) * (1.0f/8.0f) >> kick.in("trig");  


    osc.out_value("/n", 0) * 12.0f >> noise.in("pitch");      
    // parse 0 as -60 and the rest as pitch 
    osc.out_value("/n", 1) * (1.0f/8.0f) >> noise.in("trig");  
    osc.out_value("/n", 2) >> noise.in("hold");  
    osc.parser("/n", 2) = [&]( float value ) noexcept { 
        return value * pdsp::Clockable::getOneBarTimeMs() * (1.0f/(16.0f*8.0f));
    };
    osc.out_value("/n", 3) * -3.0f >> noise.in("decimate");  
    osc.out_value("/n", 4) * -2.0f >> noise.in("filter");  

    osc.out_value("/r", 0) >> sub.node_pitch;
    osc.parser("/r", 0) = [&]( float value ) noexcept {
        int i = value;
        float p = table.pitches[i%table.degrees];
        int o = i / table.degrees;
        p += o*12.0f;
        return p;  
    };           
    osc.out_trig("/r", 1) * (1.0f/8.0f) >> sub.node_gate;
    osc.parser("/r", 1) = [&]( float value ) noexcept {
        if( value==subgate ){ return pdsp::osc::Ignore;
        }else{ subgate = value; return value;  }
    };
    


}

//--------------------------------------------------------------
void ofApp::update(){
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch( key ){
        case 'g': bDrawGui = !bDrawGui; break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
