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
};

extern clingof_t cof;
