
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
        
        synths[i].out("gain") >> enableSynth[i];

        enableSynth[i] >> sendSwitch;        
        enableSynth[i] >> reverb;

        enableSynth[i] >> engine.audio_out(0);
        enableSynth[i] >> engine.audio_out(1);     
        
        enableSynth[i].enableSmoothing( 20.0f );   
    }
    sendSwitch.enableSmoothing( 20.0f );   

    synths[0].out("other") >> synths[1].in("other");
    synths[1].out("other") >> synths[0].in("other");
    
    delays.ch(0) >> reverb;
    delays.ch(1) >> reverb;
    
    reverb.ch(0) >> engine.audio_out(0);
    reverb.ch(1) >> engine.audio_out(1);
  
    sendSwitch >> delays.ch(0);
    sendSwitch >> delays.ch(1);    
    delays.ch(0) >> engine.audio_out(0);
    delays.ch(1) >> engine.audio_out(1);

    // OSC mapping -----------------------------
    osc.linkTempo( "/orca/tempo" );
    oscMapping( "/a", 0 );
    oscMapping( "/b", 1 );
    
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

    synthsGUI.setup("SYNTHS", "synths.xml", 220, 10);
        synthsGUI.add( enableSynth[0].set("enable A", true) );
        synthsGUI.add( synths[0].label("fm synth A") );
        synthsGUI.add( enableSynth[1].set("enable B", true) );
        synthsGUI.add( synths[1].label("fm synth B") );
    synthsGUI.loadFromFile("synths.xml");
    
    fxGUI.setup("FX", "fx.xml", 430, 10);
        fxGUI.add( timeSelect.set("time select", 0, 0, 9) );
        fxGUI.add( sendSwitch.set("send switch", false) );
        fxGUI.add( delays.parameters );
        fxGUI.add( reverb.parameters );
    fxGUI.loadFromFile( "fx.xml" );
    
    tuningGUI.setup("TUNING", "tuning.xml", 10, 10 );
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
void ofApp::mapControls(){
    lc.setup();
    /*
    for( int i=0; i<2; ++i ){
        lc.knob( 0 + i*8, synths[i].fm_other.getOFParameterFloat());
        //lc.knob( 1 + i*8, synths[i].self_mod.getOFParameterFloat(), 0.0f, 0.3f );
        //lc.knob( 2 + i*8, synths[i].fm_mod.getOFParameterFloat());
        //lc.knob( 3 + i*8, synths[i].ratioSelect );
        lc.knob( 4 + i*8, synths[i].attack );
        //lc.knob( 5 + i*8, synths[i].decay );
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
    */
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
