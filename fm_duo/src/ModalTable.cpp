
// ModalTable.cpp
// Nicola Pisanti, GPLv3 License, 2016

#include "ModalTable.h"

ofParameterGroup & np::tuning::ModalTable::setup( int grades, std::string name ) {
    
    this->grades = grades;
    ratios.resize( grades );
    pitches.resize( grades );
    
    parameters.setName( name );
    parameters.add( masterPitchControl.set( "master pitch",  36,   0,    60  ) );

    int index = 0;

    for( RatioUI & r : ratios ){
        r.setBasePitch( masterPitchControl );  
        std::string label = "d";
        label += ofToString(index+1);
        label += " numerator";
        parameters.add( r.numerator.set( label, grades + index, 1, 23 ) );
        label = "d";
        label += ofToString(index+1);
        label += " denominator";
        parameters.add( r.denominator.set( label, grades, 1, 23 ) );
        
        r.numerator.addListener( this, &ModalTable::updateAll );
        r.denominator.addListener( this, &ModalTable::updateAll);

        index++;
    }
    
    masterPitchControl.addListener( this, &ModalTable::updateAll);
    
    int dummy = 0;
    updateAll( dummy );
    
    return parameters;
}

ofParameterGroup & np::tuning::ModalTable::label( std::string name ){
    parameters.setName(name);
    return parameters;
}

void np::tuning::ModalTable::updateAll( int & value ) {
    int dummy = 0;
    for( RatioUI & r : ratios ){
        r.ratioChange( dummy );
    } 
    for( size_t i=0; i<pitches.size(); ++i){
        pitches[i] = ratios[i].pitch;
    }
}
