#ifndef SP_SHADERS_H_
#define SP_SHADERS_H_

#include "GL/glew.h"

GLuint loadShader(GLenum shaderType, const char* filename);

GLuint createProgram(GLuint vertexShader, GLuint fragShader);

#endif // SP_SHADERS_H_