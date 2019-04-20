
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    config = "settings.xml";
    
    ofSetWindowTitle("|::|");
    engine.score.setTempo( 120.0f); 
    
    mode = 1;
    bDrawGui = false;

    osc.setVerbose( true );
    osc.openPort( 4444 );

    samplers.resize( 8 );
    samplersAddresses.resize( samplers.size() );
    
    samplersAddresses[0] = "a";
    samplersAddresses[1] = "b";
    samplersAddresses[2] = "c";
    samplersAddresses[3] = "d";
    samplersAddresses[4] = "e";
    samplersAddresses[5] = "f";
    samplersAddresses[6] = "g";
    samplersAddresses[7] = "h";
    
    samplersUI.resize( samplers.size() );
    
    for( size_t i=0; i<samplers.size(); ++i ){
        samplers[i].out("L") >> limiter.ch(0);
        samplers[i].out("R") >> limiter.ch(1);
        samplers[i].out("send_rev") >> revcut;
        samplers[i].out("send_del") >> delaycut;
    }
    
    for( size_t i=1; i<samplers.size(); ++i ){
        samplers[i-1].out("duck_send") >> samplers[i].in("duck_in");
    }

    // ---------- master FX -------------
    
    delaycut >> delays.ch(0);
    delaycut >> delays.ch(1);
    delays.ch(0) >> limiter.ch(0);
    delays.ch(1) >> limiter.ch(1);
        
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

    for( size_t i=0; i<samplers.size(); ++i ){
        std::string label = "sampler "+samplersAddresses[i];
        std::string file = "sampler_"+samplersAddresses[i] + ".xml";
        
        samplersUI[i].setup( label, file, 20+220*i, 20 ); 
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

    table.setup( 12 );
    tuningUI.setup("TUNING", "tuning.xml", 20, 20);
    tuningUI.add( table.parameters );    
    //tuningUI.loadFromFile( "tuning.xml" );    
    
    fxUI.setup("FX", "fx.xml", 240, 20);
        fxUI.add( delays.parameters );
        fxUI.add( reverb.parameters );
        fxUI.add( limiter.parameters );
    //fxUI.loadFromFile("fx.xml");
    
    global.add( table.parameters );    
    global.add( delays.parameters );
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

    for( int i=0; i< int( samplers.size() ); ++i ){
        std::string ad = std::string("/") + samplersAddresses[i];
        
        osc.out_trig( ad, 0) * (1.0f/8.0f) >> samplers[i].in_trig();
        osc.parser( ad, 0) = [&](float value){
            if( value == 0.0f){
                return pdsp::osc::Ignore;
            }else if( value > 8.0f ){
                return 8.0f;
            }else{
                return value;
            }
        };
        
        osc.out_value( ad, 1) >> samplers[i].in("mod");     
        osc.parser( ad, 1) = [&, i]( float value ) noexcept {
            if( samplers[i].useTuning ){
                int v = value;
                float p = table.pitches[v%table.degrees];
                int o = v / table.degrees;
                p += o*12.0f;
                return p;                  
            }else{
                return value;
            }
        };        
        
        osc.out_value( ad, 2) >> samplers[i].in("hold");  
        osc.parser( ad, 2) = [&]( float value ) noexcept { 
            return value * pdsp::Clockable::getOneBarTimeMs() * ( 1.0f/16.0f);
        };
        
        osc.out_value( ad, 3) * (1.0f/16.0f)>> samplers[i].in("start");  
    }
    
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    switch( mode ){
        case 1:
            for( size_t i=0; i<samplersUI.size(); ++i ){
                samplersUI[i].draw();
            }
        break;
        
        case 2:
            tuningUI.draw();
            fxUI.draw();           
        break;
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch( key ){
        case 'g': bDrawGui = !bDrawGui; break;
        case '1': mode = 1; break;
        case '2': mode = 2; break;
        
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
