
#pragma once

#include "ofMain.h"
#include "ofxPDSP.h"

namespace np { namespace synth {
    
class ResoSampler : public pdsp::Patchable {
public:
    ResoSampler() { patch(); } 
    ResoSampler( const ResoSampler & other ){ patch(); } 
    
    ofParameterGroup    parameters;
    
    pdsp::Patchable & in_trig();
    pdsp::Patchable & in_mod();

    void dBTriggering( bool enable );
    void load(string path);
    
    ofParameterGroup & label( std::string name );
    
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
    
    ofParameterGroup combGroup;
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
    
    pdsp::PatchNode     modNode;
    pdsp::Parameter     tuningControl;
        
    pdsp::Parameter     startControl;
    pdsp::Parameter     startModControl;
        
    ofParameterGroup    ampEnvGroup;
    pdsp::Parameter     attackControl;
    pdsp::Parameter     holdControl;
    pdsp::Parameter     releaseControl;
    pdsp::Parameter     envDynControl;
        
    ofParameterGroup    modEnvGroup;
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

    ofParameterGroup filterGroup;

    pdsp::SampleBuffer  sample;    
    ofParameter<bool>   loadButton;
    ofParameter<string> samplePath;
    ofParameter<string> sampleName;
    
    ofParameterGroup extModGroup;
    pdsp::ParameterAmp modToPitch;
    pdsp::ParameterAmp modToFilter;
    pdsp::ParameterAmp modToDecimate;
    pdsp::ParameterAmp modToComb;
    
    pdsp::ParameterAmp  duckNext;
    pdsp::Amp           duck;
    pdsp::OneMinusInput invert;
    
    ofParameterGroup mixer;
    
};   
    
}} // end namespaces 
