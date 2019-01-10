
// FMMono.cpp
// Nicola Pisanti, MIT License, 2018

#include "FMMono.h"

void np::synth::FMMono::patch(){

    parameters.setName( "fm monosynth" );
    
    attack.addListener( this, &np::synth::FMMono::onAttack );
    
    parameters.add( gain.set("gain", -12, -48, 12) );

    parameters.add( attack.set( "attack", 0.0f, 0.0f, 200.0f ) );
    parameters.add( env_release_ctrl.set("env release", 200, 5, 2000));   
    slew.addListener( this, &np::synth::FMMono::onSlew );
    parameters.add( slew.set("pitch slew", 0.0f, 0.0f, 1.0f) ); 
    parameters.add( drift.set("pitch drift", 0.0f, 0.0f, 1.0f) );    

    // ---------------- PATCHING ------------------------
    addModuleInput("trig", voiceTrigger);
    addModuleInput("trig_other", otherEnv.in_trig() );
    addModuleInput("pitch", pitchSlew);
    addModuleInput("decay", decayControl );
    addModuleInput("env_amount", envAmountControl );
    addModuleInput("other", otherControl.in_signal() );
    addModuleInput("other_amount", otherControl.in_mod() );
    addModuleOutput( "gain", gain );
    addModuleOutput( "other", otherAmp );

    // SIGNAL PATH -----------------------------
    modulator >> fmAmp >> carrier.in_fm() >> voiceAmp >> gain;
          otherControl >> carrier.in_fm();
                          carrier >> otherAmp;
                          
    0.2f    >> phazorFree;
    0.05f  >> randomSlew.in_freq();
                                         drift >> driftAmt.in_mod();        
    phazorFree.out_trig() >> rnd >> randomSlew >> driftAmt >> pitchNode;

    
    // MODULATIONS AND CONTROL -----------------

    pitchNode >> carrier.in_pitch();
    pitchNode >> modulator.in_pitch();

    voiceTrigger >> ampEnv >> voiceAmp.in_mod();
    voiceTrigger >> modEnv;

    envAmountControl.enableBoundaries( 0.0f, 9.0f );
    envAmountControl >> fmModScale >> fmModAmount.in_mod();
    envAmountControl >> selfModScale >> selfModAmount.in_mod();
    ratioControl >> modulator.in_ratio();
    ratioControl.enableSmoothing( 100.0f );   
    
    preset( 0 );
    
    modEnv >> fmModAmount   >> fmAmp.in_mod();
    modEnv >> selfModAmount >> carrier.in_fb();

    env_attack_ctrl  >> ampEnv.in_attack();
    decayControl   >> ampEnv.in_decay();
    0.45f >> ampEnv.in_sustain();
    env_release_ctrl >> ampEnv.in_release();

    env_attack_ctrl  >> modEnv.in_attack();
    decayControl   >> modEnv.in_decay();
    0.0f >> modEnv.in_sustain();
    env_release_ctrl >> modEnv.in_release();
    
    env_attack_ctrl  >> otherEnv.in_attack();
    decayControl   >> otherEnv.in_decay();
    0.0f >> otherEnv.in_sustain();
    env_release_ctrl >> otherEnv.in_release();   
    otherEnv >> otherAmp.in_mod(); 

    slew_ctrl.set(20000.0f);
    slew_ctrl >> pitchSlew.in_freq();
    pitchSlew >> pitchNode;
    
    lastTrigger = 0.0f;    
    lastOtherTrigger = 0.0f;

}

void np::synth::FMMono::preset( int index ){
    switch( index ){
        case 0:
            ratioControl.set( 1.0f ); 
            selfModScale.set( 0.25f / 9.0f );
            fmModScale.set( 0.0f );
        break;
        
        case 1:
            ratioControl.set( 0.25f ); 
            selfModScale.set( 0.0f );
            fmModScale.set( 4.0f / 9.0f );
        break;
        
        case 2:
            ratioControl.set( 0.5f ); 
            selfModScale.set( 0.0f );
            fmModScale.set( 4.0f / 9.0f );
        break;

        case 3:
            ratioControl.set( 1.0f ); 
            selfModScale.set( 0.0f );
            fmModScale.set( 4.0f / 9.0f );
        break;
        
        case 4:
            ratioControl.set( 1.5f ); 
            selfModScale.set( 0.0f );
            fmModScale.set( 4.0f / 9.0f );
        break;
        
        case 5:
            ratioControl.set( 2.0f ); 
            selfModScale.set( 0.0f );
            fmModScale.set( 4.0f / 9.0f );
        break;
        
        case 6:
            ratioControl.set( 3.0f ); 
            selfModScale.set( 0.0f );
            fmModScale.set( 4.0f / 9.0f );
        break;
        
        case 7:
            ratioControl.set( 4.0f ); 
            selfModScale.set( 0.0f );
            fmModScale.set( 4.0f / 9.0f );
        break;
        
        case 8:
            ratioControl.set( 5.0f ); 
            selfModScale.set( 0.0f );
            fmModScale.set( 4.0f / 9.0f );
        break;
        
        case 9:
            ratioControl.set( 8.0f ); 
            selfModScale.set( 0.0f );
            fmModScale.set( 4.0f / 9.0f );
        break;
        
        case 10:
            ratioControl.set( 11.0f ); 
            selfModScale.set( 0.0f );
            fmModScale.set( 4.0f / 9.0f );
        break;
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
    env_attack_ctrl.set( value );
}

float np::synth::FMMono::meter_mod_env() const{
    return modEnv.meter_output();
}

float np::synth::FMMono::meter_pitch() const{
    return carrier.meter_pitch();
}

ofParameterGroup & np::synth::FMMono::label (std::string name ){
    parameters.setName( name );
    return parameters;
}
