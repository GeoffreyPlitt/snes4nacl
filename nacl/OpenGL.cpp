/**
 *  snes4nacl
 *  (c) 2012 Jeffrey Quesnelle <jquesnelle@gmail.com>
 *  This code is licensed under GPLv3, see LICENSE
 */

#include "OpenGL.h"
#include "Instance.h"
#include "ppapi/cpp/completion_callback.h"
#include "ppapi/gles2/gl2ext_ppapi.h"
#include "GLES2/gl2.h"

namespace snes4nacl {

//Matches the vertices of the quad that covers the whole scene to the edges of the emulator pixels
const char* OpenGL::sVertexSource =
"attribute vec4 vert_pos;								\n"
"attribute vec2 tex_coord_in;							\n"
"varying vec2 tex_coord_final;							\n"
"uniform mat4 projection;								\n"
"void main()											\n"
"{														\n"
"	gl_Position = projection * vec4(vert_pos.xyz, 1);	\n"
"	tex_coord_final = tex_coord_in;						\n"
"}														\n";

//Directly samples the emulator pixels from the linearly interpolated coordinates
const char* OpenGL::sFragmentSource =
"precision mediump float;								\n"
"varying vec2 tex_coord_final;							\n"
"uniform sampler2D texture;								\n"
"void main()											\n"
"{														\n"
"	gl_FragColor = texture2D(texture, tex_coord_final);	\n"
"}														\n";

OpenGL::OpenGL() : pp::Graphics3DClient(Instance::get())
{
	shaderProgram = texture = 0;
	vertexPos_var = textureCoord_var = texture_var = projection_var = -1;
	width = height = sourceWidth = sourceHeight = trueTextureSize = 1;
	waitingForFlip = sceneCreated = false;
	isSmoothScaled = true;
}

OpenGL::~OpenGL()
{
	glSetCurrentContextPPAPI(0);
}

void OpenGL::size(int width, int height)
{
	this->width = width;
	this->height = height;
	if(!context.is_null())
	{
		int32_t ret = context.ResizeBuffers(width, height);
		printf("OpenGL::size() -- Graphics3D::ResizeBuffers() returned %d", ret);
	}

}

bool OpenGL::start()
{
	if(context.is_null())
	{
		int32_t attribs[] = {
			PP_GRAPHICS3DATTRIB_WIDTH, width,
			PP_GRAPHICS3DATTRIB_HEIGHT, height,
			PP_GRAPHICS3DATTRIB_NONE
		};
		context = pp::Graphics3D(Instance::get(), pp::Graphics3D(), attribs);
		if (context.is_null())
		{
			glSetCurrentContextPPAPI(0);
			printf("OpenGL::start() -- Unable to create OpenGL context");
			return false;
		}
		if(Instance::get()->BindGraphics(context))
			printf("OpenGL::start() -- OpenGL context created");
		else
		{
			printf("OpenGL::start() -- Could not bind graphics to instance");
			return false;
		}
	}
	glSetCurrentContextPPAPI(context.pp_resource());
	if(sceneCreated)
			setupViewport();
	return true;
}

void OpenGL::stop()
{
	glSetCurrentContextPPAPI(0);
}

void OpenGL::flip()
{
	if(waitingForFlip)
		return;
	waitingForFlip = true;
	context.SwapBuffers(pp::CompletionCallback(&flipCallback, this));
}

void OpenGL::flipCallback(void* ptr, int result)
{
	 static_cast<OpenGL*>(ptr)->waitingForFlip = false;
}

void OpenGL::Graphics3DContextLost()
{
	printf("Yikes, OpenGL context lost!");
}

GLuint OpenGL::createShader(GLenum type, const char* source)
{
	GLuint shader = glCreateShader(type);
	if(shader)
	{
		glShaderSource(shader, 1, &source, NULL);
		glCompileShader(shader);
		GLint wasCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &wasCompiled);
		if(!wasCompiled)
		{
			GLint errorLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errorLen);
			if(errorLen)
			{
				char* error = new char[errorLen];
				glGetShaderInfoLog(shader, errorLen, NULL, error);
				printf("OpenGL::createShader() -- Error compiling shader: %s", error);
				delete [] error;
			}
			glDeleteShader(shader);
			shader = 0;
		}
	}
	else
		printf("OpenGL::createShader() -- Unable to allocate shader of type %d", type);
	return shader;
		printf("Unable to create OpenGL shader of type %d", type);
}

GLuint OpenGL::createShaderProgram(const char* vertexSource, const char* fragmentSource)
{
	GLuint vertex = createShader(GL_VERTEX_SHADER, vertexSource);
	if(!vertex)
		return 0;
	GLuint fragment = createShader(GL_FRAGMENT_SHADER, fragmentSource);
	if(!fragment)
		return 0;
	GLuint program = glCreateProgram();
	if(program)
	{
		glAttachShader(program, vertex);
		glAttachShader(program, fragment);
		glLinkProgram(program);
		GLint linkResult = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &linkResult);
		if(linkResult != GL_TRUE)
		{
			GLint errorLen = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &errorLen);
			if(errorLen)
			{
				char* error = new char[errorLen];
				glGetProgramInfoLog(program, errorLen, NULL, error);
				printf("OpenGL::createShaderProgram() -- Error linking program: %s", error);
				delete [] error;
			}
			glDeleteProgram(program);
			program = 0;
		}
	}
	else
		printf("OpenGL::createShaderProgram() -- Unable to create shader program");
	return program;
}

bool OpenGL::setupScene(GLuint pixelFormat, GLuint pixelType, int maxSource)
{
	printf("OpenGL::setupScene() -- Starting scene creation");
	shaderProgram = createShaderProgram(sVertexSource, sFragmentSource);
	if(!shaderProgram)
		return false;
	printf("OpenGL::setupScene() -- Shaders compiled");

	vertexPos_var = glGetAttribLocation(shaderProgram, "vert_pos");
	textureCoord_var = glGetAttribLocation(shaderProgram, "tex_coord_in");
	projection_var = glGetUniformLocation(shaderProgram, "projection");
	texture_var = glGetUniformLocation(shaderProgram, "texture");

	this->pixelFormat = pixelFormat;
	this->pixelType = pixelType;
	trueTextureSize = maxSource;
	glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	applySmoothScale();

	glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, trueTextureSize, trueTextureSize, 0, pixelFormat, pixelType, NULL);

	glDisable(GL_DEPTH_TEST);
	sceneCreated = true;

	setupViewport();

	return true;
}

void OpenGL::setupViewport()
{
	glViewport(0, 0, width, height);

	//top left
	verticies[0] = 0;
	verticies[1] = height;
	verticies[2] = 0;

	//top right
	verticies[3] = width;
	verticies[4] = height;
	verticies[5] = 0;

	//bottom left
	verticies[6] = 0;
	verticies[7] = 0;
	verticies[8] = 0;

	//bottom right
	verticies[9] = width;
	verticies[10] = 0;
	verticies[11] = 0;

	//transform to screen space
	matrix[0] = 2.0f / width;
	matrix[1] = 0;
	matrix[2] = 0;
	matrix[3] = 0;

	matrix[4] = 0;
	matrix[5] = 2.0f / -height;
	matrix[6] = 0;
	matrix[7] = 0;

	matrix[8] = 0;
	matrix[9] = 0 ;
	matrix[10] = -2.0f;
	matrix[11] = 0;

	matrix[12] = -1;
	matrix[13] = 1;
	matrix[14] = -1;
	matrix[15] = 1;
}

void OpenGL::setupSource(int _sourceWidth, int _sourceHeight)
{
	sourceWidth = _sourceWidth;
	sourceHeight = _sourceHeight;
	float normalX = (float)sourceWidth / (float)trueTextureSize;
	float normalY = (float)sourceHeight / (float)trueTextureSize;

	textureCoords[0] = 0;
	textureCoords[1] = normalY;

	textureCoords[2] = normalX;
	textureCoords[3] = normalY;

	textureCoords[4] = 0;
	textureCoords[5] = 0;

	textureCoords[6] = normalX;
	textureCoords[7] = 0;

}

void OpenGL::setSourceData(const unsigned char* data)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, sourceWidth, sourceHeight, pixelFormat, pixelType, data);
}

void OpenGL::draw()
{
	glUseProgram(shaderProgram);
	//load the quad verticies
	glVertexAttribPointer(vertexPos_var, 3, GL_FLOAT, GL_FALSE, 0, verticies);
	//load how much of the texture we're actually going to sample
	glVertexAttribPointer(textureCoord_var, 2, GL_FLOAT, GL_FALSE, 0, textureCoords);
	glEnableVertexAttribArray(vertexPos_var);
	glEnableVertexAttribArray(textureCoord_var);

	//activate the actual source texture
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(texture_var, 0);

	//load the projection matrix
	glUniformMatrix4fv(projection_var, 1, GL_FALSE, matrix);

	//actually draw :)
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void OpenGL::clear()
{
	glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGL::setSmoothScale(bool set)
{
	isSmoothScaled = set;
	applySmoothScale();
}

void OpenGL::applySmoothScale()
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, isSmoothScaled ? GL_LINEAR : GL_NEAREST );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, isSmoothScaled ? GL_LINEAR : GL_NEAREST );
}

}
