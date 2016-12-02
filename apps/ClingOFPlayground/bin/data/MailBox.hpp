#include "ofMain.h"
#include "ofxInterface.h"

using namespace ofxInterface;

class MailBox {
public:

	ofColor bgColor;
	std::function<void(void)> update;
	std::function<void(void)> draw;

	MailBox() {
		bgColor = ofColor(0);
		update = [this]() {};
		draw = [this]() {};
	}

	void setup() {
		dummyFunction();
	}

	void dummyFunction() {
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

extern MailBox box;
extern Node scene;
extern function<void(float*,int,int)> clingAudioOut;