/**
 *  snes4nacl
 *  (c) 2012 Jeffrey Quesnelle <jquesnelle@gmail.com>
 *  This code is licensed under GPLv3, see LICENSE 
 */
 
#ifndef _SNES4NACL_OPENGL_H
#define _SNES4NACL_OPENGL_H

#include "ppapi/c/ppb_opengles2.h"
#include "ppapi/cpp/graphics_3d_client.h"
#include "ppapi/cpp/graphics_3d.h"

namespace snes4nacl {

class OpenGL : public pp::Graphics3DClient 
{
public:
	explicit OpenGL();
	~OpenGL();
	
	bool start();
	void stop();
	void destroy();
	void size(int w, int h);
	void flip();
	void draw();
	void clear();
	
	void Graphics3DContextLost();
	
	bool setupScene(GLuint pixelFormat, GLuint pixelType, int maxSourceWidth);
	void setupSource(int sourceWidth, int sourceHeight);
	void setSourceData(const unsigned char* data);
	void setSmoothScale(bool set);
	
private:
	int width;
	int height;
	int sourceWidth;
	int sourceHeight;
	int trueTextureSize;
	pp::Graphics3D context;
	bool waitingForFlip;
	bool sceneCreated;
	
	//OpenGL members
	GLuint pixelFormat;
	GLuint pixelType;
	GLuint shaderProgram;
	GLint vertexPos_var;
	GLint textureCoord_var;
	GLint texture_var;
	GLint projection_var;
	GLuint texture;
	GLfloat verticies[12]; //4 points in 3D space representing the quad that covers the screen
	GLfloat matrix[16]; // 4x4 homogenous matrix representing projection onto viewport
	GLfloat textureCoords[8]; //4 points in 2D space representing how much of the source texture we're actually going to sample
	
	void setupViewport();
	void applySmoothScale();
	
	int trueTextureWidth;
	bool isSmoothScaled;
	
	static const char* sVertexSource;
	static const char* sFragmentSource;
	
	static void flipCallback(void* data, int result);
	
	static GLuint createShader(GLenum type, const char* source);
	static GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource);

};

}

#endif