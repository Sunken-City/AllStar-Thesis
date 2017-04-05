// FRAGMENT SHADER
#version 410 core

//UNIFORMS/////////////////////////////////////////////////////////////////////
uniform sampler2D gDiffuseTexture;
uniform float gTime;
uniform float gPercentage;
uniform float gPercentagePerUse;

//INPUTS/////////////////////////////////////////////////////////////////////
in vec4 passColor;
in vec2 passUV;

//OUTPUTS/////////////////////////////////////////////////////////////////////
out vec4 fragmentColor;

//MAIN/////////////////////////////////////////////////////////////////////
void main()
{
  vec4 diffuseColor = texture(gDiffuseTexture, passUV);

  vec2 up = vec2(0.0, -1.0);
  vec2 circleCenter = vec2(0.5, 0.5);
  vec2 dispFromCenterToUV = passUV - circleCenter;
  vec2 normalizedDisp = normalize(dispFromCenterToUV);
  float dotProduct = dot(normalizedDisp, up);
  float angle = acos(dotProduct) * 57.2957795;

  if(passUV.x < 0.5)
  {
    angle = 360 - angle;
  }

  vec4 dampingColor = vec4(0.0f);
  bool isUnderPercentage = (gPercentage * 360) < angle;
  
  if(isUnderPercentage)
  {
    dampingColor = vec4(-0.75f, -0.75f, -0.75f, 0.0f);
  }

  fragmentColor = (passColor * diffuseColor) + dampingColor;

  if(isUnderPercentage && mod((angle / 360.0f), gPercentagePerUse) < 0.005f)
  {
    fragmentColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
  }
}
