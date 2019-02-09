
#include "ofApp.h"

#define FRAGW 320
#define CAMW 240
#define CAMH 240

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetWindowTitle("|::|");
    engine.score.setTempo( 120.0f); // the delay times are clocked

    bDrawGui = false;

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

    synths[1].out("signal") >> synths[0].in("other");
    synths[2].out("signal") >> synths[1].in("other");
    synths[3].out("signal") >> synths[2].in("other");
    synths[0].out("signal") >> synths[3].in("other");

    delays.ch(0) >> reverb;
    delays.ch(1) >> reverb;
    
    delays.ch(0) >> meterL >> engine.blackhole();
    delays.ch(1) >> meterR >> engine.blackhole();
    
    reverb.ch(0) >> engine.audio_out(0);
    reverb.ch(1) >> engine.audio_out(1);
 
    delays.ch(0) >> engine.audio_out(0);
    delays.ch(1) >> engine.audio_out(1);


    dtsynth.setup( 8 );
    dtriggers.resize( dtsynth.voices.size() );
    for(size_t i=0; i<dtsynth.voices.size(); ++i){
        dtriggers[i] = 0.0f;
    }
    
    dtsynth.ch(0) >> filter >> reverbGain;
                     filter >> chorus.ch(0) >> engine.audio_out(0);
                     filter >> chorus.ch(1) >> engine.audio_out(1);

    webcam.setDeviceID(0);
    webcam.setDesiredFrameRate(60);
    webcam.initGrabber( 640, 480 );
    
    dtsynth.datatable.setup( CAMH, CAMH ); // as many samples as the webcam width
    col = 40;
    waveplot.allocate( CAMW, CAMH );
    
    camfbo.allocate( CAMW, CAMH );
    monochrome.allocate( camfbo );
    hsb.allocate( camfbo );
    
    process.allocate( CAMW, CAMH, GL_RGBA );    
    twist.allocate( process );

    select = 0;
    fragamount = 0.0f;
    linecontrol = 0;

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

    synthsGUI.setup("SYNTHS", "synths.xml", 220, 10);
        synthsGUI.add( synths[0].label("fm synth A") );
        synthsGUI.add( synths[1].label("fm synth B") );
        synthsGUI.add( synths[2].label("fm synth C") );
        synthsGUI.add( synths[3].label("fm synth D") );
        synthsGUI.add( dtsynth.parameters );
        synthsGUI.add( filter.parameters );
        synthsGUI.add( chorus.parameters );
    synthsGUI.loadFromFile("synths.xml");
    
    fxGUI.setup("FX", "fx.xml", 10, 320);
        fxGUI.add( delays.parameters );
        fxGUI.add( reverbGain.set("reverb gain", -12, -36, 0 ) );
    fxGUI.loadFromFile( "fx.xml" );
    
    tuningGUI.setup("TUNING", "tuning.xml", 10, 10 );
    tuningGUI.add( table.parameters );
    tuningGUI.loadFromFile( "tuning.xml" );
        
    fragGUI.setup("FRAGS", "frags.xml", 10, 600 );
        calibration.setName( "calibration" );
        calibration.add( offX.set("off x", 0, 0, 640) );
        calibration.add( offY.set("off y", 0, 0, 480) );
        calibration.add( linemin.set("line min", 40, 0, CAMW) );
        calibration.add( linestep.set("line step", 5, 1, 20) );
        fragGUI.add( calibration );
        
        fragGUI.add( monochrome.parameters );
        fragGUI.add( hsb.parameters );
        fragGUI.add( twist.parameters );
        
        graphics.setName("graphis");
            graphics.add( waveColor.set("wave color", ofColor(255), ofColor(0), ofColor(255) ) );
            graphics.add( bandsColor.set("bands color", ofColor(255), ofColor(0), ofColor(255) ) );
            graphics.add( camOffset.set( "cam offset", 0, 0, ofGetHeight()));
            graphics.add( iconOffset.set( "icon offset", 320, 0, ofGetHeight()));
            graphics.add( bandsWidth.set("bands width", 60, 0, 200) );
            graphics.add( bandsSeparation.set("bands separation", 10, 0, 200) );
            graphics.add( bandsThreshold.set("bands threshold", 1.0f, 0.0f, 1.0f) );
            graphics.add( meterL.label("meter L") );
            graphics.add( meterR.label("meter R") );
        fragGUI.add( graphics );
    fragGUI.loadFromFile( "frags.xml" );
    

    polyicon.load( ofToDataPath("polyicon.frag"));
    polyicon.uniform( envelopes.set( "envelopes", glm::vec4(0),glm::vec4(0),glm::vec4(1) ), "u_envelopes" );
    polyicon.uniform( crossmods.set( "crossmods", glm::vec4(0),glm::vec4(0),glm::vec4(1) ), "u_crossmods" );
    
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
        osc.parser("/c", index) = [&, index]( float value ) noexcept {
            float crossmodulating = (value>0.0f) ? 1.0f : 0.0f;
            meter_cross[index].store(crossmodulating);
            return value;  
        };      

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

    for( size_t index = 0; index<dtsynth.voices.size(); ++index ){
        
        osc.out_trig("/t", index) >> dtsynth.voices[index].in("trig");  
        osc.parser("/t", index) = [&, index]( float value ) noexcept {
            if( value==dtriggers[index] ){ return pdsp::osc::Ignore;
            }else{ dtriggers[index]  = value; return value;  }
        };        
        osc.out_value("/p", index) >> dtsynth.voices[index].in("pitch");  
        osc.parser("/p", index) = [&]( float value ) noexcept {
            int i = value;
            float p = table.pitches[i%table.degrees];
            int o = i / table.degrees;
            p += o*12.0f;
            return p;  
        };        
        osc.out_value("/p", index).enableSmoothing( 75.0f );
    }

    
    osc.parser("/v", 0) = [&]( float value ) noexcept {
        linecontrol = value;
        return pdsp::osc::Ignore;
    };      
    
    osc.out_value("/v", 1 ) * 15.0 >> filter.in_cutoff();
    osc.out_value("/v", 1 ).enableSmoothing( 3000.0f ); 
    osc.out_value("/v", 2 ) * 12.0 >> filter.in_cutoff();
    osc.out_value("/v", 2 ).enableSmoothing( 20.0f ); 
    
    osc.parser("/v", 3) = [&]( float value ) noexcept {
        select = value;
        return pdsp::osc::Ignore;
    };        

    osc.parser("/v", 4) = [&]( float value ) noexcept {
        fragamount = value / 16.0f;
        return pdsp::osc::Ignore;
    };        

}

//--------------------------------------------------------------
void ofApp::update(){
    
    webcam.update();
    if( webcam.isFrameNew() ){
        camfbo.begin();
            ofClear( 0, 0, 0, 0 );
            ofSetColor( 255 );
            webcam.draw( -offX, -offY );
        camfbo.end();
        hsb.apply( camfbo );
        monochrome.apply( camfbo );
    }
    
    if( dtsynth.datatable.ready() ){
        
        process.begin();
            ofSetColor( 255 );
            camfbo.draw( 0, 0 );
        process.end();
        
        int mode = select;
        float amount = fragamount;
        
        switch( mode ){
            default: case 0: break;
            
            case 1:
                twist.amount = amount * amount * amount * 0.5f;
                twist.apply( process );
            break;
        }
		
        process.readToPixels( pixels );
        
        
        // ------------------ GENERATING THE WAVE ----------------------
		// converting pixels to waveform samples
        dtsynth.datatable.begin();
        for(int n=0; n<CAMH; ++n){
            int pixel = pixels[ col*4 + n*4*CAMW ];
            float sample = ofMap(pixel, 0, 255, -0.5f, 0.5f);
            dtsynth.datatable.data(n, sample);
        }
        dtsynth.datatable.end(false);

		// ----------------- PLOTTING THE WAVEFORM ---------------------
    
        waveplot.begin();
		ofClear(0, 0, 0, 0);
		ofSetColor(255);
        
		// plot the raw waveforms
        int xmin = CAMW * 0.95;
        int xmax = CAMW * 0.05;
        ofBeginShape();
        for(int n=0; n<CAMH; ++n){
            float x = ofMap(pixels.getData()[col*4 + n*4*CAMW], 0, 255, xmin, xmax);
            ofVertex( x, n );
        }
        ofEndShape(false);

		waveplot.end();
    }
    
    envelopes.set( glm::vec4( synths[0].meter_env(), synths[1].meter_env(), synths[2].meter_env(), synths[3].meter_env() ));
    
    float cross0 = meter_cross[0] * synths[1].meter_env();
    float cross1 = meter_cross[1] * synths[2].meter_env();
    float cross2 = meter_cross[2] * synths[3].meter_env();
    float cross3 = meter_cross[3] * synths[0].meter_env();   
    crossmods.set( glm::vec4( cross0, cross1, cross2, cross3 ) );

    col = linecontrol*linestep + linemin;
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofPushMatrix();
        ofTranslate( ofGetWidth()/2, 0 );
        ofFill();
        
        float delayLrms = ofMap( meterL.meter(), 0.0f, bandsThreshold, 0, 255, true );
        ofSetColor( bandsColor, delayLrms );
        ofDrawRectangle( -bandsSeparation, 0, -bandsWidth, ofGetHeight() );

        float delayRrms = ofMap( meterR.meter(), 0.0f, bandsThreshold, 0, 255, true );
        ofSetColor( bandsColor, delayRrms );
        ofDrawRectangle( bandsSeparation, 0, bandsWidth, ofGetHeight() );
    ofPopMatrix();

    polyicon.draw( (ofGetWidth()-FRAGW)/2, iconOffset, FRAGW, FRAGW );
    
    ofPushMatrix();    
        ofTranslate( (ofGetWidth()+CAMH)/2, camOffset + CAMH );
        ofRotateRad( M_PI_2 );

        ofSetColor( 255 );
        process.draw(0, 0);
        
        ofSetColor( 100, 130 );
        for( int n=0; n<32; ++n ){
            int x = linemin + linestep * n;
            ofDrawLine( x, 0, x, CAMH*0.05 );
            ofDrawLine( x, CAMH*0.95, x, CAMH );
        }
        
        ofSetColor( bandsColor );
        ofDrawLine(  col, 0, col, CAMH );

        float max = 0.0f;
        ofNoFill();
        for( size_t i=0; i<dtsynth.voices.size(); ++i ){
            float env = dtsynth.voices[i].meter_env();
            if( env > max ){ max = env; }
        }
        ofSetColor( waveColor, max * 160 );
        waveplot.draw( 0, 0 );
    ofPopMatrix();
    
    if(bDrawGui){
        tuningGUI.draw();
        synthsGUI.draw();
        fxGUI.draw();
        fragGUI.draw();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch( key ){
        case 'g': bDrawGui = !bDrawGui; break;
        case 'v': webcam.videoSettings(); break;
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
