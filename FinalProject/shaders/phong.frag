varying vec3 normalV;
varying vec3 positionV;

varying vec3 normTest;

uniform vec3 lAmbient;

uniform vec3 lPosition;
uniform vec3 lDiffuse;
uniform vec3 lSpecular;

uniform vec3 kAmbient;
uniform vec3 kDiffuse;
uniform vec3 kSpecular;
uniform float kShininess;

uniform float doSSAO;

void main()
{
  vec3 normal = normalize(normalV);
  vec3 ambient = lAmbient * kAmbient;
  float diffuseC = dot(normal, normalize(lPosition - positionV));
  vec3 diffuse = max(0.0, diffuseC) * kDiffuse * lDiffuse;

  vec3 specular = vec3(0.0);
  float specPreExp = 0.0;
  if (diffuseC >= 0.0) {
    vec3 reflection = reflect(normalize(positionV - lPosition), normal);
    specPreExp = dot(normalize(reflection), normalize(-positionV));
    specular = max(0.0, pow(specPreExp, kShininess)) * lSpecular * kSpecular;
  }

  if (doSSAO > 0.5) {
    gl_FragData[0] = vec4(ambient + diffuse + specular, 1.0);
    // Scale and bias normal from [-1, 1] to [0, 1]
    vec3 scaledNormal = 0.5 * (normal + vec3(1.0));
    gl_FragData[1] = vec4(scaledNormal, 0.0);
  }
  else {
    gl_FragData[0] = vec4(ambient + diffuse + specular, 1.0);
  }
}
