
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
    
    float meter_mod_env() const;
    float meter_pitch() const;

    ofParameter<float> slew;
    
private:
    pdsp::PatchNode     voiceTrigger;
    pdsp::PatchNode     pitchNode;
    
    pdsp::FMOperator    carrier;
    pdsp::FMOperator    modulator;

    pdsp::Amp           fmAmp;
    pdsp::Amp           voiceAmp;

    pdsp::ADSR          otherEnv;    
    pdsp::Amp           otherAmp;

    pdsp::ADSR          ampEnv;    
    pdsp::ADSR          modEnv;    
            
    pdsp::Amp               driftAmt;
    pdsp::LFOPhazor         phazorFree;
    pdsp::TriggeredRandom   rnd;
    pdsp::OnePole           randomSlew;
    
    pdsp::PatchNode envAmountControl;
    pdsp::Amp fmModScale;
    pdsp::Amp fmModAmount;
    pdsp::Amp selfModScale;
    pdsp::Amp selfModAmount;
    pdsp::OnePole pitchSlew;
    pdsp::ValueControl ratioControl;
    
    void preset( int index );

    pdsp::ValueControl slew_ctrl;
    void onSlew( float & value );
    
public:
    ofParameterGroup    parameters;

    ofParameterGroup & label (std::string name );

    // ------- parameters ------------------------------------
    pdsp::ParameterGain gain;

    pdsp::Amp  otherControl;

    ofParameter<float>  attack;
    pdsp::ValueControl     env_attack_ctrl;

    pdsp::PatchNode     decayControl;
    
    pdsp::Parameter     env_release_ctrl;

    pdsp::Parameter     drift;
    
    void onAttack( float & value );
    
    float lastTrigger;
    float lastOtherTrigger;
    
};

}} // end namspaces 
