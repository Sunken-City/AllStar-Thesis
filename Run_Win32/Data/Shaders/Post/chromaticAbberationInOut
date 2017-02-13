#version 410 core

uniform sampler2D gDiffuseTexture;
uniform sampler2D gNormalTexture; //gDepthTexture
uniform float gTime;
uniform float gEffectTime;

in vec2 passUV0;

out vec4 outColor;

void main(void)
{
  float animationLength = 1.0f;
  float aberrationAmount = clamp((gTime - gEffectTime) * 8.0f, -1.57079633f, 1.57079633f);
  vec3 aberration = vec3(-0.01f * cos(aberrationAmount), 0.0f * cos(aberrationAmount), 0.01f * cos(aberrationAmount));

  outColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);

  outColor.r =  texture(gDiffuseTexture, vec2(passUV0.x + aberration.x, passUV0.y)).r;
  outColor.g =  texture(gDiffuseTexture, vec2(passUV0.x + aberration.y, passUV0.y)).g;
  outColor.b =  texture(gDiffuseTexture, vec2(passUV0.x + aberration.z, passUV0.y)).b;
}
