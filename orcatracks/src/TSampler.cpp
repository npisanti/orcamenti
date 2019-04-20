
// TSampler.cpp
// Nicola Pisanti, MIT License, 2018

#include "TSampler.h"

void np::synth::TSampler::patch(){
   
    addModuleInput("trig", triggers );
    addModuleInput("mod", modNode );
    addModuleInput("hold", env.in_hold() );
    addModuleInput("start", sampler.in_start_mod() );
    addModuleInput("duck_in", invert );
    addModuleOutput("mono", fader );
    addModuleOutput("send_rev", revSend );
    addModuleOutput("send_del", delSend );
    addModuleOutput("duck_send", duckNext );
    addModuleOutput("L", panner.out_L() );
    addModuleOutput("R", panner.out_R() );
    
    env.enableDBTriggering(-24.0f, 0.0f);
    
    sampler >> decimate >> filter >> amp >> comb >> combMix.in_B();
                                     amp >> combMix.in_A();
    combMix >> duck >> fader >> panner;
     invert >> duck.in_mod();
     env >> duckNext;
    
    fader >> revSend;
    fader >> delSend;
    
    drift        >> driftAmt;
    driftControl >> driftAmt.in_mod();
                    driftAmt >> sampler.in_pitch();

    lfoPhasor >> lfoSine >> lfoMix.in_A();
    lfoPhasor >> lfoTri  >> lfoMix.in_B();
                            lfoMix >> combVibrato >> comb.in_pitch();
    combVibratoSpeed >> combP2F >> lfoPhasor.in_freq();
    combVibratoShape >> lfoMix.in_fade();
    
    combMixControl >> combMix.in_fade();
    combTuneControl >> comb.in_pitch();
    combFBControl >> comb.in_feedback();
    combDampingControl >> comb.in_damping();
    fEnv >> combEnvMod >> comb.in_feedback();

            tuningControl >> sampler.in_pitch();
    modNode >> modToPitch >> sampler.in_pitch();
    
    modNode >> modToFilter   >> filter.in_cutoff();
    modNode >> modToDecimate >> decimP2F;
    modNode >> modToComb >> comb.in_pitch();
    
    panControl >> panner.in_pan();
    
    startControl >> sampler.in_start();
    startModControl >> sampler.in_start_mod();
    
    triggers >> drift.in_trig();
    triggers >> sampler >> amp;
    triggers >> env     >> amp.in_mod();
    
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

    parameters.setName( "track sampler");
    
    sampleGroup.setName("sample");
    sampleGroup.add( loadButton.set( "load", false ));
    sampleGroup.add( sampleName.set("sample", "no sample"));
    sampleGroup.add( samplePath.set("path", "no path"));
    sampleGroup.add( startControl.set("start", 0.0f, 0.0f, 1.0f));

    parameters.add( sampleGroup );
    
    tuningGroup.setName("tuning");
    tuningGroup.add( tuningControl.set("pitch coarse", 0, -36, 36));
    tuningGroup.add( tuningControl.set("pitch fine", 0.0f, -0.5f, 0.5f));
    tuningGroup.add( driftControl.set("pitch drift", 0.0f, 0.0f, 0.5f));
    
    parameters.add( tuningGroup );
    
    
    filterGroup.setName( "filtering");
    filterGroup.add( decimateControl.set( "decimate", 152, 20, 152) );
    filterGroup.add( envToDecimate.set("env to decimate", 0, 0, 120));

    filterGroup.add( cutoffControl.set( "filter cutoff", 136, 20, 136) );
    filterGroup.add( resoControl.set( "filter reso", 0.0f, 0.0f, 1.0f) );
    filterGroup.add( modeControl.set( "filter type", 0, 0, 5) );
    filterGroup.add( envToFilter.set("env to filter", 0, 0, 120));
    parameters.add( filterGroup );

    combGroup.setName( "resonator");
    combGroup.add( combMixControl.set("mix", 0.0f, 0.0f, 0.5f ) );
    combGroup.add( combTuneControl.set("tune", 48, -24, 120 ) );
    combGroup.add( combTuneControl.set("fine", 0.0f, -0.5f, 0.5f ) );
    combGroup.add( combFBControl.set("feedback", 0.8f, 0.0f, 1.0f ) );
    combGroup.add( combDampingControl.set("damping", 0.0f, 0.0f, 1.0f ) );
    combGroup.add( combVibrato.set("vibrato", 0.0f, 0.0f, 4.0f ) );
    combGroup.add( combVibratoSpeed.set("vibrato speed", 0, -30, 80 ) );
    combGroup.add( combVibratoShape.set("vibrato shape", 0.0f, 0.0f, 1.0f ) );
    combGroup.add( combEnvMod.set("env to feedback", 0.0f, 0.0f, 1.0f ) );
    parameters.add( combGroup );

    ampEnvGroup.setName( "volume envelope");
    ampEnvGroup.add( attackControl.set( "attack", 0, 0, 600));
    ampEnvGroup.add( holdControl.set( "hold", 0, 0, 600));
    ampEnvGroup.add( releaseControl.set( "release", 0, 0, 600));
    ampEnvGroup.add( envDynControl.set( "dynamics", 1.0f, 0.0f, 1.0f));
    ampEnvGroup.add( startModControl.set("dyn sample mod", 0.0f, 0.0f, 1.0f));
    parameters.add( ampEnvGroup );
    
    modEnvGroup.setName( "mod envelope");
    modEnvGroup.add( modAttackControl.set( "attack", 0, 0, 600));
    modEnvGroup.add( modHoldControl.set( "hold", 0, 0, 600));
    modEnvGroup.add( modReleaseControl.set( "release", 0, 0, 600));
    modEnvGroup.add( modEnvDynControl.set( "dynamics", 0.5f, 0.0f, 1.0f));
    parameters.add( modEnvGroup );
    
    extModGroup.setName( "mod input" );
    extModGroup.add( useTuning.set("use tuning", true) );
    extModGroup.add( modToPitch.set("to pitch", 1.0f, -12.0f, 12.0));
    extModGroup.add( modToFilter.set("to filter", 0.0f, -12.0f, 12.0f));
    extModGroup.add( modToDecimate.set("to decimate", 0.0f, -12.0f, 12.0f));
    extModGroup.add( modToComb.set("to comb", 0.0f, -12.0f, 12.0f));
    parameters.add( extModGroup );
    
    mixer.setName("mixer channel");
    mixer.add( fader.set("volume", -12, -48, 24) );
    mixer.add( panControl.set("pan", 0.0f, -1.0f, 1.0f) );
    mixer.add( revSend.set("reverb send", -48, -48, 12) );
    mixer.add( delSend.set("delay send", -48, -48, 12) );
    mixer.add( duckNext.set("duck next", false) );
    parameters.add( mixer );
    
    loadButton.addListener(this, &TSampler::loadButtonCall );
    samplePath.addListener(this, &TSampler::sampleChangedCall );
    
}

ofParameterGroup & np::synth::TSampler::label( std::string name ){
    parameters.setName( name );
    return parameters;
}

void np::synth::TSampler::loadButtonCall( bool & value ) {
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

void np::synth::TSampler::sampleChangedCall( std::string & value ) {
    
    ofLogVerbose( "loading" + value );
    loadSample( samplePath );    

    auto v = ofSplitString(samplePath, "/" );
    sampleName = v[v.size()-1];        

}
    
void np::synth::TSampler::loadSample( std::string path ) {
    
    sample.load( path );
    
    int sample_length_ms = ((double) sample.length * 1000.0) / sample.fileSampleRate;
    
    holdControl.set("hold time", sample_length_ms, 0, sample_length_ms*2);
    releaseControl.set("release time", 50, 0, sample_length_ms*2);
    
    modHoldControl.set("hold time", sample_length_ms, 0, sample_length_ms*2);
    modReleaseControl.set("release time", 50, 0, sample_length_ms*2);
    
}

void np::synth::TSampler::load( std::string path ) {
    samplePath = path;
}

void np::synth::TSampler::dBTriggering( bool enable ) {
    if(enable){
        env.enableDBTriggering(-24.0f, 0.0f);
    }else{
        env.disableDBTriggering();
    }
}

pdsp::Patchable & np::synth::TSampler::in_trig() {
    return in("trig");
}
pdsp::Patchable & np::synth::TSampler::in_mod() {
    return in("mod");
}
