
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetWindowTitle("fm duo");
    engine.score.setTempo( 120.0f); // the delay times are clocked

    osc.setVerbose( true );
    osc.openPort( 4444 );

    synths.resize( 2 );
    enableSynth.resize( 2 );
    
    table.setup( 8, "modal table" );
    
    for( size_t i=0; i<synths.size(); ++i ){
        
        synths[i].out_gain() >> enableSynth[i];

        enableSynth[i] >> sendSwitch;        
        enableSynth[i] >> reverb;

        enableSynth[i] >> engine.audio_out(0);
        enableSynth[i] >> engine.audio_out(1);     
        
        enableSynth[i].enableSmoothing( 20.0f );   
    }
    sendSwitch.enableSmoothing( 20.0f );   

    synths[0] >> synths[1].in_other();
    synths[1] >> synths[0].in_other();
    
    delays.ch(0) >> reverb;
    delays.ch(1) >> reverb;
    
    reverb.ch(0) >> engine.audio_out(0);
    reverb.ch(1) >> engine.audio_out(1);
  
    sendSwitch >> delays.ch(0);
    sendSwitch >> delays.ch(1);    
    delays.ch(0) >> engine.audio_out(0);
    delays.ch(1) >> engine.audio_out(1);

    // ---------------- OSC CONTROL --------------------------------
    osc.linkTempo( "/orca/tempo" );
    
    osc.out_value( "/a", 1 ) * 12.0f     >> synths[0].in("pitch");
    osc.out_value( "/a", 2 ) >> synths[0].in("pitch");
    osc.out_trig( "/a", 0 ) >> synths[0].in("trig");  
    
    osc.out_value( "/b", 1 ) * 12.0f     >> synths[1].in("pitch");
    osc.out_value( "/b", 2 ) >> synths[1].in("pitch");
    osc.out_trig( "/b", 0 ) >> synths[1].in("trig");  
    
    osc.parser( "/a", 2 ) = [&]( float value ) noexcept {
        int i = value-1.0f;
        float p = table.pitches[i%table.grades];
        int o = i / table.grades;
        p += o*12.0f;
        return p;  
    };
    osc.parser( "/b", 2 ) = osc.parser( "/a", 2 );

    lastTrigA = lastTrigB = 0.0f;
    osc.parser( "/a", 0 ) = [&]( float value ) noexcept {
        if(value==lastTrigA){ return pdsp::osc::Ignore;
        }else{ lastTrigA = value; return value;  }
    };
    osc.parser( "/b", 0 ) = [&]( float value ) noexcept {
        if(value==lastTrigB){ return pdsp::osc::Ignore;
        }else{ lastTrigB = value; return value;  }
    };

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
    
    timeSelect.addListener( this, &ofApp::onTime );

    synthsGUI.setup("SYNTHS", "synths.xml", 240, 20);
        synthsGUI.add( enableSynth[0].set("enable A", true) );
        synthsGUI.add( synths[0].label("fm synth A") );
        synthsGUI.add( enableSynth[1].set("enable B", true) );
        synthsGUI.add( synths[1].label("fm synth B") );
    synthsGUI.loadFromFile("synths.xml");
    
    fxGUI.setup("FX", "fx.xml", 460, 20);
        fxGUI.add( timeSelect.set("time select", 0, 0, 9) );
        fxGUI.add( sendSwitch.set("send switch", false) );
        fxGUI.add( delays.parameters );
        fxGUI.add( reverb.parameters );
    fxGUI.loadFromFile( "fx.xml" );
    
    tuningGUI.setup("TUNING", "tuning.xml", 20, 20 );
    tuningGUI.add( table.parameters );
    tuningGUI.loadFromFile( "tuning.xml" );
    
    mapControls();
    
    // audio setup----------------------------
    engine.sequencer.play();
    
    engine.addOscInput ( osc );
    
    engine.listDevices();
    engine.setDeviceID(0); // REMEMBER TO SET THIS AT THE RIGHT INDEX!!!!
    engine.setup( 44100, 512, 3);     
}

//--------------------------------------------------------------
void ofApp::mapControls(){
    lc.setup();
    
    for( int i=0; i<2; ++i ){
        lc.knob( 0 + i*8, synths[i].fm_other.getOFParameterFloat());
        lc.knob( 1 + i*8, synths[i].self_mod.getOFParameterFloat(), 0.0f, 0.3f );
        lc.knob( 2 + i*8, synths[i].fm_mod.getOFParameterFloat());
        lc.knob( 3 + i*8, synths[i].ratioSelect );
        lc.knob( 4 + i*8, synths[i].attack );
        lc.knob( 5 + i*8, synths[i].decay );
        lc.knob( 6 + i*8, synths[i].slew );
    }
    
    lc.knob( 7+8, delays.inputFader.getOFParameterInt(), -24, 0 );
    lc.knob( 7, delays.lFeedbackControl.getOFParameterFloat() );
    lc.knob( 7, delays.rFeedbackControl.getOFParameterFloat() );
    
    lc.toggle( 0, enableSynth[0].getOFParameterBool() );
    lc.toggle( 1, enableSynth[1].getOFParameterBool() );
    lc.radio( 2, 5, timeSelect, ofxLCLeds::Amber );
    lc.toggle( 6, delays.modAmt.getOFParameterFloat(), 0.0f, 3.5f );
    lc.toggle( 7, sendSwitch.getOFParameterBool() );
}

//--------------------------------------------------------------
void ofApp::onTime( int & value ){
    switch( value ){
        case 0:
            delays.lDelayTimeControl.getOFParameterInt() = 16;
            delays.rDelayTimeControl.getOFParameterInt() = 16;
        break;
        
        case 1:
            delays.lDelayTimeControl.getOFParameterInt() = 4;
            delays.rDelayTimeControl.getOFParameterInt() = 4;
        break;
        
        case 2:
            delays.lDelayTimeControl.getOFParameterInt() = 3;
            delays.rDelayTimeControl.getOFParameterInt() = 3;
        break;
        
        case 3:
            delays.lDelayTimeControl.getOFParameterInt() = 2;
            delays.rDelayTimeControl.getOFParameterInt() = 2;
        break;
    }
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
    switch( key ){
        case '1': engine.sequencer.setTempo( 60.0f); break;
        case '2': engine.sequencer.setTempo( 120.0f); break;
        case '3': engine.sequencer.setTempo( 180.0f); break;
        case '4': engine.sequencer.setTempo( 240.0f); break;
        case '5': engine.sequencer.setTempo( 300.0f); break;
        
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
