/**
 *  snes4nacl
 *  (c) 2012 Jeffrey Quesnelle <jquesnelle@gmail.com>
 *  This code is licensed under GPLv3, see LICENSE
 */

#include "Graphics.h"
#include "GLES2/gl2.h"
#include "snes9x.h"
#include "display.h"
#include "apu/apu.h"
#include "memmap.h"
#include "Instance.h"

#define SNES_BIGGEST_WIDTH 512

namespace snes4nacl {

Graphics::Graphics() : opengl(NULL), isFullScreen(false)
{
}

Graphics::~Graphics()
{
	if(opengl)
		delete opengl;
	if(GFX.Screen)
		delete [] GFX.Screen;
	if(fullScreen)
		delete fullScreen;
}

bool Graphics::isInited()
{
	return opengl != NULL;
}

void Graphics::init()
{
	printf("Graphics::init() -- Showing the show");

	if(!opengl)
		opengl = new OpenGL();
	opengl->start();
	opengl->setupScene(GL_RGB, GL_UNSIGNED_SHORT_5_6_5, SNES_BIGGEST_WIDTH);

	GFX.Pitch = 512;
    GFX.Screen = new uint16_t[1024 * 239];
    memset(GFX.Screen, 0, 1024 * 478);

	fullScreen = new pp::Fullscreen(Instance::get());
}

void Graphics::size(int width, int height)
{
	if(!opengl)
		return;
	opengl->stop();
	opengl->size(width, height);
	opengl->start();
	printf("Graphics::size() -- now %dx%d", width, height);
}

void Graphics::sourceSize(int width, int height)
{
	if(!opengl)
		return;
	opengl->setupSource(width, height);
	printf("Graphics::sourceSize() -- Source pixel data is %dx%d", width, height);
}

void Graphics::source(const unsigned char* pixels)
{
	if(!opengl)
		return;
	opengl->setSourceData(pixels);
}

void Graphics::draw()
{
	if(!opengl)
		return;
	opengl->clear();
	opengl->draw();
	opengl->flip();
}

void Graphics::setSmoothScale(bool set)
{
	if(opengl)
		opengl->setSmoothScale(set);
	printf("Graphics::setSmoothScale() -- Smooth scale %s", set ? "enabled" : "disabled");
}

void Graphics::setSmoothScale(const ScriptingBridge& bridge, const MethodParameter& parameters)
{
	MethodParameter::const_iterator arg(parameters.find("arg"));
	if(arg != parameters.end())
		setSmoothScale(arg->second == "1" ? true : false);
}

void Graphics::toggleFullScreen()
{
	if(fullScreen->SetFullscreen(!isFullScreen))
		isFullScreen = !isFullScreen;
}

}
