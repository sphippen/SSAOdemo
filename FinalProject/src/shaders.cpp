#include "shaders.h"

#include <cstdio>
#include <fstream>

using std::ifstream;

// These shader functions were written by Spencer Phippen in CS 5600.

GLuint loadShader(GLenum shaderType, const char* filename)
{
  // Open the file and read in the data
  int fileSize = 0;

  ifstream in(filename, ifstream::in | ifstream::binary);

  // If the file doesn't exist, quit
  if (!in.is_open())
  {
    fprintf(stderr, "Error: couldn't open file %s.\n", filename);
    exit(1);
  }

  // First get the size of the file
  in.seekg(0, ifstream::end);
  fileSize = in.tellg();
  in.seekg(0, ifstream::beg);

  // Create a buffer and read the data
  GLchar* shaderSource = new GLchar[fileSize + 1];
  in.read(shaderSource, fileSize);
  in.close();
  
  // Add a null terminator, as read does not do that
  shaderSource[fileSize] = '\0';

  // Set up and compile the shader
  GLuint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, const_cast<const GLchar **>(&shaderSource), NULL);
  glCompileShader(shader);

  // Check compilation status and log
  GLint result;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) // compilation was unsuccessful
  {
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &result);
    GLchar* compilationLog = new char[result];
    glGetShaderInfoLog(shader, result, &result, compilationLog);
    const char* whichShader = (shaderType == GL_VERTEX_SHADER ? "Vertex" : "Fragment");
    fprintf(stderr, "%s shader compilation failed in file %s\nLog: %s\n", whichShader, filename, compilationLog);
    delete[] compilationLog;
    exit(1);
  }

  return shader;
}

GLuint createProgram(GLuint vertexShader, GLuint fragShader)
{
  GLuint prog = glCreateProgram();
  glAttachShader(prog, vertexShader);
  glAttachShader(prog, fragShader);
  glLinkProgram(prog);

  // Check link status and log
  GLint result;
  glGetProgramiv(prog, GL_LINK_STATUS, &result);
  if (result == GL_FALSE) // linking was unsuccessful
  {
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &result);
    GLchar* linkLog = new char[result];
    glGetProgramInfoLog(prog, result, &result, linkLog);
    fprintf(stderr, "Program linking failed\nLog: %s\n", linkLog);
    delete[] linkLog;
    exit(1);
  }

  return prog;
}
