#version 410 core

uniform sampler2D gDiffuseTexture;
uniform sampler2D gNormalTexture; //gTransitionTexture
uniform float gTime;
uniform float gEffectTime;

in vec2 passUV0;

out vec4 outColor;

void main(void)
{
  //float cutoff = sin(clamp((gTime - gEffectTime) * 5.0f, 0.0f, 4.15f)) + 0.1f;
  float cutoff = clamp((gTime - gEffectTime) / 4.0f, 0.0f, 1.0f);
  vec4 transitionColor = texture(gNormalTexture, passUV0);
  vec4 diffuseColor = texture(gDiffuseTexture, passUV0);
  vec4 wipeColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);

  float blendFactor = 1.0f;

  float distFromCutoff = transitionColor.r - cutoff;
  blendFactor -= (distFromCutoff * 100.0f);
  blendFactor = clamp(blendFactor, 0.0f, 1.0f);

  outColor = mix(diffuseColor, wipeColor, blendFactor);
}
