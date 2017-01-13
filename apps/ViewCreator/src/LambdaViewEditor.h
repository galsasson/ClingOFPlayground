//
//  LambdaViewEditor.h
//  ViewCreator
//
//  Created by Gal Sasson on 1/12/17.
//
//

#ifndef LambdaViewEditor_h
#define LambdaViewEditor_h

#include "ofMain.h"
#include "ofxInterfaceTextEditor.h"

using namespace ofxInterface;

class LambdaViewEditor : public Node
{
public:
	LambdaViewEditor(const string& compName);
	string getFunctionCode(const string& func);
	string getCode();

	void update(float dt) override;
	void draw() override;

	ofEvent<LambdaViewEditor> eventCompile;

private:
	LambdaView* view;
	map<string, ofxInterfaceTextEditor*> editors;
	BitmapTextButton updateButton;
	float updateTimer;
	bool autoUpdate;

	void onUpdateClicked(TouchEvent& event);
	Json::Value getEditorConfig();
	void onTouchMove(TouchEvent& event);
};

#endif /* LambdaViewEditor_h */
