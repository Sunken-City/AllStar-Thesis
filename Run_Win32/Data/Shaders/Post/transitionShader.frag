#version 410 core

uniform sampler2D gDiffuseTexture;
uniform sampler2D gNormalTexture; //gTransitionTexture
uniform float gTime;
uniform float gEffectTime;

in vec2 passUV0;

out vec4 outColor;

void main(void)
{
  float cutoff = clamp(gTime - gEffectTime, 0.0f, 1.0f);
  vec4 transitionColor = texture(gNormalTexture, passUV0);

  if(transitionColor.r <= cutoff)
  {
    outColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
  }
  else
  {
    outColor = texture(gDiffuseTexture, passUV0);
  }
}
