#pragma once

#include <string>
#include "ofColor.h"
#include "ofFbo.h"
#include <glm/vec2.hpp>
#include "ofTexture.h"
#include "ofRectangle.h"

class ImageEditor
{
public:
	void setup(ofColor bg_color=ofColor::black);
	void loadImage(std::string filepath);
	
	void saveImage(std::string filepath);

	void clear();
	
	void setModeBrush();
	void setModePath();
	void toggleModeEraser();
	void setModeEraser(bool eraser);
	
	void setPenColor(ofColor color);
	void setPenSize(float size);
	void setBackgroundColor(ofColor color);
	
	void setCursorPosition(glm::vec2 point, bool plot);
	
	void beginCommandGroup();
	void endCommandGroup(bool apply);
	
	void draw() const;
	void drawCursor() const;
	void drawPending() const;
	
	void undo();
	void redo();
	
private:
	struct Plot {
		std::vector<glm::vec2> point;
	};
	struct Texture {
		ofTexture tex;
		ofRectangle rect;
	};
	struct Background {
		ofColor color = ofColor::black;
	};
	struct State {
		ofColor bg_color = ofColor::black;
		ofColor brush_color = ofColor::white;
		float pen_size = 10;
		bool is_eraser = false;
		bool is_path = false;
	};
	struct Command {
		enum Type {
			PLOT,
			IMAGE,
			STATE,
			CLEAR
		} type;
		Plot plot;
		State state;
		Texture image;
		
		Command(Plot plot, State state):type(PLOT),plot(plot),state(state){}
		Command(State state):type(STATE),state(state){}
		Command(Texture image):type(IMAGE),image(image){}
		Command():type(CLEAR){}
	};
	
	void drawHistory() const;
	void applyState();
	void drawPlots(Plot plot, State state) const;
	
	std::vector<Command> history_;
	int valid_history_size_ = 0;
	int valid_history_begin_index_ = 0;
	void addHistory(const Command &c);
	bool is_plot_pending_ = false;
	glm::vec2 cursor_pos_;
	Plot plot_;
	State state_;
};
