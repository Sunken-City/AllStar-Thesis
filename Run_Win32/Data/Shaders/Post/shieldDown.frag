#version 410 core

uniform sampler2D gDiffuseTexture;
uniform sampler2D gNormalTexture; //gDepthTexture
uniform float gTime;
uniform float gEffectTime;

in vec2 passUV0;

out vec4 outColor;

void main(void)
{
  float amount = clamp((gTime - gEffectTime) * 8.0f, -1.57079633f, 1.57079633f);
  float cosAmount = cos(amount);
  vec2 uv = passUV0;

  //Distort UVs
  if(cosAmount > 0.0f)
  {
    uv = passUV0 * vec2(1600, 900);
    uv = round(uv / (16.0f * (1.0f - sin((gTime + passUV0.y) * 100.0f)))) * (16.0f * (1.0f - sin((gTime + passUV0.y) * 100.0f)));
    uv /= vec2(1600, 900);
    uv = mix(passUV0, uv, cosAmount);
  }

  //Chromatic Aberration
  vec3 aberration = vec3(-0.01f * cos(amount), 0.0f * cos(amount), 0.01f * cos(amount));

  outColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
  outColor.r =  texture(gDiffuseTexture, vec2(uv.x + aberration.x, uv.y)).r;
  outColor.g =  texture(gDiffuseTexture, vec2(uv.x + aberration.y, uv.y)).g;
  outColor.b =  texture(gDiffuseTexture, vec2(uv.x + aberration.z, uv.y)).b;
}
