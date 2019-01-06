
// FMMono.cpp
// Nicola Pisanti, MIT License, 2018

#include "FMMono.h"

void np::synth::FMMono::patch(){

    parameters.setName( "fm monosynth" );
    
    attack.addListener( this, &np::synth::FMMono::onAttack );
    decay.addListener( this, &np::synth::FMMono::onDecay );
    
    parameters.add( gain.set("gain", -12, -48, 12) );
    parameters.add( fm_ctrl.set( "fm", 0.0f, 0.0f, 4.0f) );    
    parameters.add( fm_mod.set( "fm mod", 0.0f, 0.0f, 4.0f) );
    parameters.add( self_ctrl.set("self fb", 0.0f, 0.0f, 1.0f) );
    parameters.add( self_mod.set("self fb mod", 0.2f, 0.0f, 0.5f) );    
    ratioSelect.addListener( this, &np::synth::FMMono::onRatio );
    parameters.add( ratioSelect.set("ratio select", 2, 0, 9 ) );
    parameters.add( fm_other.set( "fm from other", 0.0f, 0.0f, 4.0f) );
    parameters.add( attack.set( "attack", 0.0f, 0.0f, 1.0f ) );
    parameters.add( decay.set( "decay", 0.9f, 0.0f, 1.0f ) );
    parameters.add( env_release_ctrl.set("env release", 200, 5, 2000));   
    slew.addListener( this, &np::synth::FMMono::onSlew );
    parameters.add( slew.set("pitch slew", 0.0f, 0.0f, 1.0f) ); 
    parameters.add( drift.set("pitch drift", 0.0f, 0.0f, 1.0f) );    

    // ---------------- PATCHING ------------------------
    addModuleInput("trig", voiceTrigger);
    addModuleInput("pitch", pitchSlew);
    addModuleInput("fm", fmAmp.in_mod());
    addModuleInput("other", fm_other.in_signal());
    addModuleInput( "self", carrier.in_fb() );
    addModuleOutput( "signal", voiceAmp );
    addModuleOutput( "gain", gain );

    pitchNode >> carrier.in_pitch();
    pitchNode >> modulator.in_pitch();
    ratio_ctrl >> modulator.in_ratio();
    fm_ctrl >> fmAmp.in_mod();
    self_ctrl >> carrier.in_fb();
    
    slew_ctrl.set(20000.0f);
    slew_ctrl >> pitchSlew.in_freq();
    pitchSlew >> pitchNode;
    
    // SIGNAL PATH
    modulator >> fmAmp >> carrier.in_fm() >> voiceAmp >> gain;
              fm_other >> carrier.in_fm();
    
    // MODULATIONS AND CONTROL
    modEnv >> fm_mod   >> fmAmp.in_mod();
    modEnv >> self_mod >> carrier.in_fb();
    
    voiceTrigger >> ampEnv >> voiceAmp.in_mod();
    voiceTrigger >> modEnv;

    env_attack_ctrl  >> ampEnv.in_attack();
    env_decay_ctrl   >> ampEnv.in_decay();
    0.3f >> ampEnv.in_sustain();
    env_release_ctrl >> ampEnv.in_release();

    env_attack_ctrl  >> modEnv.in_attack();
    env_decay_ctrl   >> modEnv.in_decay();
    0.0f >> modEnv.in_sustain();
    env_release_ctrl >> modEnv.in_release();
    
    0.2f    >> phazorFree;
    0.05f  >> randomSlew.in_freq();
                                         drift >> driftAmt.in_mod();        
    phazorFree.out_trig() >> rnd >> randomSlew >> driftAmt >> pitchNode;

    ratio_ctrl.enableSmoothing( 100.0f );   
    fm_mod.enableSmoothing( 100.0f );
    fm_other.enableSmoothing( 100.0f );
    self_mod.enableSmoothing( 100.0f );
}

void np::synth::FMMono::onRatio( int & value ){
    switch( ratioSelect ){
        case 0: ratio_ctrl.set( 0.25f ); break;
        case 1: ratio_ctrl.set( 0.5f ); break;
        case 2: ratio_ctrl.set( 1.0f ); break;
        case 3: ratio_ctrl.set( 1.5f ); break;
        case 4: ratio_ctrl.set( 2.0f ); break;
        case 5: ratio_ctrl.set( 3.0f ); break;
        case 6: ratio_ctrl.set( 4.0f ); break;
        case 7: ratio_ctrl.set( 5.0f ); break;
        case 8: ratio_ctrl.set( 7.0f ); break;
        case 9: ratio_ctrl.set( 9.0f ); break;
    }
}

void np::synth::FMMono::onSlew( float & value ){
    if( slew> 0.0f){
        float control =  (1.0f-value);
        control *= control;
        control = control * 14.0f  + 1.0f;
        slew_ctrl.set( control );
        //std::cout<<"slew is "<< control <<"hz\n";
    }else{
        slew_ctrl.set( 20000.0f );
    }
}

void np::synth::FMMono::onAttack( float & value ){
    env_attack_ctrl.set( value * value * value * 1500.0f );
}

void np::synth::FMMono::onDecay( float & value ){
    env_decay_ctrl.set( value * value * 4000.0f );
}

float np::synth::FMMono::meter_mod_env() const{
    return modEnv.meter_output();
}

float np::synth::FMMono::meter_pitch() const{
    return carrier.meter_pitch();
}

pdsp::Patchable& np::synth::FMMono::in_trig(){
    return in("trig");
}

pdsp::Patchable& np::synth::FMMono::in_pitch(){
    return in("pitch");
}
pdsp::Patchable& np::synth::FMMono::in_fm(){
    return in("fm");
}

pdsp::Patchable& np::synth::FMMono::in_other(){
    return in("other");
}

pdsp::Patchable& np::synth::FMMono::in_self(){
    return in("self");
}

pdsp::Patchable& np::synth::FMMono::out_gain(){
    return out("gain");
}

ofParameterGroup & np::synth::FMMono::label (std::string name ){
    parameters.setName( name );
    return parameters;
}
