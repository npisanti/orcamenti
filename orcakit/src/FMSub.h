
#pragma once

#include "ofMain.h"
#include "ofxPDSP.h"

// wavetable based polysynth

namespace np { namespace synth {

class FMSub : public pdsp::Patchable {

public:

    // synth public API --------------------------------------

    FMSub(){ patch(); }
    FMSub( const FMSub& other ){ patch(); }

    pdsp::Patchable& in_trig();
    pdsp::Patchable& in_pitch();

    
    float meter_mod_env() const;
    float meter_pitch() const;

    ofParameterGroup    parameters;

    ofParameterGroup & label (std::string name );


    void patch();
    // ------- parameters ------------------------------------

    pdsp::PatchNode     voiceTrigger;
    pdsp::PatchNode     pitchNode;
    
    pdsp::FMOperator    carrierA;
    pdsp::FMOperator    carrierB;
    pdsp::FMOperator    modulator;

    pdsp::Amp           fmAmp;
    pdsp::Amp           voiceAmp;

    pdsp::AHR           ampEnv;
    pdsp::Parameter         envVeloControl;
    pdsp::Parameter         envAttackControl;
    pdsp::Parameter         envHoldControl;
    pdsp::Parameter         envReleaseControl;

    pdsp::AHR           modEnv;
    pdsp::Parameter         modEnvVeloControl;
    pdsp::Parameter         modEnvAttackControl;
    pdsp::Parameter         modEnvHoldControl;
    pdsp::Parameter         modEnvReleaseControl;            

    pdsp::Amp               driftAmt;
    pdsp::LFOPhazor         phazorFree;
    pdsp::TriggeredRandom   rnd;
    pdsp::OnePole           randomSlew;

    pdsp::ParameterGain gain;

    pdsp::ParameterAmp  fm_mod;
    pdsp::ParameterAmp  self_mod;

    pdsp::Parameter     fm_ctrl;
    pdsp::Parameter     self_ctrl;
    pdsp::Parameter     ratio_ctrl;
    
    pdsp::Parameter     basePitch;
    pdsp::ParameterAmp  carrierAScale;
    pdsp::ParameterAmp  carrierBScale;
    pdsp::ParameterAmp  modulatorScale;

    pdsp::Parameter     drift;
    
    pdsp::Parameter     detune_ctrl;
    pdsp::Parameter     osc_mix;
    
    pdsp::LinearCrossfader oscXFader;
    
    pdsp::Amp           duck;
    pdsp::OneMinusInput invert;


};

}} // end namspaces 
