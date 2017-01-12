//
//  LambdaViewEditor.cpp
//  ViewCreator
//
//  Created by Gal Sasson on 1/12/17.
//
//

#include "LambdaViewEditor.h"

LambdaViewEditor::LambdaViewEditor()
{
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
	updateButton.setup("    UPDATE    ");
	updateButton.setPosition(x, y);
	ofAddListener(updateButton.eventTouchUp, this, &LambdaViewEditor::onUpdateClicked);
	setSize(2*x+editors.begin()->second->getWidth(), y+30);
}

string LambdaViewEditor::getCode(const string& func)
{
	if (editors.find(func) != editors.end()) {
		return editors[func]->getText();
	}
	return "";
}

void LambdaViewEditor::draw()
{
	ofSetColor(40);
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