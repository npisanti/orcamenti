
#include "ofApp.h"

#define NUMSYNTHS 3

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetWindowTitle("fm trio");
    engine.score.setTempo( 120.0f); // the delay times are clocked

    osc.setVerbose( true );
    osc.openPort( 4444 );

    synths.resize( NUMSYNTHS );

    table.setup( 8, "modal table" );
    
    for( size_t i=0; i<synths.size(); ++i ){
        synths[i].out("gain") >> delays.ch(0);
        synths[i].out("gain") >> delays.ch(1);   
        synths[i].out("gain") >> reverbGain >> reverb; 
    }
    
    synths[0].out("gain") >> engine.audio_out(0);
    synths[0].out("gain") >> engine.audio_out(1);    
    synths[1].out("gain") * pdsp::panL( -0.3f ) >> engine.audio_out(0);
    synths[1].out("gain") * pdsp::panR( -0.3f ) >> engine.audio_out(1);
    synths[2].out("gain") * pdsp::panL( 0.3f ) >> engine.audio_out(0);
    synths[2].out("gain") * pdsp::panR( 0.3f ) >> engine.audio_out(1);

    synths[0].out("other") >> synths[1].in("other");
    synths[1].out("other") >> synths[2].in("other");
    synths[2].out("other") >> synths[0].in("other");
    
    delays.ch(0) >> reverb;
    delays.ch(1) >> reverb;
    
    reverb.ch(0) >> engine.audio_out(0);
    reverb.ch(1) >> engine.audio_out(1);
 
    delays.ch(0) >> engine.audio_out(0);
    delays.ch(1) >> engine.audio_out(1);

    // OSC mapping -----------------------------
    osc.linkTempo( "/orca/tempo" );
    oscMapping( "/a", 0 );
    oscMapping( "/b", 1 );
    oscMapping( "/c", 2 );
    
    int i=0;
    for( ; i<NUMSYNTHS; ++i ){
        osc.parser("/s", i*NUMSYNTHS ) = [&, i]( float value ) noexcept {
            synths[i].preset( int(value) ); return pdsp::osc::Ignore;
        };
        osc.parser("/s", i*NUMSYNTHS + 1 ) = [&, i]( float value ) noexcept {
            value *= 0.112;
            value = (value<1.0) ? value : 1.0;
            synths[i].slew( value ); return pdsp::osc::Ignore;
        };
    }
    
    // graphic setup---------------------------
    ofSetVerticalSync(true);
    ofDisableAntiAliasing();
    ofBackground(0);
    ofSetColor(ofColor(0,100,100));
    ofNoFill();
    ofSetLineWidth(1.0f);
    ofSetFrameRate(24);

    // GUI -----------------------------------
    ofxGuiSetHeaderColor(ofColor( 40, 0, 0));
    ofxGuiSetBorderColor(ofColor(40, 0, 0));
    
    ofxGuiSetFillColor(ofColor(255, 85, 85));
    ofxGuiSetBackgroundColor(ofColor(0,0,0));

    synthsGUI.setup("SYNTHS", "synths.xml", 220, 10);
        synthsGUI.add( synths[0].label("fm synth A") );
        synthsGUI.add( synths[1].label("fm synth B") );
    synthsGUI.loadFromFile("synths.xml");
    
    fxGUI.setup("FX", "fx.xml", 430, 10);
        fxGUI.add( delays.parameters );
        fxGUI.add( reverbGain.set("reverb gain", -12, -36, 0 ) );
    fxGUI.loadFromFile( "fx.xml" );
    
    tuningGUI.setup("TUNING", "tuning.xml", 10, 10 );
    tuningGUI.add( table.parameters );
    tuningGUI.loadFromFile( "tuning.xml" );
    
    // audio setup----------------------------
    engine.sequencer.play();
    
    engine.addOscInput ( osc );
    
    engine.listDevices();
    engine.setDeviceID(0); // REMEMBER TO SET THIS AT THE RIGHT INDEX!!!!
    engine.setup( 44100, 512, 3);     
}

        
//--------------------------------------------------------------
void ofApp::oscMapping( std::string address, int index ){

    osc.out_trig( address, 0 ) >> synths[index].in("trig");  
    osc.parser( address, 0 ) = [&, index]( float value ) noexcept {
        if( value==synths[index].lastTrigger ){ return pdsp::osc::Ignore;
        }else{ synths[index].lastTrigger = value; return value;  }
    };
    
    osc.out_value( address, 1 ) * 12.0f     >> synths[index].in("pitch");
    
    osc.out_value( address, 2 ) >> synths[index].in("pitch");
    osc.parser( address, 2 ) = [&]( float value ) noexcept {
        int i = value-1.0f;
        float p = table.pitches[i%table.grades];
        int o = i / table.grades;
        p += o*12.0f;
        return p;  
    };
    
    osc.out_value( address, 3 ) >> synths[index].in("decay");
    osc.parser( address, 3 ) = [&]( float value ) noexcept {
        value *= 0.112;
        value = (value<1.0) ? value : 1.0;
        value = value * value * 4000.0f;
        return value;  
    };
        
    osc.out_value( address, 4 ) >> synths[index].in("env_amount");

    osc.out_value( address, 5 ) * 0.25f >> synths[index].in("other_amount");
    
    osc.out_trig( address, 6 ) >> synths[index].in("trig_other");  
    osc.parser( address, 6 ) = [&, index]( float value ) noexcept {
        if( value==synths[index].lastOtherTrigger ){ return pdsp::osc::Ignore;
        }else{ synths[index].lastOtherTrigger = value; return value;  }
    };
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    tuningGUI.draw();
    synthsGUI.draw();
    fxGUI.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
