#pragma once

#include "ofMain.h"
#include "cling/Interpreter/Interpreter.h"
#include "ofxInterface.h"

class clingof_t {
public:
	ofxInterface::Node scene;
	function<void(void)> update;
	function<void(void)> draw;
	function<void(ofSoundBuffer& buffer)> audioOut;
	ofSoundStream soundStream;

	cling::Interpreter *interp;
	cling::Interpreter::CompilationResult lastCompilationResult;

	clingof_t() :
	interp(NULL),
	lastCompilationResult(cling::Interpreter::kSuccess)
	{
		update = [] () {};
		draw = [] () {};
		audioOut = [] (ofSoundBuffer& buffer){};
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
		ofVideoPlayer player;
		ofAVFoundationPlayer avplayer;
		ofLog();
	}

};

extern clingof_t cof;
