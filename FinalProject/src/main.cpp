// File: main.cpp
// Author: Spencer Phippen (u0683479)
// Class: CS 5610
// Final Project
//
// Contains nearly all the code for Final Project, including model loading,
// OpenGL drawing, and more.

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "GL/glew.h"
#include "GL/glut.h"
#include "rply.h"

#include "vec3.h"
#include "mat4.h"
#include "texture.h"
#include "shaders.h"

using std::vector;

const static int wWidth = 1024;
const static int wHeight = 768;

void initializeOpenGL();
void loadShaders();
void setupState();
void loadModel();

void myGlutKeyboard(unsigned char key, int x, int y);
void myGlutSpecial(int key, int x, int y);
void myGlutMouse(int button, int state, int x, int y);
void myGlutMotion(int x, int y);
void myGlutIdle();

void drawModel(bool ssao);
void doSSAO();
void doBlur();

int main_window;

// the camera info
Vec3 eye;
Vec3 lookat;

// Shader programs and attrib/uniform locations
GLuint phongProg;
GLint phongProgPosAttrib;
GLint phongProgNormAttrib;
GLint phongProgModelViewMat;
GLint phongProgMvpMat;
GLint phongProgNormalMat;
GLint phongProgLAmb;
GLint phongProgLPos;
GLint phongProgLDif;
GLint phongProgLSpc;
GLint phongProgKAmb;
GLint phongProgKDif;
GLint phongProgKSpc;
GLint phongProgKShn;
GLint phongProgDoSSAO;

GLuint aoProg;
GLint aoProgPosAttrib;
GLint aoProgDepthTexture;
GLint aoProgNormTexture;
GLint aoProgRandomTexture;
GLint aoProgProjMat;
GLint aoProgInvProjMat;
GLint aoProgSampleOffsets;
GLint aoProgSampleRadius;
GLint aoProgRandomTexCoordScale;

GLuint blurProg;
GLint blurProgPosAttrib;
GLint blurProgAoTexture;
GLint blurProgColorTexture;
GLint blurProgInvRes;

// Contain data for the model
vector<GLfloat> modelVertices;
vector<GLuint> faceIndices;
vector<GLfloat> allModelData;

GLuint vertexDataBuf;
GLuint floorBuf;

GLsizei faceIndexCount;

// Program functionality variables
// Shading related
int ambientOcclusionState;
float depthDiscontinuityRadius;

// Framebuffers, textures to render to, renderbuffers, other textures
GLuint framebuffer;
GLuint depthTexture;
GLuint colorTexture;
GLuint normalTexture;
GLuint aoTexture;
GLuint depthRenderbuffer;
GLuint randomTexture;

// draw the scene
void myGlutDisplay()
{
  static int frameNum = 0;
  GLenum error = glGetError();
  if (error != GL_NO_ERROR)
  {
    fprintf(stderr, "gl error\n");
  }
  glClearColor(0, 0, 0, 0);

  // If we're rendering with ambient occlusion
  if (ambientOcclusionState) {
    // TODO: fill in yo
    drawModel(true);
    doSSAO();
    doBlur();
  }
  else {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    drawModel(false);
  }

  frameNum++;
  //printf("%d\n", frameNum);

  glutSwapBuffers();
}

// entry point
int main(int argc, char* argv[])
{
  // Initialize glut
  glutInit(&argc, argv);

  setupState();

  //
  // create the glut window
  //
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(wWidth, wHeight);
  glutInitWindowPosition(100,100);
  main_window = glutCreateWindow("Sample Interface");

  GLenum glewResult = glewInit();
  if (glewResult != GLEW_OK) {
    fprintf(stderr, "GLEW initialization failed.\n");
    exit(1);
  }

  //
  // set callbacks
  //
  glutDisplayFunc(myGlutDisplay);
  glutIdleFunc(myGlutIdle);
  glutKeyboardFunc(myGlutKeyboard);
  glutSpecialFunc(myGlutSpecial);
  glutMouseFunc(myGlutMouse);
  glutMotionFunc(myGlutMotion);

  // initialize the camera
  eye = Vec3(0, 1.5f, 1.5f);
  lookat = Vec3(0, 0, 0);

  // initialize gl
  initializeOpenGL();
  loadShaders();
  
  // load the model
  loadModel();

  printf("Use 'a' key to enable/disable ambient occlusion.\n");
  printf("Use up/down arrow keys to increase/decrease depth discontinuity radius.\n");

  // give control over to glut
  glutMainLoop();

  return 0;
}

// ----------------- INITIALIZATION FUNCTIONS -------------- //
void setupState()
{
  ambientOcclusionState = 0;
  depthDiscontinuityRadius = 0.01f;
}

void initializeOpenGL()
{
  glEnable(GL_DEPTH_TEST);

  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
  glCullFace(GL_BACK);

  // Enable and configure textures on applicable texture units
  glActiveTexture(GL_TEXTURE0);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // Create our framebuffer
  glGenFramebuffers(1, &framebuffer);

  // Setup a texture to render depth to
  glGenTextures(1, &depthTexture);
  glBindTexture(GL_TEXTURE_2D, depthTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, wWidth, wHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // Setup a texture to render color to
  glGenTextures(1, &colorTexture);
  glBindTexture(GL_TEXTURE_2D, colorTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wWidth, wHeight, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  // Setup a texture to store normals in
  glGenTextures(1, &normalTexture);
  glBindTexture(GL_TEXTURE_2D, normalTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wWidth, wHeight, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // Setup a texture to store occlusion values in
  glGenTextures(1, &aoTexture);
  glBindTexture(GL_TEXTURE_2D, aoTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wWidth, wHeight, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  // Setup a renderbuffer to use for depth
  glGenRenderbuffers(1, &depthRenderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, wWidth, wHeight);

  // Set up a texture used to store random sample offset values
  glGenTextures(1, &randomTexture);
  glBindTexture(GL_TEXTURE_2D, randomTexture);
  // All these were calculated offline, the values may look weird because they're
  // already scaled and biased from [-1, 1] to [0, 1].
  static const GLfloat randomDirections[48] = { 0.682909, 0.965344, 0.500000,
      0.164830, 0.871026, 0.500000, 0.896964, 0.804005, 0.500000, 0.127030, 0.833006, 0.500000,
      0.960280, 0.695300, 0.500000, 0.198356, 0.898761, 0.500000, 0.245320, 0.069723, 0.500000,
      0.928136, 0.241738, 0.500000, 0.704112, 0.956441, 0.500000, 0.984019, 0.374598, 0.500000,
      0.171691, 0.122888, 0.500000, 0.806018, 0.895415, 0.500000, 0.951012, 0.715844, 0.500000,
      0.633597, 0.981822, 0.500000, 0.173631, 0.878791, 0.500000, 0.338373, 0.973156, 0.500000 };
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 4, 4, 0, GL_RGB, GL_FLOAT, randomDirections);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // Set up buffer for floor data
  const static GLfloat floorData[36] = { -10.0f, -0.4f, -10.0f, 0.0f, 1.0f, 0.0f,
      -10.0f, -0.4f, 10.0f, 0.0f, 1.0f, 0.0f, 10.0f, -0.4f, 10.0f, 0.0f, 1.0f, 0.0f,
      10.0f, -0.4f, 10.0f, 0.0f, 1.0f, 0.0f, 10.0f, -0.4f, -10.0f, 0.0f, 1.0f, 0.0f,
      -10.0f, -0.4f, -10.0f, 0.0f, 1.0f, 0.0f};
  glGenBuffers(1, &floorBuf);
  glBindBuffer(GL_ARRAY_BUFFER, floorBuf);
  glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(GLfloat), floorData, GL_STATIC_DRAW);
}

void loadShaders()
{
  // Phong shaders
  phongProg = createProgram(loadShader(GL_VERTEX_SHADER, "shaders/phong.vert"),
      loadShader(GL_FRAGMENT_SHADER, "shaders/phong.frag"));
  
  phongProgPosAttrib = glGetAttribLocation(phongProg, "positionIn");
  phongProgNormAttrib = glGetAttribLocation(phongProg, "normalIn");
  phongProgModelViewMat = glGetUniformLocation(phongProg, "modelViewMat");
  phongProgMvpMat = glGetUniformLocation(phongProg, "modelViewProjMat");
  phongProgNormalMat = glGetUniformLocation(phongProg, "normalMat");
  phongProgLAmb = glGetUniformLocation(phongProg, "lAmbient");
  phongProgLPos = glGetUniformLocation(phongProg, "lPosition");
  phongProgLDif = glGetUniformLocation(phongProg, "lDiffuse");
  phongProgLSpc = glGetUniformLocation(phongProg, "lSpecular");
  phongProgKAmb = glGetUniformLocation(phongProg, "kAmbient");
  phongProgKDif = glGetUniformLocation(phongProg, "kDiffuse");
  phongProgKSpc = glGetUniformLocation(phongProg, "kSpecular");
  phongProgKShn = glGetUniformLocation(phongProg, "kShininess");
  phongProgDoSSAO = glGetUniformLocation(phongProg, "doSSAO");

  // Ambient occlusion shaders
  aoProg = createProgram(loadShader(GL_VERTEX_SHADER, "shaders/ssao.vert"),
      loadShader(GL_FRAGMENT_SHADER, "shaders/ssao.frag"));

  aoProgPosAttrib = glGetAttribLocation(aoProg, "positionIn");
  aoProgDepthTexture = glGetUniformLocation(aoProg, "depthTex");
  aoProgNormTexture = glGetUniformLocation(aoProg, "normTex");
  aoProgRandomTexture = glGetUniformLocation(aoProg, "randomTex");
  aoProgProjMat = glGetUniformLocation(aoProg, "projMat");
  aoProgInvProjMat = glGetUniformLocation(aoProg, "invProjMat");
  aoProgSampleOffsets = glGetUniformLocation(aoProg, "sampleOffsets");
  aoProgSampleRadius = glGetUniformLocation(aoProg, "sampleRadius");
  aoProgRandomTexCoordScale = glGetUniformLocation(aoProg, "randomTexCoordScale");

  // Blur texture shaders
  blurProg = createProgram(loadShader(GL_VERTEX_SHADER, "shaders/blur.vert"),
      loadShader(GL_FRAGMENT_SHADER, "shaders/blur.frag"));

  blurProgPosAttrib = glGetAttribLocation(blurProg, "positionIn");
  blurProgAoTexture = glGetUniformLocation(blurProg, "aoTex");
  blurProgColorTexture = glGetUniformLocation(blurProg, "colorTex");
  blurProgInvRes = glGetUniformLocation(blurProg, "invRes");

}

static GLfloat maxValue = 0.0;

void load_error_cb(p_ply ply, const char* message)
{
  fprintf(stderr, "Error loading model: %s\n", message);
}

int load_read_vertex_cb(p_ply_argument argument)
{
  // Get an offset into the modelVertices vector
  long offset;
  ply_get_argument_element(argument, NULL, &offset);
  offset *= 3;

  p_ply_property prop;
  const char* propName;
  ply_get_argument_property(argument, &prop, NULL, NULL);
  ply_get_property_info(prop, &propName, NULL, NULL, NULL);
  switch (propName[0]) {
    case 'x':
      break;
    case 'y':
      offset += 1;
      break;
    case 'z':
      offset += 2;
      break;
  }
  modelVertices[offset] = ply_get_argument_value(argument);
  if (fabs(modelVertices[offset]) > fabs(maxValue)) {
    maxValue = fabs(modelVertices[offset]);
    //printf("%f\n", maxValue);
  }
  return 1;
}

int load_read_face_cb(p_ply_argument argument)
{
  // Get an offset into the faceIndices vector
  long offset;
  ply_get_argument_element(argument, NULL, &offset);
  offset *= 3;

  long valueIndex;
  ply_get_argument_property(argument, NULL, NULL, &valueIndex);
  if (valueIndex >= 0) {
    offset += valueIndex;
  }
  faceIndices[offset] = ply_get_argument_value(argument);
  return 1;
}

void loadModel()
{
  // ////////////hmm...>>>>>>>>>>>>
  int result;
  p_ply plyModel = ply_open("resources/bun_zipper.ply", load_error_cb, 0, NULL);
  if (!plyModel)
    return;

  result = ply_read_header(plyModel);
  if (!result)
    return;

  long vertexCount = ply_set_read_cb(plyModel, "vertex", "x", load_read_vertex_cb, NULL, 0);
  ply_set_read_cb(plyModel, "vertex", "y", load_read_vertex_cb, NULL, 1);
  ply_set_read_cb(plyModel, "vertex", "z", load_read_vertex_cb, NULL, 2);

  modelVertices.resize(vertexCount * 3);

  long triCount = ply_set_read_cb(plyModel, "face", "vertex_indices", load_read_face_cb, NULL, 0);
  faceIndices.resize(triCount * 3);
  allModelData.resize(triCount * 3 * 6);

  result = ply_read(plyModel);
  if (!result) {
    modelVertices.clear();
    faceIndices.clear();
    allModelData.clear();
  }

  ply_close(plyModel);

  // Put ALL the data into |allModelData|.
  // Scale vertices to unit cube
  float scaleFactor = 1.0f / maxValue;
  Vec3 halfUnit(0.0f, 0.5f, 0.0f);
  for (vector<GLuint>::size_type faceIndex = 0; faceIndex < faceIndices.size() / 3; faceIndex++) {
    GLuint vi1, vi2, vi3;
    vi1 = faceIndices[faceIndex*3];
    vi2 = faceIndices[faceIndex*3+1];
    vi3 = faceIndices[faceIndex*3+2];
    
    Vec3 v1(modelVertices[vi1*3], modelVertices[vi1*3+1], modelVertices[vi1*3+2]);
    Vec3 v2(modelVertices[vi2*3], modelVertices[vi2*3+1], modelVertices[vi2*3+2]);
    Vec3 v3(modelVertices[vi3*3], modelVertices[vi3*3+1], modelVertices[vi3*3+2]);
    
    // Scale vertices to unit cube
    v1 = v1.scale(scaleFactor);
    v2 = v2.scale(scaleFactor);
    v3 = v3.scale(scaleFactor);
    
    v1 = v1.subtract(halfUnit);
    v2 = v2.subtract(halfUnit);
    v3 = v3.subtract(halfUnit);

    Vec3 normal = (v2 - v1).cross(v3 - v1);
    normal.normalize();
    
    allModelData[faceIndex*18] = v1.x;
    allModelData[faceIndex*18+1] = v1.y;
    allModelData[faceIndex*18+2] = v1.z;
    allModelData[faceIndex*18+3] = normal.x;
    allModelData[faceIndex*18+4] = normal.y;
    allModelData[faceIndex*18+5] = normal.z;
    allModelData[faceIndex*18+6] = v2.x;
    allModelData[faceIndex*18+7] = v2.y;
    allModelData[faceIndex*18+8] = v2.z;
    allModelData[faceIndex*18+9] = normal.x;
    allModelData[faceIndex*18+10] = normal.y;
    allModelData[faceIndex*18+11] = normal.z;
    allModelData[faceIndex*18+12] = v3.x;
    allModelData[faceIndex*18+13] = v3.y;
    allModelData[faceIndex*18+14] = v3.z;
    allModelData[faceIndex*18+15] = normal.x;
    allModelData[faceIndex*18+16] = normal.y;
    allModelData[faceIndex*18+17] = normal.z;
  }

  // Set up VBO for vertex data
  glGenBuffers(1, &vertexDataBuf);
  glBindBuffer(GL_ARRAY_BUFFER, vertexDataBuf);
  glBufferData(GL_ARRAY_BUFFER, allModelData.size() * sizeof(GLfloat), allModelData.data(), GL_STATIC_DRAW);

  faceIndexCount = faceIndices.size();
  
  // Clear the vectors now, we're done with them
  modelVertices.clear();
  faceIndices.clear();
  allModelData.clear();
}

// ------------------- CALLBACK FUNCTIONS ----------------- //
void myGlutIdle()
{
  // make sure the main window is active
  if (glutGetWindow() != main_window)
    glutSetWindow(main_window);

  // if you have moving objects, you can do that here

  // just keep redrawing the scene over and over
  glutPostRedisplay();
}

// mouse handling functions for the main window
// left mouse translates, middle zooms, right rotates
// keep track of which button is down and where the last position was
int cur_button = -1;
int last_x;
int last_y;

// catch mouse up/down events
void myGlutMouse(int button, int state, int x, int y)
{
  if (state == GLUT_DOWN) {
    cur_button = button;
  }
  else {
    if (button == cur_button)
      cur_button = -1;
  }

  last_x = x;
  last_y = y;
}

// catch mouse move events
void myGlutMotion(int x, int y)
{
  // the change in mouse position
  int dx = x-last_x;
  int dy = y-last_y;

  float scale, len, theta;
  Vec3 neye, neye2;
  Vec3 f, r, u;

  switch(cur_button) {
  case GLUT_LEFT_BUTTON:
    // translate
    f = lookat.add(eye.scale(-1));
    u.x = 0;
    u.y = 1;
    u.z = 0;

    // scale the change by how far away we are
    scale = f.norm() * 0.007;

    r = f.cross(u);
    u = r.cross(f);

    r.normalize();
    u.normalize();

    eye = eye.add(r.scale(-1).scale(dx).scale(scale));
    eye = eye.add(u.scale(dy).scale(scale));

    lookat = lookat.add(r.scale(-1).scale(dx).scale(scale));
    lookat = lookat.add(u.scale(dy).scale(scale));

    break;

  case GLUT_MIDDLE_BUTTON:
    // zoom
    f = lookat.add(eye.scale(-1));

    len = f.norm();
    f.normalize();

    // scale the change by how far away we are
    len -= sqrt(len)*dx*0.03;

    eye = lookat.add(f.scale(-1).scale(len));

    // make sure the eye and lookat points are sufficiently far away
    // push the lookat point forward if it is too close
    if (len < 1) {
      printf("lookat move: %f\n", len);
      lookat = eye.add(f);
    }

    break;

  case GLUT_RIGHT_BUTTON:
    // rotate

    neye = eye.add(lookat.scale(-1));

    // first rotate in the x/z plane
    theta = -dx * 0.007;
    neye2.x = (float)cos(theta)*neye.x + (float)sin(theta)*neye.z;
    neye2.y = neye.y;
    neye2.z =-(float)sin(theta)*neye.x + (float)cos(theta)*neye.z;


    // now rotate vertically
    theta = -dy * 0.007;

    f = neye2.scale(-1);
    u.x = 0;
    u.y = 1;
    u.z = 0;

    r = f.cross(u);
    u = r.cross(f);

    len = f.norm();
    f.normalize();
    u.normalize();

    neye = f.scale(cos(theta)).add(u.scale(sin(theta))).scale(len);

    eye = lookat.add(neye.scale(-1));

    break;
  }

  last_x = x;
  last_y = y;

  glutPostRedisplay();
}

// you can put keyboard shortcuts in here
void myGlutKeyboard(unsigned char key, int x, int y)
{
  switch(key) {
  // SSAO
  case 'a':
  case 'A':
    ambientOcclusionState = !ambientOcclusionState;
    if (ambientOcclusionState) {
      printf("Enabled ambient occlusion.\n");
    }
    else {
      printf("Disabled ambient occlusion.\n");
    }
    break;
  // quit
  case 27: // esc
  case 'q':
  case 'Q':
    exit(0);
    break;
  }

  glutPostRedisplay();
}

void myGlutSpecial(int key, int x, int y)
{
  switch (key) {
  // Increase depth discont. radius
  case GLUT_KEY_UP:
    depthDiscontinuityRadius *= 2.0f;
    printf("Depth discontinuity radius increased to %f\n", depthDiscontinuityRadius);
    break;
  case GLUT_KEY_DOWN:
    depthDiscontinuityRadius *= 0.5f;
    printf("Depth discontinuity radius decreased to %f\n", depthDiscontinuityRadius);
    break;
  }
  glutPostRedisplay();
}

// ------------------- DRAW FUNCTIONS ----------------- //
void drawModel(bool ssao)
{
  if (ssao) {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
    GLenum bufs[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, bufs);
  }
  else {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glUseProgram(phongProg);
  
  const static double pi = acos(0.0) * 2;
  Mat4 ident = Mat4::identityMatrix();
  Mat4 view, viewNorm;
  Mat4::lookAtMatrix(eye, lookat, Vec3(0, 1, 0), view, viewNorm);
  Mat4 proj = Mat4::perspectiveMatrix(pi * 0.5, 1.333333f, 0.1f, 1000.0f);

  Mat4 mvp = proj * view;

  glUniformMatrix4fv(phongProgModelViewMat, 1, GL_FALSE, reinterpret_cast<float*>(&view));
  glUniformMatrix4fv(phongProgMvpMat, 1, GL_FALSE, reinterpret_cast<float*>(&mvp));
  glUniformMatrix4fv(phongProgNormalMat, 1, GL_FALSE, reinterpret_cast<float*>(&ident));

  // Lighting uniforms
  static GLfloat lAmb[3] = {1.0f, 1.0f, 1.0f};
  static GLfloat lPos[3] = {0.0f, 0.0f, 0.5f};
  static GLfloat lDif[3] = {1.0f, 1.0f, 1.0f};
  static GLfloat lSpc[3] = {0.3f, 0.3f, 0.3f};
  static GLfloat kAmb[3] = {0.2f, 0.1f, 0.0f};
  static GLfloat kDif[3] = {0.6f, 0.2f, 0.1f};
  static GLfloat kSpc[3] = {0.0f, 0.0f, 0.0f};
  static GLfloat kShn = 0.0f;
  glUniform3fv(phongProgLAmb, 1, lAmb);
  glUniform3fv(phongProgLPos, 1, lPos);
  glUniform3fv(phongProgLDif, 1, lDif);
  glUniform3fv(phongProgLSpc, 1, lSpc);
  glUniform3fv(phongProgKAmb, 1, kAmb);
  glUniform3fv(phongProgKDif, 1, kDif);
  glUniform3fv(phongProgKSpc, 1, kSpc);
  glUniform1f(phongProgKShn, kShn);

  if (ssao) {
    glUniform1f(phongProgDoSSAO, 1.0f);
  }
  else {
    glUniform1f(phongProgDoSSAO, 0.0f);
  }

  // Set up vertex attributes
  glEnableVertexAttribArray(phongProgPosAttrib);
  glEnableVertexAttribArray(phongProgNormAttrib);
  
  glBindBuffer(GL_ARRAY_BUFFER, vertexDataBuf);
  glVertexAttribPointer(phongProgPosAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));
  glVertexAttribPointer(phongProgNormAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));

  glDrawArrays(GL_TRIANGLES, 0, faceIndexCount);

  static GLfloat floorKAmb[3] = {1.0f, 1.0f, 1.0f};
  static GLfloat floorKDif[3] = {1.0f, 1.0f, 1.0f};
  static GLfloat floorKSpc[3] = {1.0f, 1.0f, 1.0f};
  static GLfloat floorKShn = 2.0f;
  glUniform3fv(phongProgKAmb, 1, floorKAmb);
  glUniform3fv(phongProgKDif, 1, floorKDif);
  glUniform3fv(phongProgKSpc, 1, floorKSpc);
  glUniform1f(phongProgKShn, floorKShn);

  // Now draw the floor
  glBindBuffer(GL_ARRAY_BUFFER, floorBuf);
  glVertexAttribPointer(phongProgPosAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));
  glVertexAttribPointer(phongProgNormAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));

  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void doSSAO()
{
  // All these were calculated offline
  const static GLfloat offsets[48] = { -0.030026, -0.091874, 0.025644, 0.005745, -0.060426, 0.083852,
      0.110698, -0.025912, 0.009211, -0.066202, 0.109803, 0.029828, 0.005172, 0.112933, 0.107859,
      0.098871, -0.094032, 0.129172, -0.116010, -0.168980, 0.096531, 0.232979, 0.061169, 0.126916,
      -0.243828, -0.177320, 0.121368, 0.079705, -0.237290, 0.292208, -0.333035, 0.151770, 0.264504,
      -0.027741, -0.338065, 0.401220, 0.421871, -0.422317, 0.105886, -0.619888, -0.288012, 0.120912,
      -0.485098, 0.605901, 0.142069, -0.783585, 0.276209, 0.321887 };
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  glDrawBuffer(GL_COLOR_ATTACHMENT0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, aoTexture, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0);
  // Detach any depth textures
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(aoProg);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, depthTexture);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, normalTexture);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, randomTexture);
  glActiveTexture(GL_TEXTURE0);
  
  static const double pi = acos(0.0) * 0.5;
  Mat4 projMat = Mat4::perspectiveMatrix(pi * 0.5, 1.333333f, 0.1f, 1000.0f);
  Mat4 invProjMat = Mat4::perspectiveInvMatrix(pi * 0.5, 1.3333333f, 0.1f, 1000.0f);

  glUniform1i(aoProgDepthTexture, 0);
  glUniform1i(aoProgNormTexture, 1);
  glUniform1i(aoProgRandomTexture, 2);
  glUniform2f(aoProgRandomTexCoordScale, wWidth / 4, wHeight / 4);
  glUniformMatrix4fv(aoProgProjMat, 1, GL_FALSE, reinterpret_cast<GLfloat*>(&projMat));
  glUniformMatrix4fv(aoProgInvProjMat, 1, GL_FALSE, reinterpret_cast<GLfloat*>(&invProjMat));
  glUniform3fv(aoProgSampleOffsets, 16, offsets);
  glUniform1f(aoProgSampleRadius, depthDiscontinuityRadius);

  static const float verts[8] = { 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f };

  glEnableVertexAttribArray(aoProgPosAttrib);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glVertexAttribPointer(aoProgPosAttrib, 2, GL_FLOAT, GL_FALSE, 0, verts);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void doBlur()
{
  // Actually render to the screen
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(blurProg);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, aoTexture);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, colorTexture);
  glActiveTexture(GL_TEXTURE0);

  glUniform1i(blurProgAoTexture, 0);
  glUniform1i(blurProgColorTexture, 1);

  glUniform2f(blurProgInvRes, 1.0f / wWidth, 1.0f / wHeight);

  static const float verts[8] = { 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f };

  glEnableVertexAttribArray(blurProgPosAttrib);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glVertexAttribPointer(blurProgPosAttrib, 2, GL_FLOAT, GL_FALSE, 0, verts);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}