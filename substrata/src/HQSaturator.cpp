

#include "HQSaturator.h"

void np2::effect::HQSaturator::patch(){

    addModuleInput( "signal", inputGain );
    addModuleOutput( "signal", outputGain );
    
    saturator.setOversampleLevel(2);
    
    inputGain >> upsampler >> saturator >> downsampler >> outputGain;
    
    parameters.setName( "saturator" );
    parameters.add( inputGain.set("input dB", 0, -48, 48) );
    parameters.add( outputGain.set("output dB", 0, -48, 0) );
}

ofParameterGroup & np2::effect::HQSaturator::label( std::string name ){
    parameters.setName(name);
    return parameters;
}

