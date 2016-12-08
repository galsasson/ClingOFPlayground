#include "ofApp.h"
#include "../bin/data/workspace/clingof.hpp"

#ifdef DEBUG
  #define CLING_PATH "../../../../../../../llvm/cling-build-debug/install"
#else
  #define CLING_PATH "../../../../../../../llvm/cling-build-release/install"
#endif
#define OF_PATH "../../../../../../openFrameworks"
#define HOST_PATH "../../../../../../apps/ClingOFPlayground/src"
#define ADDONS_PATH "../../../../../../external_addons"
 //--------------------------------------------------------------

clingof_t cof;

void ofApp::setup(){

	lastFile = "";
	ternimalMode = false;

	ofSetEscapeQuitsApp(false);
	ofSetFrameRate(60);
	ofEnableAlphaBlending();

	// set main mailbox object between cling and OF
	cof.setup();

	// setup sound
	ofSoundStreamSetup(1, 0);

	// setup cling interpreter
	setupCling();

	// create editor
	newBtn.setup("  NEW  ");
	loadBtn.setup("  OPEN  ");
	saveBtn.setup("  SAVE  ");
	saveAsBtn.setup(" SAVE AS ");
	newBtn.setPosition(10,	340);
	loadBtn.setPosition(newBtn.getX()+newBtn.getWidth()+2, newBtn.getY());
	saveBtn.setPosition(loadBtn.getX()+loadBtn.getWidth()+2, loadBtn.getY());
	saveAsBtn.setPosition(saveBtn.getX()+saveBtn.getWidth()+2, saveBtn.getY());
	ofAddListener(newBtn.eventTouchDown, this, &ofApp::onNew);
	ofAddListener(loadBtn.eventTouchDown, this, &ofApp::onLoad);
	ofAddListener(saveBtn.eventTouchDown, this, &ofApp::onSave);
	ofAddListener(saveAsBtn.eventTouchDown, this, &ofApp::onSaveAs);
	execToggle.setName("  TERMINAL MODE  ");
	execToggle.setDrawFunction([this](){
		execToggle.setSize(execToggle.getName().length()*8 + 10, 20);
		ofSetColor(ternimalMode?255:40);
		ofFill();
		ofDrawRectangle(0, 0, execToggle.getWidth(), execToggle.getHeight());
		ofSetColor(0);
		ofDrawBitmapString(execToggle.getName(), 5, execToggle.getHeight()-5);
	});
	execToggle.setTouchDownFunction([this](TouchEvent& event) {
		ternimalMode = !ternimalMode;
	});

	Json::Value config;
	config["title-text"] = "Cling OF Playground";
	config["font-size"] = 20;
	config["background-color"] = "#111111 100%";
	config["width"] = 70;
	editor.setConfig(config);
	editor.setPosition(10, saveBtn.getY()+saveBtn.getHeight()+20);
	ofAddListener(editor.eventEnterDown, this, &ofApp::onEnterHit);
	ofAddListener(editor.eventTabDown, this, &ofApp::onTabHit);

	cof.scene.setSize(ofGetWidth(), ofGetHeight());
	cof.scene.addChild(&editor);
	cof.scene.addChild(&newBtn);
	cof.scene.addChild(&loadBtn);
	cof.scene.addChild(&saveBtn);
	cof.scene.addChild(&saveAsBtn);
	cof.scene.addChild(&execToggle);
	TouchManager::one().setup(&cof.scene);

	setEditorVisible(true);
}

void ofApp::setupCling()
{
	int argc=1;
	char* argv[10];
	argv[0] = (char*)malloc(1024);
	sprintf(argv[0], "ClingOFPlayground");

	const char* LLVMRESDIR = CLING_PATH; //path to llvm resource directory

	cof.interp = new cling::Interpreter(argc, argv, LLVMRESDIR);

	cof.interp->DumpIncludePath();

	vector<string> includePaths;
	includePaths.push_back("libs/openFrameworks");
	includePaths.push_back("libs/openFrameworks/3d");
	includePaths.push_back("libs/openFrameworks/communication");
	includePaths.push_back("libs/openFrameworks/gl");
	includePaths.push_back("libs/openFrameworks/math");
	includePaths.push_back("libs/openFrameworks/sound");
	includePaths.push_back("libs/openFrameworks/utils");
	includePaths.push_back("libs/openFrameworks/app");
	includePaths.push_back("libs/openFrameworks/events");
	includePaths.push_back("libs/openFrameworks/graphics");
	includePaths.push_back("libs/openFrameworks/types");
	includePaths.push_back("libs/openFrameworks/video");
	includePaths.push_back("libs/FreeImage/include");
	includePaths.push_back("libs/freetype/include");
	includePaths.push_back("libs/rtaudio/include");
	includePaths.push_back("libs/boost/include");
	includePaths.push_back("libs/tess2/include");
	includePaths.push_back("libs/cairo/include");
	includePaths.push_back("libs/cairo/include/cairo");
	includePaths.push_back("libs/glfw/include");
	includePaths.push_back("libs/glew/include");
	includePaths.push_back("libs/fmodex/include");
	includePaths.push_back("libs/poco/include");
	includePaths.push_back("libs/glm/include");
	includePaths.push_back("libs/utf8/include");
	includePaths.push_back("libs/json/include");
	includePaths.push_back("libs/pugixml/include");

	stringstream finalIncludeString;
	for (string& path: includePaths) {
		finalIncludeString << OF_PATH << "/" << path;
		if (path != includePaths[includePaths.size()-1]) {
			finalIncludeString << ":";
		}
	}

	cof.interp->AddIncludePath("../../../data");
//	cof.interp->AddIncludePath("../Frameworks");
	cof.interp->AddIncludePaths(finalIncludeString.str());
	cof.interp->AddIncludePath(ADDONS_PATH"/ofxInterface/src/components");
	cof.interp->AddIncludePath(ADDONS_PATH"/ofxInterface/src");
	cof.interp->AddIncludePath(ADDONS_PATH"/ofxNanoVG/src");
	cof.interp->AddIncludePath(ADDONS_PATH"/ofxNanoVG/libs/nanovg/src");
	cof.interp->AddIncludePath(ADDONS_PATH"/ofxNanoVG/libs/nanosvg/src");
	cof.interp->AddIncludePath(ADDONS_PATH"/ofxInterfaceEditor/src");
	cof.interp->AddIncludePath(ADDONS_PATH"/ofxJSON/src");
	cof.interp->AddIncludePath(ADDONS_PATH"/ofxJSON/libs/jsoncpp/include");
}


//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::onEnterHit(ofxInterfaceEditor::EventArgs &args)
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
		if (ternimalMode) {
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

void ofApp::onTabHit(ofxInterfaceEditor::EventArgs &args)
{
	string text = editor.getText();
	if (text.empty()) {
		return;
	}

	size_t pos = editor.getCaretPos();
	if (text[pos-1] == ' ' ||
		text[pos-1] == '\n') {
		return;
	}

	try {
		vector<string> completions;
		cof.interp->codeComplete(text, pos, completions);
		ofLog() << "\n\n\nCompletions:";
		for (string& str: completions) {
			ofLog() << "- "<<str;
		}
		args.continueNormalBehavior=false;
	}
	catch(exception& e) {
		ofLogError("ofApp") << "Exception on tab: "<<text<<"\npos="<<pos<<"\nwhat?\n"<<e.what();
	}
}

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

void ofApp::onNew(ofxInterface::TouchEvent &event)
{
	editor.setText("");
	editor.setTitle("Cling OF Playground");
	lastFile="";
}

void ofApp::onLoad(ofxInterface::TouchEvent &event)
{
	ofFileDialogResult result = ofSystemLoadDialog("Open File", false, ofToDataPath("workspace"));
	if (result.bSuccess) {
		editor.loadFromFile(result.filePath);
		lastFile = result.filePath;
		editor.setTitle("Cling OF Playground ( "+result.fileName+" )");
	}
}

void ofApp::onSave(ofxInterface::TouchEvent &event)
{
	if (lastFile != "") {
		editor.saveToFile(lastFile);
	}
	else {
		ofFileDialogResult result = ofSystemSaveDialog("Save to file...", "Save text to file");
		if (result.bSuccess) {
			editor.saveToFile(result.filePath);
			lastFile = result.filePath;
			editor.setTitle("Cling OF Playground ( "+result.fileName+" )");
		}
	}
}

void ofApp::onSaveAs(ofxInterface::TouchEvent &event)
{
	ofFileDialogResult result = ofSystemSaveDialog("Save to file...", "Save text to file");
	if (result.bSuccess) {
		editor.saveToFile(result.filePath);
		lastFile = result.filePath;
		editor.setTitle("Cling OF Playground ( "+result.fileName+" )");
	}
}

//--------------------------------------------------------------
void ofApp::update(){
	newBtn.setPosition(editor.getX(), editor.getY()-newBtn.getHeight()-24);
	loadBtn.setPosition(newBtn.getX()+newBtn.getWidth()+2, newBtn.getY());
	saveBtn.setPosition(loadBtn.getX()+loadBtn.getWidth()+2, loadBtn.getY());
	saveAsBtn.setPosition(saveBtn.getX()+saveBtn.getWidth()+2, saveBtn.getY());
	execToggle.setPosition(editor.getX()+editor.getWidth()-execToggle.getWidth(), newBtn.getY());


	cof.update();

	cof.scene.updateSubtreePostOrder(1.0f/60);
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofClear(cof.bgColor);
	cof.draw();
	cof.scene.render();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (!editor.getVisible()) {
		if (key == 'e') {
			setEditorVisible(true);
			editor.setPosition(ofGetMouseX(), ofGetMouseY());
		}
		return;
	}

	editor.keyPressed(key);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if (!editor.getVisible()) {
		return;
	}

	editor.keyReleased(key);
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
	editor.vscroll(x, y, v);
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::audioOut(float * output, int bufferSize, int nChannels)
{
	cof.audioOut(output, bufferSize, nChannels);
}

void ofApp::setEditorVisible(bool visible) {
	if (visible) {
		editor.activate();
		newBtn.activate();
		loadBtn.activate();
		saveBtn.activate();
		saveAsBtn.activate();
		execToggle.activate();
	}
	else {
		editor.deactivate();
		newBtn.deactivate();
		loadBtn.deactivate();
		saveBtn.deactivate();
		saveAsBtn.deactivate();
		execToggle.deactivate();
	}
}
