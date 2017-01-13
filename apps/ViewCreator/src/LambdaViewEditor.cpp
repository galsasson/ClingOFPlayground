//
//  LambdaViewEditor.cpp
//  ViewCreator
//
//  Created by Gal Sasson on 1/12/17.
//
//

#include "LambdaViewEditor.h"

LambdaViewEditor::LambdaViewEditor(const string& compName)
{
	setName(compName + "_editor");

	// Create editors
	Json::Value conf = getEditorConfig();
	float x=6;
	float y=6;
	vector<string> names;
	names.push_back("update");
	names.push_back("draw");
	names.push_back("touchDown");
	names.push_back("touchMove");
	names.push_back("touchUp");
	for (string& name: names) {
		editors[name] = new ofxInterfaceTextEditor(conf);
		editors[name]->setTitle(name);
		editors[name]->setPosition(x, y);
		addChild(editors[name]);
		y += editors[name]->getHeight()+6;
	}

	// Create update button
	updateButton.setup("    UPDATE    ");
	updateButton.setPosition(x, y);
	addChild(&updateButton);
	ofAddListener(updateButton.eventTouchUp, this, &LambdaViewEditor::onUpdateClicked);

	// Create the lambda view that we'll be changing
	view = new ofxInterface::LambdaView();
	view->setName(compName);
	view->setPosition(2*x+editors.begin()->second->getWidth(), 0);
	addChild(view);

	setSize(2*x+editors.begin()->second->getWidth(), y+30);

	autoUpdate=true;
	updateTimer=2;
	ofAddListener(eventTouchMove, this, &LambdaViewEditor::onTouchMove);
}

string LambdaViewEditor::getFunctionCode(const string& func)
{
	if (editors.find(func) != editors.end()) {
		return editors[func]->getText();
	}
	return "";
}

string LambdaViewEditor::getCode()
{
	stringstream code;
	code << "((ofxInterface::LambdaView*)(" << view << "))->setUpdateFunction([](float dt){"<<getFunctionCode("update")<<"});\n";
	code << "((ofxInterface::LambdaView*)(" << view << "))->setDrawFunction([](){"<<getFunctionCode("draw")<<"});\n";
	code << "((ofxInterface::LambdaView*)(" << view << "))->setTouchDownFunction([](ofxInterface::TouchEvent& event){"<<getFunctionCode("touchDown")<<"});\n";
	code << "((ofxInterface::LambdaView*)(" << view << "))->setTouchMoveFunction([](ofxInterface::TouchEvent& event){"<<getFunctionCode("touchMove")<<"});\n";
	code << "((ofxInterface::LambdaView*)(" << view << "))->setTouchUpFunction([](ofxInterface::TouchEvent& event){"<<getFunctionCode("touchUp")<<"});\n";
	ofLogNotice() << "Code = "<<code.str();
	return code.str();
}

void LambdaViewEditor::update(float dt)
{
	if (autoUpdate) {
		updateTimer-=dt;
		if (updateTimer<=0) {
			ofNotifyEvent(eventCompile, *this, this);
			updateTimer=2;
		}
	}
}

void LambdaViewEditor::draw()
{
	ofSetColor(80);
	ofFill();
	ofDrawRectangle(0, 0, getWidth(), getHeight());
}

void LambdaViewEditor::onUpdateClicked(ofxInterface::TouchEvent &event)
{
	ofNotifyEvent(eventCompile, *this, this);
}

Json::Value LambdaViewEditor::getEditorConfig()
{
	Json::Value conf = Json::objectValue;
	conf["width"] = 40;
	conf["lines"] = 6;
	conf["font"] = "Menlo-Regular.ttf";
	conf["font-size"] = 16;
	conf["draggable"] = false;
	return conf;
}

void LambdaViewEditor::onTouchMove(ofxInterface::TouchEvent &event)
{
	ofVec3f m = event.position-event.prevPosition;
	move(m);
}