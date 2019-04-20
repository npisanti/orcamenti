
#pragma once

#include "ofMain.h"
#include "ofxPDSP.h"

namespace np { namespace synth {
    
class TSampler : public pdsp::Patchable {
public:
    TSampler() { patch(); } 
    TSampler( const TSampler & other ){ patch(); } 
    
    ofParameterGroup    parameters;
    
    pdsp::Patchable & in_trig();
    pdsp::Patchable & in_mod();

    void dBTriggering( bool enable );
    void load(string path);
    
    ofParameterGroup & label( std::string name );
    
    ofParameter<bool> useTuning;
    

    ofParameterGroup sampleGroup;    
    ofParameterGroup tuningGroup;    
    ofParameterGroup combGroup;    

    ofParameterGroup filterGroup;
    ofParameterGroup extModGroup;
        
    ofParameterGroup ampEnvGroup;         
    ofParameterGroup modEnvGroup;
    
    ofParameterGroup mixer;
    
    ofParameter<bool>   loadButton;
    ofParameter<string> samplePath;
    ofParameter<string> sampleName;
    
private:
    void patch();

    void loadButtonCall( bool & value );
    void sampleChangedCall( std::string & value );
    void loadSample( std::string path );

    pdsp::PatchNode     triggers;
    pdsp::Sampler       sampler;
    pdsp::AHR           env;
    pdsp::AHR           fEnv;
    pdsp::Amp           amp;

    pdsp::Panner        panner;
    pdsp::Parameter     panControl;
    
    pdsp::ParameterGain fader; 

    pdsp::TriggeredRandom   drift;
    pdsp::Amp               driftAmt;
    pdsp::Parameter         driftControl;

    pdsp::CombFilter       comb;      
    pdsp::LinearCrossfader combMix;
    pdsp::LFOPhasor        lfoPhasor;
    pdsp::CheapSine        lfoSine;
    pdsp::CheapTri         lfoTri;
    pdsp::LinearCrossfader lfoMix;
    

    pdsp::Parameter        combTuneControl;
    pdsp::Parameter        combMixControl;
    pdsp::Parameter        combFBControl;
    pdsp::Parameter        combDampingControl;
    pdsp::ParameterAmp     combVibrato;
    pdsp::Parameter        combVibratoSpeed;
    pdsp::PitchToFreq      combP2F;
    pdsp::Parameter        combVibratoShape;
    pdsp::ParameterAmp     combEnvMod;
    
    pdsp::ParameterGain revSend;
    pdsp::ParameterGain delSend;
    
    pdsp::PatchNode     modNode;
    pdsp::Parameter     tuningControl;
        
    pdsp::Parameter     startControl;
    pdsp::Parameter     startModControl;

    pdsp::Parameter     attackControl;
    pdsp::Parameter     holdControl;
    pdsp::Parameter     releaseControl;
    pdsp::Parameter     envDynControl;

    pdsp::Parameter     modAttackControl;
    pdsp::Parameter     modHoldControl;
    pdsp::Parameter     modReleaseControl;
    pdsp::Parameter     modEnvDynControl;
    
    pdsp::ParameterAmp  envToFilter;

    pdsp::Decimator     decimate;
    pdsp::PitchToFreq   decimP2F;
    pdsp::Parameter     decimateControl;
    pdsp::ParameterAmp  envToDecimate;
        
    pdsp::VAFilter  filter;
    pdsp::Parameter    cutoffControl;
    pdsp::Parameter    resoControl;
    pdsp::Parameter    modeControl;

    pdsp::SampleBuffer  sample;    

    
    pdsp::ParameterAmp modToPitch;
    pdsp::ParameterAmp modToFilter;
    pdsp::ParameterAmp modToDecimate;
    pdsp::ParameterAmp modToComb;
    
    pdsp::ParameterAmp  duckNext;
    pdsp::Amp           duck;
    pdsp::OneMinusInput invert;
    

    
};   
    
}} // end namespaces 
