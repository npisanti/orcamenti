
// FMSub.cpp
// Nicola Pisanti, MIT License, 2018

#include "FMSub.h"

ofParameterGroup & np::synth::FMSub::setup(){
    
    // -------------------------- PATCHING ------------------------------------
    voices.resize( 2 );
 
    self_mod.channels( voices.size() );
    fm_mod.channels( voices.size() );

    for( int i=0; i<2; ++i ){
        // setup voice
        voices[i].setup( *this, i );
        node_gate >> voices[i].in("trig");
        node_pitch >> voices[i].in("pitch");
    }

    detune_ctrl * -1.0f >> voices[0].in("pitch");
    detune_ctrl         >> voices[1].in("pitch");
        
    
    voices[0] >> duck;
    voices[1] >> osc2_amp >> duck >> gain;
 
    // CONTROLS ---------------------------------------------------------------
    parameters.setName( "fm sub" );
    
    parameters.add( gain.set("gain", -12, -48, 12) );
    parameters.add( osc2_amp.set("osc2", 0.0f, 0.0f, 1.0f) );
    parameters.add( detune_ctrl.set("detune", 0.0f, 0.0f, 0.5f) );
    
    parameters.add( ratio_ctrl.set( "ratio coarse", 1, 1, 13) );
    parameters.add( ratio_ctrl.set( "ratio fine", 0.0f, -1.0f, 0.0f) );
    parameters.add( fm_ctrl.set( "fm", 0.0f, 0.0f, 4.0f) );    
    parameters.add( fm_mod.set( "fm mod", 0.0f, 0.0f, 4.0f) );
    parameters.add( self_ctrl.set("self fb", 0.0f, 0.0f, 1.0f) );
    parameters.add( self_mod.set("self fb mod", 0.2f, 0.0f, 0.5f) );    
    parameters.add( drift.set("drift", 0.0f, 0.0f, 1.0f) );    
    
    parameters.add(env_attack_ctrl.set( "env attack", 400, 0, 4000) );
    parameters.add(env_decay_ctrl.set(  "env decay", 400, 5, 10000) );
    parameters.add(env_sustain_ctrl.set("env sustain", 1.0f, 0.0f, 1.0f) );
    parameters.add(env_release_ctrl.set("env release", 900, 5, 8000));    

    gain.enableSmoothing(50.f);
    self_ctrl.enableSmoothing( 200.0f );
    fm_ctrl.enableSmoothing( 200.0f );
    ratio_ctrl.enableSmoothing( 200.0f );
    
    return parameters;
}


void np::synth::FMSub::Voice::setup( FMSub & m, int i ){

    addModuleInput("trig", voiceTrigger);
    addModuleInput("pitch", pitchNode);
    addModuleInput("fm", fmAmp.in_mod());
    addModuleInput("self", carrier.in_fb());
    addModuleOutput("signal", voiceAmp);

    
    pitchNode >> carrier.in_pitch();
    pitchNode >> modulator.in_pitch();
    m.ratio_ctrl >> modulator.in_ratio();
    m.fm_ctrl >> fmAmp.in_mod();
    m.self_ctrl >> carrier.in_fb();
    
    
    // SIGNAL PATH
    modulator >> fmAmp >> carrier.in_fm() >> voiceAmp;
    
        
    // MODULATIONS AND CONTROL
    envelope >> m.fm_mod.ch(i)   >> fmAmp.in_mod();
    envelope >> m.self_mod.ch(i) >> carrier.in_fb();
    
    voiceTrigger >> envelope >> voiceAmp.in_mod();

        m.env_attack_ctrl  >> envelope.in_attack();
        m.env_decay_ctrl   >> envelope.in_decay();
        m.env_sustain_ctrl >> envelope.in_sustain();
        m.env_release_ctrl >> envelope.in_release();


        
    0.2f    >> phazorFree;
    0.05f  >> randomSlew.in_freq();
                                       m.drift >> driftAmt.in_mod();        
    phazorFree.out_trig() >> rnd >> randomSlew >> driftAmt;
                                                  driftAmt >> carrier.in_pitch();
                                                  driftAmt >> modulator.in_pitch();
        
}

float np::synth::FMSub::Voice::meter_mod_env() const{
    return envelope.meter_output();
}

float np::synth::FMSub::Voice::meter_pitch() const{
    return carrier.meter_pitch();
}

pdsp::Patchable& np::synth::FMSub::ch( size_t index ){
    pdsp::wrapChannelIndex( index );
    switch( index ){
        case 0: return gain.ch(0); break;
        case 1: return gain.ch(1); break;
    }
    return gain.ch(0);
}

pdsp::Patchable& np::synth::FMSub::Voice::in_trig(){
    return in("trig");
}

pdsp::Patchable& np::synth::FMSub::Voice::in_pitch(){
    return in("pitch");
}
pdsp::Patchable& np::synth::FMSub::Voice::in_fm(){
    return in("fm");
}

pdsp::Patchable& np::synth::FMSub::Voice::in_self(){
    return in("self");
}

ofParameterGroup & np::synth::FMSub::label (std::string name ){
    parameters.setName( name );
    return parameters;
}
