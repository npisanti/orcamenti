
#pragma once

#include "ofMain.h"
#include "ofxPDSP.h"

// pure tuning using integer ratios

namespace np { namespace tuning {

class ModalTable {

private:

    // private internal classes ------------------------------------------------
    struct RatioUI {

        RatioUI(){
            numerator.addListener(this, &RatioUI::ratioChange);
            denominator.addListener(this, &RatioUI::ratioChange);
        }

        void ratioChange( int & value ) {
            double ratio = double(numerator) / double (denominator);
            double bp = (double) (*basePitch);
            double freq = pdsp::p2f(bp);
            freq *= ratio;
            pitch = pdsp::f2p(freq);
        }
        
        void setBasePitch( ofParameter<int> & bp) {
            basePitch = &bp;
        }
        
        ofParameter<int>*   basePitch;
        ofParameter<int>    numerator;
        ofParameter<int>    denominator;
        
        float pitch;
    };

public: // ------------------- PUBLIC API --------------------------------------

    ModalTable(){}
    ModalTable( const ModalTable & other ) { }
    
    ofParameterGroup & setup( int grades, std::string name="integer ratio mode" );
    ofParameterGroup & label( std::string name );  
    
    ofParameterGroup parameters;    

    std::vector<float> pitches;
    int grades;
    
private: // --------------------------------------------------------------------

    void updateAll( int & value );
    
    ofParameter<int>    masterPitchControl;
    vector<RatioUI>     ratios;

    
};

}} // end namespaces
