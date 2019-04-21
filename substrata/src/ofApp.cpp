
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetWindowTitle("|::|");
    engine.score.setTempo( 120.0f); 

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
    sub >> subSend >> revcut;
    
    lowEndSaturation >> limiter.ch(0);
    lowEndSaturation >> limiter.ch(1);
    

    samplers.resize( 5 );
    samplersAddresses.resize( samplers.size() );
    
    samplersAddresses[0] = "a";
    samplersAddresses[1] = "b";
    samplersAddresses[2] = "c";
    samplersAddresses[3] = "d";
    samplersAddresses[4] = "e";
    
    samplersUI.resize( samplers.size() );
    
    for( size_t i=0; i<samplers.size(); ++i ){
        samplers[i].out("L") >> limiter.ch(0);
        samplers[i].out("R") >> limiter.ch(1);
        samplers[i].out("send_rev") >> revcut;
    }
    
    for( size_t i=1; i<samplers.size(); ++i ){
        samplers[i-1].out("duck_send") >> samplers[i].in("duck_in");
    }    
    

    // ---------- master FX -------------
    
    revcut >> reverb; 
    reverb.ch(0) >> limiter.ch(0);
    reverb.ch(1) >> limiter.ch(1);
   
    limiter.ch(0) >> engine.audio_out(0);
    limiter.ch(1) >> engine.audio_out(1);

    100.0f >> revcut.in_freq();

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

    global.setup("settings", config, 20, 20 );

    lowendUI.setup("LOW END", "lowend.xml", 20, 10);
        lowendUI.add( sub.parameters );
        lowendUI.add( kick.parameters );
        lowendUI.add( lowEndSaturation.label("low end saturation") );
        lowendUI.add( subSend.set("sub send", -20, -48, 0 ) );
    
    noiseUI.setup("NOISE", "noise.xml", 240, 10);
        noiseUI.add( noiseN.label( "noise n") );
        noiseUI.add( noiseM.label( "noise m") );

    for( size_t i=0; i<samplers.size(); ++i ){
        std::string label = "sampler "+samplersAddresses[i];
        std::string file = "sampler_"+samplersAddresses[i] + ".xml";
        
        samplersUI[i].setup( label, file, 20+220*(i+2), 10 ); 
        samplersUI[i].add( samplers[i].loadButton );
        samplersUI[i].add( samplers[i].samplePath );
        samplersUI[i].add( samplers[i].sampleName );
        samplersUI[i].add( samplers[i].tuningGroup );
        samplersUI[i].add( samplers[i].ampEnvGroup );
        samplersUI[i].add( samplers[i].filterGroup );
        samplersUI[i].add( samplers[i].combGroup );
        samplersUI[i].add( samplers[i].modEnvGroup );
        samplersUI[i].add( samplers[i].extModGroup );
        samplersUI[i].add( samplers[i].mixer );
        //samplersUI[i].loadFromFile( file );
        samplersUI[i].minimizeAll();
        
        global.add( samplers[i].label(label) );
    }    

    fxUI.setup("FX", "fx.xml", 20+220*7, 10);
        fxUI.add( reverb.parameters );
        fxUI.add( limiter.parameters );

    global.add( sub.parameters );
    global.add( kick.parameters );
    global.add( lowEndSaturation.parameters );
    global.add( subSend.getOFParameterInt() );
    global.add( noiseN.parameters );
    global.add( noiseM.parameters );
    global.add( reverb.parameters );
    global.add( limiter.parameters );
    global.loadFromFile( config );

    
    // audio setup----------------------------
    engine.sequencer.play();
    
    engine.addOscInput ( osc );
    
    engine.listDevices();
    engine.setDeviceID(0); 
    
    engine.setup( 44100, 512, 3);     
}

    
//--------------------------------------------------------------
void ofApp::oscMapping(){

    triggerCheck =[&](float value){
        if( value == 0.0f){
            return pdsp::osc::Ignore;
        }else if( value > 8.0f ){
            return 8.0f;
        }else{
            return value;
        }
    };

    osc.out_value("/k", 1) >> kick.in("pitch");  
    osc.out_trig("/k", 0) * (1.0f/8.0f) >> kick.in("trig");  

    osc.out_trig("/r", 0) * (1.0f/8.0f) >> sub.in_trig();
    osc.parser("/r", 0) = triggerCheck;
    osc.out_value("/r", 1) >> sub.in_pitch();
    osc.out_value("/r", 2) >> sub.in("hold");  
    osc.parser("/r", 2) = [&]( float value ) noexcept { 
        return value * pdsp::Clockable::getOneBarTimeMs() * (1.0f/16.0f);
    };
 
 
    // parse 0 as -60 and the rest as pitch 
    osc.out_trig("/n", 0) * (1.0f/8.0f) >> noiseN.in("trig");  
    osc.parser("/n", 0) = triggerCheck;
    osc.out_value("/n", 1) >> noiseN.in("mod");     
    osc.out_value("/n", 2) >> noiseN.in("hold");  
    osc.parser("/n", 2) = [&]( float value ) noexcept { 
        return value * pdsp::Clockable::getOneBarTimeMs() * (1.0f/(16.0f*8.0f));
    };
    
    osc.out_trig("/m", 0) * (1.0f/8.0f) >> noiseM.in("trig");  
    osc.parser("/m", 0) = triggerCheck;
    osc.out_value("/m", 1) >> noiseM.in("mod");     
    osc.out_value("/m", 2) >> noiseM.in("hold");  
    osc.parser("/m", 2) = [&]( float value ) noexcept { 
        return value * pdsp::Clockable::getOneBarTimeMs() * (1.0f/(16.0f*8.0f));
    };

    for( size_t i=0; i<samplers.size(); ++i ){
        std::string ad = std::string("/") + samplersAddresses[i];
        
        osc.out_trig( ad, 0) * (1.0f/8.0f) >> samplers[i].in_trig();
        osc.parser( ad, 0) = triggerCheck;
        osc.out_value( ad, 1) >> samplers[i].in("mod");     
        osc.out_value( ad, 2) >> samplers[i].in("hold");  
        osc.parser( ad, 2) = [&]( float value ) noexcept { 
            return value * pdsp::Clockable::getOneBarTimeMs() * ( 1.0f/16.0f);
        };
    }
    
}

//--------------------------------------------------------------
void ofApp::update(){

    
}

//--------------------------------------------------------------
void ofApp::draw(){
    lowendUI.draw();
    noiseUI.draw();
    for( size_t i=0; i<samplersUI.size(); ++i ){
        samplersUI[i].draw();
    }
    fxUI.draw();    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch( key ){
        case 's': global.saveToFile(config); break;
        case 'l': global.loadFromFile(config); break;
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
