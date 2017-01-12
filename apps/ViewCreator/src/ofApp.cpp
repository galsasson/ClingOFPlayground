#include "ofApp.h"
#include "../bin/data/workspace/clingof.hpp"
#include "ofxCling.h"

#define OSX_SDK_PATH "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk"
#define CLING_PATH "../../../../../../external_addons/ofxCling"
#define OF_PATH "../../../../../../openFrameworks"
#define ADDONS_PATH "../../../../../../external_addons"

//--------------------------------------------------------------

clingof_t cof;

void ofApp::setup(){
	lastFile = "";
	terminalMode = false;

	ofAddListener(ofEvents().draw, this, &ofApp::lateDraw, 1000);

	ofSetEscapeQuitsApp(false);
	ofSetFrameRate(60);
	ofEnableAlphaBlending();

	// setup sound
	setupSound();

	// call some functions to force linkage with these symbols
	ofxCling::forceLinkWithSymbols();
	// setup cling interpreter
	cof.interp = ofxCling::createInterpreter(OF_PATH, CLING_PATH, OSX_SDK_PATH);

	// add some more include paths
	cof.interp->AddIncludePath("../../../data");
	cof.interp->AddIncludePath(ADDONS_PATH"/ofxInterface/src/components");
	cof.interp->AddIncludePath(ADDONS_PATH"/ofxInterface/src");
	cof.interp->AddIncludePath(ADDONS_PATH"/ofxNanoVG/src");
	cof.interp->AddIncludePath(ADDONS_PATH"/ofxNanoVG/libs/nanovg/src");
	cof.interp->AddIncludePath(ADDONS_PATH"/ofxNanoVG/libs/nanosvg/src");
	cof.interp->AddIncludePath(ADDONS_PATH"/ofxInterfaceEditor/src");
	cof.interp->AddIncludePath(ADDONS_PATH"/ofxJSON/src");
	cof.interp->AddIncludePath(ADDONS_PATH"/ofxJSON/libs/jsoncpp/include");

	cof.interp->DumpIncludePath();

	// create lambda editor
	lambdaEditor.setPosition(4, 4);

	// create general code editor
	editor.setTitle("Playground");
	editor.setPosition(4, lambdaEditor.getY()+lambdaView.getHeight()+6);
	ofAddListener(editor.eventEnterDown, this, &ofApp::onEnterHit);

	// default lambda editor
	lambdaView.setSize(100, 100);
	lambdaView.setDrawFunction([this]() {
		ofSetColor(255, 0, 0);
		ofFill();
		ofDrawRectangle(0, 0, lambdaView.getWidth(), lambdaView.getHeight());
	});
	lambdaView.setPosition(lambdaEditor.getX() + lambdaEditor.getWidth() + 10, 10);

	// create load/save buttons
	newBtn.setup("  NEW  ");
	loadBtn.setup("  OPEN  ");
	saveBtn.setup("  SAVE  ");
	saveAsBtn.setup(" SAVE AS ");
	newBtn.setPosition(10,	340);
	loadBtn.setPosition(newBtn.getX()+newBtn.getWidth()+4, newBtn.getY());
	saveBtn.setPosition(loadBtn.getX()+loadBtn.getWidth()+4, loadBtn.getY());
	saveAsBtn.setPosition(saveBtn.getX()+saveBtn.getWidth()+4, saveBtn.getY());
	ofAddListener(newBtn.eventTouchDown, this, &ofApp::onNew);
	ofAddListener(loadBtn.eventTouchDown, this, &ofApp::onLoad);
	ofAddListener(saveBtn.eventTouchDown, this, &ofApp::onSave);
	ofAddListener(saveAsBtn.eventTouchDown, this, &ofApp::onSaveAs);
	execToggle.setName("  TERMINAL MODE  ");
	execToggle.setDrawFunction([this](){
		execToggle.setSize(execToggle.getName().length()*8 + 10, 20);
		ofSetColor(terminalMode?255:80);
		ofFill();
		ofDrawRectangle(0, 0, execToggle.getWidth(), execToggle.getHeight());
		ofSetColor(0);
		ofDrawBitmapString(execToggle.getName(), 5, execToggle.getHeight()-5);
	});
	execToggle.setTouchDownFunction([this](TouchEvent& event) {
		terminalMode = !terminalMode;
	});

	cof.scene.setSize(ofGetWidth(), ofGetHeight());
	cof.scene.addChild(&lambdaEditor);
	cof.scene.addChild(&editor);
	cof.scene.addChild(&newBtn);
	cof.scene.addChild(&loadBtn);
	cof.scene.addChild(&saveBtn);
	cof.scene.addChild(&saveAsBtn);
	cof.scene.addChild(&execToggle);
	cof.scene.addChild(&lambdaView);
	TouchManager::one().setup(&cof.scene);
}

void ofApp::setupSound()
{
	cof.soundStream.printDeviceList();

	ofSoundStreamSettings settings;
#ifdef USE_SOUNDFLOWER
	auto devices = cof.soundStream.getMatchingDevices("Soundflower (2ch)");
	settings.setOutDevice(devices[0]);
#endif
	settings.setOutListener(this);
	settings.sampleRate = 44100;
	settings.numOutputChannels = 2;
	settings.numInputChannels = 0;
	settings.bufferSize = 512;
	cof.soundStream.setup(settings);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::onEnterHit(ofxInterfaceTextEditor::EventArgs &args)
{
	if (ofGetKeyPressed(OF_KEY_SHIFT)) {
		string code = args.editor->getSelectedText();
		try {
			cof.lastCompilationResult = cof.interp->process(code);
			args.continueNormalBehavior=false;
			args.editor->flashSelectedText(0.5);
		}
		catch(exception& e) {
			ofLogError("ofApp") << "Exception when processing code: "<<code<<"\nwhat?\n"<<e.what();
		}
	}
	else {
		if (terminalMode) {
			string line = args.editor->getLine(args.editor->getCaret().line);
			if (line != "") {
				try {
					if (line[0]=='.') {
						if (line[1]=='L') {
							cof.interp->loadFile(line.substr(3));
						}
						else if (line[1]=='I') {
							cof.interp->AddIncludePath(line.substr(3));
						}
						else if (line[1]=='l' && line[2]=='s') {
							cout<<ofSystem(line.substr(1).c_str())<<endl;
						}
						else if (line.substr(1, 6)=="unload") {
							int times = atoi(line.substr(8).c_str());
							cof.interp->unload(times);
						}
					}
					else {
						cof.lastCompilationResult = cof.interp->process(line);
					}
				}
				catch(exception& e) {
					ofLogError("ofApp") << "Exception when processing line: "<<line<<"\nwhat?\n"<<e.what();
				}
			}

			// if enter was not pressed at the end of line don't break the line
			if (args.editor->getCaret().chr < line.size()) {
				args.continueNormalBehavior=false;
				args.editor->keyPressed(OF_KEY_DOWN);
			}
		}
	}
}

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

void ofApp::onNew(ofxInterface::TouchEvent &event)
{
//	editor.setText("");
//	editor.setTitle("Cling OF Playground");
	lastFile="";
}

void ofApp::onLoad(ofxInterface::TouchEvent &event)
{
	ofFileDialogResult result = ofSystemLoadDialog("Open File", false, ofToDataPath("workspace"));
	if (result.bSuccess) {
//		editor.loadFromFile(result.filePath);
		lastFile = result.filePath;
//		editor.setTitle("Cling OF Playground ( "+result.fileName+" )");
	}
}

void ofApp::onSave(ofxInterface::TouchEvent &event)
{
	if (lastFile != "") {
//		editor.saveToFile(lastFile);
	}
	else {
		ofFileDialogResult result = ofSystemSaveDialog("Save to file...", "Save text to file");
		if (result.bSuccess) {
//			editor.saveToFile(result.filePath);
			lastFile = result.filePath;
//			editor.setTitle("Cling OF Playground ( "+result.fileName+" )");
		}
	}
}

void ofApp::onSaveAs(ofxInterface::TouchEvent &event)
{
	ofFileDialogResult result = ofSystemSaveDialog("Save to file...", "Save text to file");
	if (result.bSuccess) {
//		editor.saveToFile(result.filePath);
		lastFile = result.filePath;
//		editor.setTitle("Cling OF Playground ( "+result.fileName+" )");
	}
}

//--------------------------------------------------------------
void ofApp::update(){
	newBtn.setPosition(editor.getX(), editor.getY()-newBtn.getHeight()-4);
	loadBtn.setPosition(newBtn.getX()+newBtn.getWidth()+4, newBtn.getY());
	saveBtn.setPosition(loadBtn.getX()+loadBtn.getWidth()+4, loadBtn.getY());
	saveAsBtn.setPosition(saveBtn.getX()+saveBtn.getWidth()+4, saveBtn.getY());
	execToggle.setPosition(editor.getX()+editor.getWidth()-execToggle.getWidth(), newBtn.getY());

	cof.update();

	cof.scene.updateSubtreePostOrder(1.0f/60);
}

//--------------------------------------------------------------
void ofApp::draw(){
}

void ofApp::lateDraw(ofEventArgs &args)
{
	cof.draw();
	cof.scene.render();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (ofxInterfaceTextEditor::getFocused() != NULL) {
		ofxInterfaceTextEditor::getFocused()->keyPressed(key);
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if (ofxInterfaceTextEditor::getFocused() != NULL) {
		ofxInterfaceTextEditor::getFocused()->keyReleased(key);
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	TouchManager::one().touchMove(button, ofVec2f(x,y));
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	TouchManager::one().touchDown(button, ofVec2f(x,y));
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	TouchManager::one().touchUp(button, ofVec2f(x,y));
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float h, float v){
	ofxInterfaceTextEditor::vscroll(x, y, v);
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::exit(){
	delete cof.interp;
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer& buffer)
{
	cof.audioOut(buffer);
}
