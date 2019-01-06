
#pragma once

#include "ofMain.h"
#include "ofxPDSP.h"
// wavetable based polysynth

namespace np { namespace synth {

class FMMono : public pdsp::Patchable  {

public:

    FMMono() { patch(); } 
    FMMono( const FMMono & other ) { patch(); } 
    
    void patch();
    
    pdsp::Patchable& in_trig();
    pdsp::Patchable& in_pitch();
    pdsp::Patchable& in_fm();
    pdsp::Patchable& in_self();
    pdsp::Patchable& in_other();
    pdsp::Patchable& out_gain();
    
    float meter_mod_env() const;
    float meter_pitch() const;

    ofParameter<int> ratioSelect;
    ofParameter<float> slew;
    
private:
    pdsp::PatchNode     voiceTrigger;
    pdsp::PatchNode     pitchNode;
    
    pdsp::FMOperator    carrier;
    pdsp::FMOperator    modulator;

    pdsp::Amp           fmAmp;
    pdsp::Amp           voiceAmp;

    pdsp::ADSR          ampEnv;    
    pdsp::ADSR          modEnv;    
            
    pdsp::Amp               driftAmt;
    pdsp::LFOPhazor         phazorFree;
    pdsp::TriggeredRandom   rnd;
    pdsp::OnePole           randomSlew;

    void onRatio( int & value );

    pdsp::OnePole pitchSlew;
    pdsp::ValueControl slew_ctrl;
    void onSlew( float & value );
    
public:
    ofParameterGroup    parameters;

    ofParameterGroup & label (std::string name );

    // ------- parameters ------------------------------------
    pdsp::ParameterGain gain;

    pdsp::ParameterAmp  fm_mod;
    pdsp::ParameterAmp  self_mod;
    pdsp::ParameterAmp  fm_other;
    pdsp::Parameter     fm_ctrl;
    pdsp::Parameter     self_ctrl;
    //pdsp::Parameter     ratio_ctrl;

    ofParameter<float>  attack;
    pdsp::ValueControl     env_attack_ctrl;
    
    ofParameter<float> decay;
    pdsp::ValueControl     env_decay_ctrl;
    
    pdsp::Parameter     env_release_ctrl;

    pdsp::Parameter     drift;
    
    void onAttack( float & value );
    void onDecay( float & value );
    
    pdsp::ValueControl ratio_ctrl;
    
};

}} // end namspaces 
