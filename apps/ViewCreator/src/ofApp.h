#pragma once

#include "ofMain.h"
#include "ofxInterfaceTextEditor.h"
#include "LambdaViewEditor.h"

class ofApp: public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void lateDraw(ofEventArgs& args);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void mouseScrolled(int x, int y, float h, float v);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
	void exit();

	void setupSound();

	BitmapTextButton newBtn;
	BitmapTextButton loadBtn;
	BitmapTextButton saveBtn;
	BitmapTextButton saveAsBtn;
	ofxInterfaceTextEditor editor;
	LambdaViewEditor* lambdaEditor;
	LambdaView execToggle;
	bool terminalMode;

	string lastFile;
	void onEnterHit(ofxInterfaceTextEditor::EventArgs& args);

	void onUpdateView(LambdaViewEditor& edit);

	void onNew(TouchEvent& event);
	void onLoad(TouchEvent& event);
	void onSave(TouchEvent& event);
	void onSaveAs(TouchEvent& event);
	void audioOut(ofSoundBuffer& buffer);
};
