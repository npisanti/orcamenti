#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){

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

	ofSetupOpenGL( 240, 1040, OF_WINDOW );
	ofRunApp(new ofApp());
}
