#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( int argc, char *argv[] ){

/*    
#ifdef __ARM_ARCH
    ofGLESWindowSettings settings;
    settings.glesVersion = 2;
#else
    ofGLFWWindowSettings settings;
    settings.resizable = true;
#endif

    settings.setSize( 240, 1040 );    
    settings.setPosition(ofVec2f( 1920-240, 0));    
    
    shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);

    shared_ptr<ofApp> mainApp(new ofApp);
    
    ofRunApp(mainWindow, mainApp);
    ofRunMainLoop();
*/

    auto * app = new ofApp();
    
    app->config = "settings.xml";
    
    if(argc>1){		
        for( int i=1; i<argc; ++i ){
            app->config = std::string( argv[i] );
        }
    }

	ofSetupOpenGL( 20 + 220*8, 920, OF_WINDOW );
	ofRunApp( app );
}
