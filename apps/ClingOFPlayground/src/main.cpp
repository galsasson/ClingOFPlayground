#include "ofMain.h"
#include "ofApp.h"

using namespace std;

struct hostapp_t {
	ofApp* app;
} hostapp;

//========================================================================
int main(int argc, char** argv){
	ofSetupOpenGL(670,790,OF_WINDOW);			// <-------- setup the GL context

//	hostapp.app = new ofApp();
//	hostapp.app->setupCling();

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
