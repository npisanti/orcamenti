
// ResoSampler.cpp
// Nicola Pisanti, MIT License, 2018

#include "ResoSampler.h"

void np::synth::ResoSampler::patch(){
   
    addModuleInput("trig", triggers );
    addModuleInput("pitch", pitchNode );
    addModuleOutput("mono", fader );
    
    env.enableDBTriggering(-24.0f, 0.0f);
    
    drift        >> driftAmt;
    driftControl >> driftAmt.in_mod();
                    driftAmt >> sampler.in_pitch();

                    pitchNode >> sampler.in_pitch();
                 tuningControl >> sampler.in_pitch();
    
    startControl >> sampler.in_start();
    startModControl >> sampler.in_start_mod();
    
    triggers >> drift.in_trig();
    triggers >> sampler >> amp;
    triggers >> env     >> amp.in_mod();
    
                sampler >> decimate >> filter >> amp >> fader;
                
    decimP2F        >> decimate.in_freq();
    decimateControl >> decimP2F;
    
    cutoffControl   >> filter.in_cutoff();
    resoControl     >> filter.in_reso();
    modeControl     >> filter.in_mode();
    
    triggers >> fEnv >> envToFilter >> filter.in_cutoff();
                fEnv >> envToDecimate >> decimP2F;
    
    sampler.addSample( &sample, 0 );

    envDynControl >> env.in_velocity();
    attackControl  >> env.in_attack();
    holdControl    >> env.in_hold();
    releaseControl >> env.in_release();

    modEnvDynControl  >> fEnv.in_velocity();
    modAttackControl  >> fEnv.in_attack();
    modHoldControl    >> fEnv.in_hold();
    modReleaseControl >> fEnv.in_release();

    parameters.setName( "quicksampler");
    parameters.add( fader.set("volume", 0, -48, 24) );
    parameters.add( loadButton.set( "load", false ));
    parameters.add( sampleName.set("sample", "no sample"));
    parameters.add( samplePath.set("path", "no path"));
    parameters.add( startControl.set("start", 0.0f, 0.0f, 1.0f));
    parameters.add( startModControl.set("dyn start mod", 0.0f, 0.0f, 1.0f));
    parameters.add( tuningControl.set("pitch", 0, -24, 24));
    parameters.add( driftControl.set("pitch drift", 0.0f, 0.0f, 0.5f));
    
    parameters.add( attackControl.set("amp attack", 0, 0, 600));
    parameters.add( holdControl.set("amp hold", 0, 0, 600));
    parameters.add( releaseControl.set("amp release", 0, 0, 600));
    parameters.add( envDynControl.set("amp dynamics", 1.0f, 0.0f, 1.0f));
    
    parameters.add( modAttackControl.set("mod attack", 0, 0, 600));
    parameters.add( modHoldControl.set("mod hold", 0, 0, 600));
    parameters.add( modReleaseControl.set("mod release", 0, 0, 600));
    parameters.add( modEnvDynControl.set("mod dynamics", 0.5f, 0.0f, 1.0f));
    
    parameters.add( decimateControl.set( "decimate", 152, 20, 152) );
    parameters.add( envToDecimate.set("env to decimate", 0, 0, 120));

    parameters.add( cutoffControl.set( "filter cutoff", 136, 20, 136) );
    parameters.add( resoControl.set( "filter reso", 0.0f, 0.0f, 1.0f) );
    parameters.add( modeControl.set( "filter type", 0, 0, 5) );
    parameters.add( envToFilter.set("env to filter", 0, 0, 120));
    
    loadButton.addListener(this, &ResoSampler::loadButtonCall );
    samplePath.addListener(this, &ResoSampler::sampleChangedCall );
    
}

ofParameterGroup & np::synth::ResoSampler::label( std::string name ){
    parameters.setName( name );
    return parameters;
}

void np::synth::ResoSampler::loadButtonCall( bool & value ) {
    if(value){

        int fvalue = fader.getOFParameterInt().get();
        fader.getOFParameterInt().set( -60 );
        
        //Open the Open File Dialog
        ofFileDialogResult openFileResult= ofSystemLoadDialog("select an audio sample"); 
        
        //Check if the user opened a file
        if (openFileResult.bSuccess){
            
            std::string path = openFileResult.getPath();
            
            samplePath = path;
        
            ofLogVerbose("file loaded");
            
        }else {
            ofLogVerbose("User hit cancel");
        }


        loadButton = false;
        
        fader.getOFParameterInt().set(fvalue);
    }
}

void np::synth::ResoSampler::sampleChangedCall( std::string & value ) {
    
    ofLogVerbose( "loading" + value );
    loadSample( samplePath );    

    auto v = ofSplitString(samplePath, "/" );
    sampleName = v[v.size()-1];
    
}
    
void np::synth::ResoSampler::loadSample( std::string path ) {
    
    sample.load( path );
    
    int sample_length_ms = ((double) sample.length * 1000.0) / sample.fileSampleRate;
    
    holdControl.set("hold time", sample_length_ms, 0, sample_length_ms*2);
    releaseControl.set("release time", 50, 0, sample_length_ms*2);
    
    modHoldControl.set("hold time", sample_length_ms, 0, sample_length_ms*2);
    modReleaseControl.set("release time", 50, 0, sample_length_ms*2);
    
}

void np::synth::ResoSampler::load( std::string path ) {
    samplePath = path;
}

void np::synth::ResoSampler::dBTriggering( bool enable ) {
    if(enable){
        env.enableDBTriggering(-24.0f, 0.0f);
    }else{
        env.disableDBTriggering();
    }
}

pdsp::Patchable & np::synth::ResoSampler::in_trig() {
    return in("trig");
}
pdsp::Patchable & np::synth::ResoSampler::in_pitch() {
    return in("pitch");
}
