#include "ImageEditor.h"
#include "ofImage.h"
#include "ofTexture.h"
#include "ofGraphics.h"
#include "ofPixels.h"
#include "ofPath.h"

void ImageEditor::setup(ofColor bg_color)
{
	setBackgroundColor(bg_color);
	clear();
	valid_history_begin_index_ = valid_history_size_;
}
void ImageEditor::loadImage(std::string filepath)
{
	Texture image;
	if(!ofLoadImage(image.tex, filepath)) {
		return;
	}
	image.rect = ofRectangle(0,0,image.tex.getWidth(), image.tex.getHeight());
	addHistory(image);
}

void ImageEditor::saveImage(std::string filepath)
{
	ofFbo fbo;
	fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
	fbo.begin();
	ofClear(0);
	drawHistory();
	fbo.end();
	ofPixels pix;
	fbo.readToPixels(pix);
	ofSaveImage(pix, filepath);
}

void ImageEditor::clear()
{
	endCommandGroup(false);
	addHistory(Command());
}

void ImageEditor::setModeBrush()
{
	endCommandGroup(false);
	state_.is_path = false;
	addHistory(Command(state_));
}
void ImageEditor::setModePath()
{
	endCommandGroup(false);
	state_.is_path = true;
	addHistory(Command(state_));
}
void ImageEditor::setModeEraser(bool eraser)
{
	endCommandGroup(false);
	state_.is_eraser = eraser;
	addHistory(Command(state_));
}
void ImageEditor::toggleModeEraser()
{
	setModeEraser(!state_.is_eraser);
}

void ImageEditor::setPenColor(ofColor color)
{
	endCommandGroup(false);
	state_.brush_color = color;
	addHistory(Command(state_));
}
void ImageEditor::setPenSize(float size)
{
	endCommandGroup(false);
	state_.pen_size = size;
	addHistory(Command(state_));
}
void ImageEditor::setBackgroundColor(ofColor color)
{
	endCommandGroup(false);
	state_.bg_color = color;
	addHistory(Command(state_));
}

void ImageEditor::setCursorPosition(glm::vec2 point, bool plot)
{
	cursor_pos_ = point;
	if(plot) {
		plot_.point.push_back(point);
	}
}
void ImageEditor::beginCommandGroup()
{
	is_plot_pending_ = true;
}

void ImageEditor::endCommandGroup(bool apply)
{
	if(apply) {
		if(is_plot_pending_ && !plot_.point.empty()) {
			addHistory(Command(plot_, state_));
		}
	}
	plot_.point.clear();
	is_plot_pending_ = false;
}

void ImageEditor::draw() const
{
	drawHistory();
}
void ImageEditor::drawCursor() const
{
	auto drawCross = [](float size, ofColor inner_color, ofColor outer_color) {
		ofPushStyle();
		ofSetColor(outer_color);
		ofSetLineWidth(3);
		ofDrawLine(-size, 0, size, 0);
		ofDrawLine(-0, -size, 0, size);
		ofSetColor(inner_color);
		ofSetLineWidth(1);
		ofDrawLine(-size, 0, size, 0);
		ofDrawLine(-0, -size, 0, size);
		ofPopStyle();
	};
	ofColor pen_color = state_.is_eraser ? state_.bg_color : state_.brush_color;
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(cursor_pos_);
	drawCross(ofGetWindowWidth(), ofColor::white, ofColor::black);
	if(state_.is_path) {
		ofRotateZDeg(45);
		drawCross(10, pen_color, ofColor::white);
	}
	else {
		ofFill();
		ofSetColor(pen_color, 128);
		ofDrawCircle(0,0,state_.pen_size);
		ofNoFill();
		ofSetColor(ofColor::white);
		ofDrawCircle(0,0,state_.pen_size);
	}
	ofPopMatrix();
	ofPopStyle();
}
void ImageEditor::drawPending() const
{
	auto state = state_;
	state.bg_color.a =
	state.brush_color.a = 128;
	drawPlots(plot_, state);
}

void ImageEditor::drawHistory() const
{
	auto &&command = history_;
	int valid_start = 0;
	int valid_end = valid_history_size_;
	
	State state_save = state_;
	for(int i = valid_start; i < valid_end; ++i) {
		auto &&c = command[i];
		switch(c.type) {
			case Command::PLOT:
			case Command::IMAGE:
				break;
			case Command::STATE:
				state_save = c.state;
				break;
			case Command::CLEAR:
				valid_start = i;
				break;
		}
	}
	ofClear(state_save.bg_color);

	for(int i = valid_start; i < valid_end; ++i) {
		auto &&c = command[i];
		switch(c.type) {
			case Command::PLOT:
				drawPlots(c.plot, c.state);
				break;
			case Command::IMAGE:
				c.image.tex.draw(c.image.rect);
				break;
			case Command::STATE:
				state_save = c.state;
				break;
			case Command::CLEAR:
//				ofClear(state_save.bg_color);
				break;
		}
	}
}
void ImageEditor::applyState()
{
	auto &&command = history_;
	int valid_start = 0;
	int valid_end = valid_history_size_;
	
	for(int i = valid_start; i < valid_end; ++i) {
		auto &&c = command[i];
		if(c.type == Command::STATE) {
			state_ = c.state;
		}
	}
}

void ImageEditor::drawPlots(Plot plot, State state) const
{
	ofPath path;
	if(state.is_path) {
		bool is_first = true;
		for(auto &&p : plot.point) {
			if(is_first) {
				path.moveTo(p);
				is_first = false;
			}
			else {
				path.lineTo(p);
			}
		}
	}
	else {
		for(auto &&p : plot.point) {
			path.circle(p, state.pen_size);
		}
	}
	path.setFilled(true);
	path.setFillColor(state.is_eraser ? state.bg_color : state.brush_color);
	path.setPolyWindingMode(OF_POLY_WINDING_NONZERO);
	path.draw();
}

void ImageEditor::undo()
{
	valid_history_size_ = std::max(valid_history_begin_index_, valid_history_size_-1);
}
void ImageEditor::redo()
{
	valid_history_size_ = std::min<int>(history_.size(), valid_history_size_+1);
}

void ImageEditor::addHistory(const Command &c)
{
	history_.resize(valid_history_size_);
	history_.push_back(c);
	++valid_history_size_;
}
