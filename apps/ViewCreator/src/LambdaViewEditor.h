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
	
	void draw();

	ofEvent<LambdaViewEditor> eventCompile;

private:
	LambdaView* view;
	map<string, ofxInterfaceTextEditor*> editors;
	BitmapTextButton updateButton;

	void onUpdateClicked(TouchEvent& event);
	Json::Value getEditorConfig();
};

#endif /* LambdaViewEditor_h */
