varying vec2 texCoord;

uniform sampler2D aoTex;
uniform sampler2D colorTex;
uniform vec2 invRes;

void main()
{
  float scaleFactor = 0.0;

  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      vec2 offset = invRes * vec2(x, y);
      vec2 realTexCoord = texCoord + offset;
      scaleFactor += texture(aoTex, realTexCoord).r;
    }
  }

  // Average out the accumulated scaleFactor
  scaleFactor /= 16.0;

  // Use the blurred occlusion value to scale the input color texture
  vec3 pixValue = scaleFactor * texture(colorTex, texCoord).rgb;

  gl_FragColor = vec4(pixValue, 1.0);
}
