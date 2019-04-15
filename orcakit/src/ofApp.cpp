
#include "ofApp.h"

/* ORCAKIT TODO 
    MESSAGES
        + trigger: control dynamics, indipendent amt for mod and amp envs
        + mod : usually pitch 
        + hold : hold env time
    SYNTHS
        + r reese sub : trigger / mod / hold 
                + slew amt parameter  
        + k kickzap : trigger / mod 
            + k&r ---> saturator 
        + nm noiseN : trigger / mod / hold 
            + exclusive 
        + abcdef samplers with possibility of karplus synthesis 
            + mix to dimension chorus 
        + t delay time/fb control    
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

    noiseN.ch(0) >> limiter.ch(0);
    noiseN.ch(1) >> limiter.ch(1);
    
    noiseN.out("env") >> envInvert >> noiseduck0.in_mod();
                         envInvert >> noiseduck1.in_mod();

    noiseM.ch(0) >> noiseduck0 >> limiter.ch(0);
    noiseM.ch(1) >> noiseduck1 >> limiter.ch(1);


    kick.out_env() >> sub.in("ducking");    
    kick >> lowEndSaturation;
    sub >> lowEndSaturation;
    
    lowEndSaturation >> limiter.ch(0);
    lowEndSaturation >> limiter.ch(1);

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

    lowendUI.setup("LOW END", "lowend.xml", 20, 20);
        lowendUI.add( sub.parameters );
        lowendUI.add( kick.parameters );
        lowendUI.add( lowEndSaturation.label("low end saturation") );
    lowendUI.loadFromFile("lowend.xml");
    
    noiseUI.setup("NOISE", "noise.xml", 240, 20);
        noiseUI.add( noiseN.label( "noise n") );
        noiseUI.add( noiseM.label( "noise m") );
    noiseUI.loadFromFile("noise.xml");
    
    fxUI.setup("FX", "fx.xml", 460, 20);
        fxUI.add( limiter.parameters );
        fxUI.add( delays.parameters );
        fxUI.add( reverbGain.set("reverb gain", -12, -36, 0 ) );
    fxUI.loadFromFile("fx.xml");
    
    
    // audio setup----------------------------
    engine.sequencer.play();
    
    engine.addOscInput ( osc );
    
    engine.listDevices();
    engine.setDeviceID(0); 
    
    engine.setup( 44100, 512, 3);     
}

        
//--------------------------------------------------------------
void ofApp::oscMapping(){

    osc.out_value("/k", 1) >> kick.in("pitch");  
    osc.out_trig("/k", 0) * (1.0f/8.0f) >> kick.in("trig");  

    osc.out_trig("/r", 0) * (1.0f/8.0f) >> sub.in_trig();
    osc.out_value("/r", 1) >> sub.in_pitch();
    osc.out_value("/r", 2) >> sub.in("hold");  
    osc.parser("/r", 2) = [&]( float value ) noexcept { 
        return value * pdsp::Clockable::getOneBarTimeMs() * (1.0f/16.0f);
    };
 
    // parse 0 as -60 and the rest as pitch 
    osc.out_trig("/n", 0) * (1.0f/8.0f) >> noiseN.in("trig");  
    osc.out_value("/n", 1) >> noiseN.in("mod");     
    osc.out_value("/n", 2) >> noiseN.in("hold");  
    osc.parser("/n", 2) = [&]( float value ) noexcept { 
        return value * pdsp::Clockable::getOneBarTimeMs() * (1.0f/(16.0f*8.0f));
    };
    
    osc.out_trig("/m", 0) * (1.0f/8.0f) >> noiseM.in("trig");  
    osc.out_value("/m", 1) >> noiseM.in("mod");     
    osc.out_value("/m", 2) >> noiseM.in("hold");  
    osc.parser("/m", 2) = [&]( float value ) noexcept { 
        return value * pdsp::Clockable::getOneBarTimeMs() * (1.0f/(16.0f*8.0f));
    };



}

//--------------------------------------------------------------
void ofApp::update(){
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    lowendUI.draw();
    noiseUI.draw();
    fxUI.draw();
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
