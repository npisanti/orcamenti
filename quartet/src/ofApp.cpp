
#include "ofApp.h"

#define NUMSYNTHS 4

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetWindowTitle("fm quartet");
    engine.score.setTempo( 120.0f); // the delay times are clocked

    osc.setVerbose( true );
    osc.openPort( 4444 );

    synths.resize( NUMSYNTHS );

    table.setup( 6, "modal table" );
    
    for( size_t i=0; i<synths.size(); ++i ){
        synths[i].out("gain") >> delays.ch(0);
        synths[i].out("gain") >> delays.ch(1);   
        synths[i].out("gain") >> reverbGain >> reverb; 
    }
    
    for( int i=0; i<NUMSYNTHS; ++i ){
        synths[i].out("gain") * pdsp::panL(pdsp::spread(i, NUMSYNTHS, 0.7f) ) >> engine.audio_out(0);
        synths[i].out("gain") * pdsp::panR(pdsp::spread(i, NUMSYNTHS, 0.7f) ) >> engine.audio_out(1);  
    }

    synths[0].out("signal") >> synths[1].in("other");
    synths[1].out("signal") >> synths[0].in("other");
    synths[2].out("signal") >> synths[3].in("other");
    synths[3].out("signal") >> synths[2].in("other");

    delays.ch(0) >> reverb;
    delays.ch(1) >> reverb;
    
    reverb.ch(0) >> engine.audio_out(0);
    reverb.ch(1) >> engine.audio_out(1);
 
    delays.ch(0) >> engine.audio_out(0);
    delays.ch(1) >> engine.audio_out(1);

    // OSC mapping -----------------------------
    osc.linkTempo( "/orca/tempo" );
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
    ofxGuiSetHeaderColor(ofColor( 40, 0, 0));
    ofxGuiSetBorderColor(ofColor(40, 0, 0));
    
    ofxGuiSetFillColor(ofColor(255, 85, 85));
    ofxGuiSetBackgroundColor(ofColor(0,0,0));

    synthsGUI.setup("SYNTHS", "synths.xml", 220, 10);
        synthsGUI.add( synths[0].label("fm synth A") );
        synthsGUI.add( synths[1].label("fm synth B") );
        synthsGUI.add( synths[2].label("fm synth C") );
        synthsGUI.add( synths[3].label("fm synth D") );
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
    engine.setDeviceID(0); 
    
    engine.setup( 44100, 512, 3);     
}

        
//--------------------------------------------------------------
void ofApp::oscMapping(){

    for( int index = 0; index< NUMSYNTHS; ++index ){
        std::cout<< "MAPPING SYNTH "<<index<<"\n";
        osc.out_trig("/x", index) >> synths[index].in("trig");  
        osc.parser("/x", index) = [&, index]( float value ) noexcept {
            if( value==synths[index].lastTrigger ){ return pdsp::osc::Ignore;
            }else{ synths[index].lastTrigger = value; return value;  }
        };
        
        osc.out_value("/x", index+NUMSYNTHS) >> synths[index].in("pitch");
        osc.parser("/x", index+NUMSYNTHS) = [&]( float value ) noexcept {
            int i = value;
            float p = table.pitches[i%table.degrees];
            int o = i / table.degrees;
            p += o*12.0f;
            return p;  
        };        
        
        osc.out_value("/c", index) * 0.125f >> synths[index].in("other_amount");

        osc.out_value("/r", index) >> synths[index].in("ratio");
        osc.parser("/r", index) = [&, index]( float value ) noexcept {
            if( value==0.0f ){ 
                return 0.5f;
            }else if( value>=17.0f ){
                 return 17.0f;
            }else{
                return value;
            }
        };

        osc.out_value("/o", index) * 12.0f     >> synths[index].in("pitch");
            
        osc.out_value("/s", index) * 0.03f >> synths[index].in("fb_amount");        

        osc.out_value("/f", index) * 0.1f >> synths[index].in("fm_amount");

        osc.out_value("/e", index) >> synths[index].in("decay");
        osc.parser("/e", index) = [&]( float value ) noexcept {
            value *= 0.112;
            value = (value<1.0) ? value : 1.0;
            value = value * value * 4000.0f;
            return value;  
        };      
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
