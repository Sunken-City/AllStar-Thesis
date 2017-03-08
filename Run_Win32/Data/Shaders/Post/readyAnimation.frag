#version 410 core

uniform sampler2D gDiffuseTexture;
uniform sampler2D gNormalTexture; //gTransitionTexture
uniform float gTime;
uniform float gEffectTime;
uniform float gEffectDurationSeconds;
uniform vec4 gWipeColor;

in vec2 passUV0;

out vec4 outColor;

void main(void)
{
  float durationMultiplier = 1.0f / gEffectDurationSeconds;
  float cutoff = clamp((gTime - gEffectTime) * durationMultiplier, 0.0f, 1.0f);
  vec4 transitionColor = texture(gNormalTexture, passUV0);
  vec4 diffuseColor = texture(gDiffuseTexture, passUV0);
  vec4 wipeColor = gWipeColor;

  float blendFactor = 1.0f;
  cutoff = 1.0f - cutoff;
  float distFromCutoff = transitionColor.r - cutoff;
  blendFactor -= (distFromCutoff * 100.0f);
  blendFactor = clamp(blendFactor, 0.0f, 1.0f);

  outColor = mix(wipeColor, diffuseColor, blendFactor);
}
