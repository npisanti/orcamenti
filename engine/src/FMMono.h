
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
    
    float meter_env() const;
    float meter_pitch() const;
    
    ofParameterGroup    parameters;
        pdsp::ParameterGain gain;
        ofParameter<float>  attack;
        pdsp::Parameter     drift;
    
    ofParameterGroup & label (std::string name );
    
    float lastTrigger;
    float lastOtherTrigger;
    
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
    
    pdsp::Amp fmModAmount;
    pdsp::Amp selfModAmount;
    
    pdsp::Amp  otherControl;

    pdsp::ValueControl  env_attack_ctrl;
    pdsp::PatchNode     decayControl;
    pdsp::Parameter     env_release_ctrl;
    
    void onAttack( float & value );
};

}} // end namspaces 
