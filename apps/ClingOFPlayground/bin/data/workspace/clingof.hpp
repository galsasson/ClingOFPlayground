#include "ofMain.h"
#include "cling/Interpreter/Interpreter.h"
#include "ofxInterface.h"

class clingof_t {
public:
	cling::Interpreter *interp;
	ofxInterface::Node scene;
	function<void(void)> update;
	function<void(void)> draw;
	function<void(float*,int,int)> audioOut;
	ofColor bgColor;

	clingof_t() :
	interp(NULL),
	bgColor(0, 255)
	{
		update = [] () {};
		draw = [] () {};
		audioOut = [] (float* output,int bufferSize, int nChannels){};
	}

	void setup() {
		forceLinkWithSymbols();
	}

	void forceLinkWithSymbols() {
		ofDrawRectangle(0, 0, 0, 0);
		ofDrawLine(0, 0, 0, 0);
		ofDrawCircle(0, 0, 0);
		ofDrawEllipse(0, 0, 0, 0);
		ofSetColor(255);
		ofNoFill();
		ofFill();
		ofRandom(1);
		ofRandom(0,1);
		ofDrawSphere(100);
		ofDrawSphere(0, 0, 100);
		ofDrawSphere(0, 0, 0, 100);
		ofEasyCam cam;
		ofColor(0);
		ofColor(0,0);
		ofColor(0,0,0);
		ofColor(0,0,0,0);
		ofLog();
	}

};

extern clingof_t cof;
