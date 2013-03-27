attribute vec3 positionIn;
attribute vec3 normalIn;

uniform mat4 modelViewMat;
uniform mat4 modelViewProjMat;
uniform mat4 normalMat;

varying vec3 normalV;
varying vec3 positionV;
varying vec3 normTest;

void main()
{
  normTest = normalIn;
  normalV = vec3(normalMat * vec4(normalIn, 0.0));
  positionV = vec3(modelViewMat * vec4(positionIn, 1.0));
  gl_Position = modelViewProjMat * vec4(positionIn, 1.0);
}
