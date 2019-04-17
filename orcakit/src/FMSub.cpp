
// FMSub.cpp
// Nicola Pisanti, MIT License, 2018

#include "FMSub.h"

void np::synth::FMSub::patch(){
    
    addModuleInput("trig", voiceTrigger);
    addModuleInput("pitch", pitchNode);
    addModuleInput("hold", ampEnv.in_hold() );
    addModuleInput("ducking", invert );
    addModuleOutput("signal", gain);

                     basePitch >> carrierA.in_pitch();
    pitchNode >> carrierAScale >> carrierA.in_pitch();
                     basePitch >> carrierB.in_pitch();
    pitchNode >> carrierBScale >> carrierB.in_pitch();
    
    detune_ctrl * -1.0f >> carrierA.in_pitch();
    detune_ctrl         >> carrierB.in_pitch();
    
    basePitch >> modulator.in_pitch();
    pitchNode >> modulatorScale >> modulator.in_pitch();
    
    ratio_ctrl >> modulator.in_ratio();
    fm_ctrl >> fmAmp.in_mod();
    self_ctrl >> carrierA.in_fb();
    self_ctrl >> carrierB.in_fb();
    
    
    // SIGNAL PATH
    modulator >> fmAmp >> carrierA.in_fm() >> oscXFader.in_A();
                 fmAmp >> carrierB.in_fm() >> oscXFader.in_B();
                 
    osc_mix >> oscXFader.in_fade();
    
    //oscXFader >> voiceAmp >> gain;
    oscXFader >> voiceAmp >> duck >> gain;
    invert >> duck.in_mod();
    
        
    // MODULATIONS AND CONTROL
    modEnv >> fm_mod >> fmAmp.in_mod();
    modEnv >> self_mod >> carrierA.in_fb();
    modEnv >> self_mod >> carrierB.in_fb();
    
    voiceTrigger >> ampEnv >> voiceAmp.in_mod();
    
    envVeloControl    >> ampEnv.in_velocity();
    envAttackControl  >> ampEnv.in_attack();
    envHoldControl    >> ampEnv.in_hold();
    envReleaseControl >> ampEnv.in_release();
    
    voiceTrigger >> modEnv;
    modEnvVeloControl    >> modEnv.in_velocity();
    modEnvAttackControl  >> modEnv.in_attack();
    modEnvHoldControl    >> modEnv.in_hold();
    modEnvReleaseControl >> modEnv.in_release();

    0.2f    >> phazorFree;
    0.05f  >> randomSlew.in_freq();
                                         drift >> driftAmt.in_mod();        
    phazorFree.out_trig() >> rnd >> randomSlew >> driftAmt;
                                                  driftAmt >> carrierA.in_pitch();
                                                  driftAmt >> carrierB.in_pitch();
                                                  driftAmt >> modulator.in_pitch();

 
    // CONTROLS ---------------------------------------------------------------
    parameters.setName( "fm sub" );
    
    parameters.add( gain.set("gain", -12, -48, 12) );
    parameters.add( basePitch.set("base pitch", 36, 12, 48) );
    parameters.add( carrierAScale.set("A pitch scale", 1.0f, 0.0f, 1.0f) );
    parameters.add( carrierBScale.set("B pitch scale", 1.0f, 0.0f, 1.0f) );
    parameters.add( modulatorScale.set("mod pitch scale", 1.0f, 0.0f, 1.0f) );
    parameters.add( osc_mix.set("carrier mix", 0.0f, 0.0f, 1.0f) );
    parameters.add( detune_ctrl.set("detune", 0.0f, 0.0f, 0.5f) );
    
    parameters.add( ratio_ctrl.set( "ratio coarse", 1, 1, 13) );
    parameters.add( ratio_ctrl.set( "ratio fine", 0.0f, -1.0f, 0.0f) );
    parameters.add( fm_ctrl.set( "fm", 0.0f, 0.0f, 4.0f) );    
    parameters.add( fm_mod.set( "mod env to fm", 0.0f, 0.0f, 6.66f) );
    parameters.add( self_ctrl.set("self fb", 0.0f, 0.0f, 0.25f) );
    parameters.add( self_mod.set("mod env to self", 0.0f, 0.0f, 0.25f) );    
    parameters.add( drift.set("drift", 0.0f, 0.0f, 1.0f) );    
    
    parameters.add( envAttackControl.set("attack",  3, 0, 200 ) );
    parameters.add( envHoldControl.set( "hold",  40, 0, 300 ) );
    parameters.add( envReleaseControl.set("release", 20, 5, 600 ) );
    parameters.add( envVeloControl.set("dynamics", 1.0f, 0.0f, 1.0f ) );    
    
    parameters.add( modEnvAttackControl.set("mod attack",  0, 0, 200 ) );
    parameters.add( modEnvHoldControl.set( "mod hold",  40, 0, 300 ) );
    parameters.add( modEnvReleaseControl.set("mod release", 50, 5, 600 ) );    
    parameters.add( modEnvVeloControl.set("mod dynamics", 0.5f, 0.0f, 1.0f ) );    

    gain.enableSmoothing(50.f);
    self_ctrl.enableSmoothing( 200.0f );
    fm_ctrl.enableSmoothing( 200.0f );
    ratio_ctrl.enableSmoothing( 200.0f );
    
}

float np::synth::FMSub::meter_mod_env() const{
    return ampEnv.meter_output();
}

float np::synth::FMSub::meter_pitch() const{
    return modulator.meter_pitch();
}

pdsp::Patchable& np::synth::FMSub::in_trig(){
    return in("trig");
}

pdsp::Patchable& np::synth::FMSub::in_pitch(){
    return in("pitch");
}


ofParameterGroup & np::synth::FMSub::label (std::string name ){
    parameters.setName( name );
    return parameters;
}
