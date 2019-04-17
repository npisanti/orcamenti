
#include "ofApp.h"

#define FRAGW 320
#define CAMW 240
#define CAMH 240

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetWindowTitle("|::|");
    engine.score.setTempo( 120.0f); 
    
    mode = 0;
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
    //samplersAddresses[5] = "f";
    
    for( size_t i=0; i<samplers.size(); ++i ){
        samplers[i].out("L") >> limiter.ch(0);
        samplers[i].out("R") >> limiter.ch(1);
        samplers[i].out("send") >> revcut;
    }
    
    for( size_t i=1; i<samplers.size(); ++i ){
        samplers[i-1].out("duck_send") >> samplers[i].in("duck_in");
    }

    // ------------- datasynth -------------

    dtsynth.setup( 8 );
    dtriggers.resize( dtsynth.voices.size() );
    for(size_t i=0; i<dtsynth.voices.size(); ++i){
        dtriggers[i] = 0.0f;
    }
    
    for( size_t index = 0; index<dtsynth.voices.size(); ++index ){
        dtsynthBasePitch >> dtsynth.voices[index].in("pitch");  
    }
    dtsynth.ch(0) >> dtcut >> filter >> reverbGain >> revcut;
                              filter >> chorus.ch(0) >> limiter.ch(0);
                              filter >> chorus.ch(1) >> limiter.ch(1);

    100.0f >> dtcut.in_freq();

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

    lowendUI.setup("LOW END", "lowend.xml", 20, 10);
        lowendUI.add( sub.parameters );
        lowendUI.add( kick.parameters );
        lowendUI.add( lowEndSaturation.label("low end saturation") );
        lowendUI.add( subSend.set("sub send", -20, -48, 0 ) );
    lowendUI.loadFromFile("lowend.xml");
    
    noiseUI.setup("NOISE", "noise.xml", 20, 10);
        noiseUI.add( noiseN.label( "noise n") );
        noiseUI.add( noiseM.label( "noise m") );
    noiseUI.loadFromFile("noise.xml");
    
    samplersUI.setup("SAMPLERS", "samplers.xml", 20, 10);
        for( size_t i=0; i<samplers.size(); ++i ){
            samplersUI.add( samplers[i].label("sampler "+samplersAddresses[i]));
        }
    samplersUI.loadFromFile("samplers.xml");
    samplersUI.minimizeAll();
    
    fragsUI.setup("FRAGS", "frags.xml", 20, 10 );
        fragsUI.add( dtsynth.parameters );
        fragsUI.add( dtsynthBasePitch.set("base pitch", 36, 12, 72) );
    
        fragsUI.add( chorus.parameters );
    
        fragsUI.add( reverbGain.set("reverb send", -12, -48, 24 ) );
        
        calibration.setName( "calibration" );
        calibration.add( offX.set("off x", 0, 0, 640) );
        calibration.add( offY.set("off y", 0, 0, 480) );
        calibration.add( linemin.set("line min", 40, 0, CAMW) );
        calibration.add( linestep.set("line step", 5, 1, 20) );
        fragsUI.add( calibration );
        
        fragsUI.add( monochrome.parameters );
        fragsUI.add( hsb.parameters );
        fragsUI.add( twist.parameters );
        
        graphics.setName("graphis");
            graphics.add( waveColor.set("wave color", ofColor(255), ofColor(0), ofColor(255) ) );
            graphics.add( bandsColor.set("bands color", ofColor(255), ofColor(0), ofColor(255) ) );
        fragsUI.add( graphics );
    fragsUI.loadFromFile( "frags.xml" );
  
  
    fxUI.setup("FX", "fx.xml", 20, 10);
        fxUI.add( limiter.parameters );
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

    for( size_t index = 0; index<dtsynth.voices.size(); ++index ){
        osc.out_trig("/t", index) >> dtsynth.voices[index].in("trig");  
        osc.parser("/t", index) = [&, index]( float value ) noexcept {
            if( value==dtriggers[index] ){ return pdsp::osc::Ignore;
            }else{ dtriggers[index]  = value; return value;  }
        };        
        
        osc.out_value("/p", index) >> dtsynth.voices[index].in("pitch");  
        osc.out_value("/p", index).enableSmoothing( 125.0f );
    }

    osc.parser("/v", 0) = [&]( float value ) noexcept {
        select = value;
        return pdsp::osc::Ignore;
    };        

    osc.parser("/v", 1) = [&]( float value ) noexcept {
        fragamount = value / 16.0f;
        return pdsp::osc::Ignore;
    };    
    
    osc.out_value("/v", 2 ) * 15.0 >> filter.in_cutoff();
    osc.out_value("/v", 2 ).enableSmoothing( 5000.0f ); 
    osc.out_value("/v", 3 ) * 12.0 >> filter.in_cutoff();
    osc.out_value("/v", 3 ).enableSmoothing( 20.0f ); 
    
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
        
        
        // ------------------ GENERATING THE WAVE --------------------
		// converting pixels to waveform samples
        dtsynth.datatable.begin();
        for(int n=0; n<CAMH; ++n){
            int pixel = pixels[ col*4 + n*4*CAMW ];
            float sample = ofMap(pixel, 0, 255, -0.5f, 0.5f);
            dtsynth.datatable.data(n, sample);
        }
        dtsynth.datatable.end(false);

		// ----------------- PLOTTING THE WAVEFORM -------------------
    
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
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    switch( mode ){
        case 0:
        {
            drawVideoDrone( 100 );
        }
        break;
        
        case 1: 
            lowendUI.draw();
        break;
        
        case 2: 
            noiseUI.draw();
        break;
        
        case 3: 
            samplersUI.draw();
        break;
        
        case 4: 
            drawVideoDrone( 780 );
            fragsUI.draw();
        break;
        
        case 5: 
            fxUI.draw();
        break;
    }
    
}

void ofApp::drawVideoDrone( int y ){
    ofPushMatrix();    
        ofTranslate( (ofGetWidth()+CAMH)/2, y );
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
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch( key ){
        case 'g': bDrawGui = !bDrawGui; break;
        
        case '0': mode = 0; break;
        case '1': mode = 1; break;
        case '2': mode = 2; break;
        case '3': mode = 3; break;
        case '4': mode = 4; break;
        case '5': mode = 5; break;
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
