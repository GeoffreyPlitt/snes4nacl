/**
 *  snes4nacl
 *  (c) 2012 Jeffrey Quesnelle <jquesnelle@gmail.com>
 *  This code is licensed under GPLv3, see LICENSE 
 */
 
#ifndef _SNES4NACL_GRAPHICS_H
#define _SNES4NACL_GRAPHICS_H

#include "callback.h"
#include "OpenGL.h"
#include "ppapi/cpp/fullscreen.h"

namespace snes4nacl {

class Graphics
{
public:
	Graphics();
	~Graphics();
	
	void init();
	void size(int width, int height);
	void sourceSize(int width, int height);
	void source(const unsigned char* pixels);
	void draw();
	void setSmoothScale(bool set);
	void setSmoothScale(const ScriptingBridge& bridge, const MethodParameter& parameters);
	
	bool isInited();
	
	void toggleFullScreen();
private:
	
	OpenGL* opengl;
	pp::Fullscreen* fullScreen;
	bool isFullScreen;

};

}

#endif