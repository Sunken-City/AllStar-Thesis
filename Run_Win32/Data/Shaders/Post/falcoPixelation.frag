#version 410 core

uniform sampler2D gDiffuseTexture;
uniform sampler2D gNormalTexture; //gDepthTexture
uniform float gPixelationFactor;

in vec2 passUV0;
in vec3 passPosition;

out vec4 outColor;

void main(void)
{
  vec2 uv = passUV0 * vec2(1600, 900);
  uv.x = round(uv.x / gPixelationFactor) * gPixelationFactor;
  uv.y = round(uv.y / (gPixelationFactor * (uv.y / 900))) * (gPixelationFactor * (uv.y / 900));
  uv /= vec2(1600, 900);

  outColor = texture(gDiffuseTexture, uv);
}
