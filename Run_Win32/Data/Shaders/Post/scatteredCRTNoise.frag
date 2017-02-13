#version 410 core

uniform sampler2D gDiffuseTexture;
uniform sampler2D gNormalTexture; //gDepthTexture
uniform float gTime;
uniform float gPixelationFactor;

in vec2 passUV0;
in vec3 passPosition;

out vec4 outColor;

void main(void)
{
  vec2 uv = passUV0 * vec2(1600, 900);
  uv = round(uv / (gPixelationFactor * (1.0f - sin((gTime + passUV0.y) * 100.0f) / 1.0f))) * (gPixelationFactor * (1.0f - sin((gTime + passUV0.y) * 100.0f) / 1.0f));
  uv /= vec2(1600, 900);

  outColor = texture(gDiffuseTexture, uv);
}
