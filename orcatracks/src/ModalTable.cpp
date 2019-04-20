
// ModalTable.cpp
// Nicola Pisanti, GPLv3 License, 2016

#include "ModalTable.h"

ofParameterGroup & np::tuning::ModalTable::setup( int degrees, std::string name ) {
    
    this->degrees = degrees;
    ratios.resize( degrees );
    pitches.resize( degrees );
    
    parameters.setName( name );
    parameters.add( masterPitchControl.set( "master pitch",  36,   0,    60  ) );

    int index = 0;

    for( RatioUI & r : ratios ){
        r.setBasePitch( masterPitchControl );  
        
        std::string label = "d";
        label += ofToString(index);
        label += " enable";        
        parameters.add( r.enable.set( label, true ) );
        
        label = "d";
        label += ofToString(index);
        label += " numerator";
        parameters.add( r.numerator.set( label, degrees + index, 1, 63 ) );
        label = "d";
        label += ofToString(index);
        label += " denominator";
        parameters.add( r.denominator.set( label, degrees, 1, 32 ) );
        
        r.numerator.addListener( this, &ModalTable::callUpdateInt );
        r.denominator.addListener( this, &ModalTable::callUpdateInt);
        r.enable.addListener( this, &ModalTable::callUpdateBool);

        index++;
    }
    
    masterPitchControl.addListener( this, &ModalTable::callUpdateInt);
    
    updateAll();
    
    return parameters;
}

ofParameterGroup & np::tuning::ModalTable::label( std::string name ){
    parameters.setName(name);
    return parameters;
}

void np::tuning::ModalTable::updateAll() {
    
    int dummy = 0;
    
    degrees = 0;
    
    for( RatioUI & r : ratios ){
        r.ratioChange( dummy );
        if( r.enable ){ degrees++; }
    } 
    
    pitches.resize( degrees );
    
    int pi = 0;
    for( size_t i=0; i<ratios.size(); ++i){
        if( ratios[i].enable ){
            pitches[pi] = ratios[i].pitch;
            pi++;
        }
    }
}
