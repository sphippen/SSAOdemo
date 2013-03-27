attribute vec2 positionIn;

varying vec2 texCoord;

void main()
{
  // Scale and bias [-1, 1] to [0, 1]
  texCoord = (positionIn * vec2(0.5)) + vec2(0.5);
  gl_Position = vec4(positionIn, 0.0, 1.0);
}
