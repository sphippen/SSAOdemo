varying vec2 texCoord;

uniform sampler2D depthTex;
uniform sampler2D normTex;
uniform sampler2D randomTex;
uniform vec2 randomTexCoordScale;

uniform mat4 projMat;
uniform mat4 invProjMat;

uniform vec3 sampleOffsets[16];

uniform float sampleRadius;

void main()
{
  // Holds an occlusion factor for this fragment, to be output at the end
  float occlusion = 0.0;

  // Construct a position for the rendered fragment
  float depth = texture(depthTex, texCoord).r;
  vec3 screenPosition = vec3(texCoord, depth);
  // Go from [0, 1] to [-1, 1]
  vec3 clipPosition = (2.0 * screenPosition) - vec3(1.0);

  // Undo the projection to get back to view coordinates
  vec4 viewPosition = invProjMat * vec4(clipPosition, 1.0);
  viewPosition /= viewPosition.w;

  vec3 normal = texture(normTex, texCoord).xyz;
  // Scale and bias
  normal = (2.0 * normal) - vec3(1.0);
  normal = normalize(normal);

  // Construct our rotation matrix (used to transform sample offsets) based on a random vector lookup
  vec3 randomVector = texture(randomTex, texCoord * randomTexCoordScale).xyz;
  // Go from [0, 1] to [-1, 1], normalizing along the way
  randomVector = normalize((2.0 * randomVector) - vec3(1.0));

  vec3 tangent = normalize(randomVector - normal * dot(normal, randomVector));
  vec3 bitangent = cross(normal, tangent);

  mat3 orientMat = mat3(tangent, bitangent, normal);

  for (int i = 0; i < 16; i++) {
    // Construct our view-space location to sample
    vec3 testSample = (orientMat * sampleOffsets[i]) * sampleRadius + viewPosition.xyz;

    // Transform the sample location into clip coords for depth comparison
    vec4 clipSample = projMat * vec4(testSample, 1.0);
    clipSample.xy /= clipSample.w;
    // Scale and bias to screen coords, [-1, 1] -> [0, 1]
    vec2 screenSample = (clipSample.xy + vec2(1.0)) * 0.5;

    float lookupDepth = texture(depthTex, screenSample).x;
    float rangeCheck = abs(lookupDepth - depth) > sampleRadius ? 0.0 : 1.0;
    occlusion += (lookupDepth < depth ? 1.0 : 0.0) * rangeCheck;
  }

  // Normalize occlusion factor
  occlusion /= 16;

  // Subtract from 1 to give a direct scale factor for lighting
  occlusion = 1.0 - occlusion;

  gl_FragColor = vec4(occlusion, occlusion, occlusion, 1.0);
}
