
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
    pdsp::Patchable & in_pitch();

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

    
    pdsp::ParameterGain fader; 

    pdsp::TriggeredRandom   drift;
    pdsp::Amp               driftAmt;
    pdsp::Parameter         driftControl;
    
    pdsp::PatchNode     pitchNode;
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
    ofParameter<bool>   loadButton;
    ofParameter<string> samplePath;
    ofParameter<string> sampleName;
    
};   
    
}} // end namespaces 
