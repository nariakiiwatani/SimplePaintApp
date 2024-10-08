#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	editor_.setup();
	
	cli_.subscribe("load", &editor_, &ImageEditor::loadImage);
	cli_.subscribe("save", &editor_, &ImageEditor::saveImage);
	cli_.subscribe("clear", &editor_, &ImageEditor::clear);
	cli_.subscribe("undo", &editor_, &ImageEditor::undo);
	cli_.subscribe("redo", &editor_, &ImageEditor::redo);
	cli_.subscribe("erase", [&]() { editor_.setModeEraser(true); });
	cli_.subscribe("draw", [&]() { editor_.setModeEraser(false); });
	cli_.subscribe("plot", [&]() { editor_.setModeBrush(); });
	cli_.subscribe("path", [&]() { editor_.setModePath(); });
	cli_.subscribe("size", &editor_, &ImageEditor::setPenSize);
	cli_.subscribe("fill", [&](float r, float g, float b, float a) {
		editor_.setPenColor(ofFloatColor{r,g,b,a});
	}, {0,0,0,1});
	cli_.subscribe("bg", [&](float r, float g, float b, float a) {
		editor_.setBackgroundColor(ofFloatColor{r,g,b,a});
	}, {0,0,0,1});
	cli_.subscribe("window", [](int x, int y, int w, int h) {
		ofSetWindowPosition(x, y);
		ofSetWindowShape(w, h);
	});
	
}
//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	editor_.draw();
	editor_.drawPending();
	editor_.drawCursor();
	ofPushStyle();
	ofSetColor(ofColor::gray);
	float x = 10, y = 10;
	cli_.draw(x,y);
	float tips_height = 20;
	cli_.drawTips(x,y+tips_height,0,tips_height);
	ofPopStyle();}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	bool is_cli_blank = cli_.getCurrent().getText().empty();
	switch(key) {
		case OF_KEY_LEFT: 
			if(is_cli_blank) {
				editor_.undo();
			}
			break;
		case OF_KEY_RIGHT:
			if(is_cli_blank) {
				editor_.redo();
			}
			break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	editor_.setCursorPosition({x,y}, false);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	editor_.setCursorPosition({x,y}, true);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	editor_.beginCommandGroup();
	editor_.setCursorPosition({x,y}, true);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	editor_.endCommandGroup(true);
}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
