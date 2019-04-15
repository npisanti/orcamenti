
#pragma once

#include "ofMain.h"
#include "ofxPDSP.h"

namespace np2 { namespace effect {
    
class HQSaturator : public pdsp::Patchable {

public:    
    HQSaturator() { patch(); }
    HQSaturator( const HQSaturator & other ) { patch (); }
    
    ofParameterGroup parameters;

    ofParameterGroup & label( std::string name );

private:    
    void patch();
    
    pdsp::ParameterGain    inputGain;
    pdsp::ParameterGain    outputGain;

    pdsp::IIRUpSampler2x upsampler;
    pdsp::IIRDownSampler2x downsampler; 
    pdsp::Saturator2 saturator;
};
    
}}
