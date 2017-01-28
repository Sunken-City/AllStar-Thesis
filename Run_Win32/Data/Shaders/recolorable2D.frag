// FRAGMENT SHADER
#version 410 core

//UNIFORMS/////////////////////////////////////////////////////////////////////
uniform sampler2D gDiffuseTexture;
uniform int gRecolorMode;

//INPUTS/////////////////////////////////////////////////////////////////////
in vec4 passColor;
in vec2 passUV;

//OUTPUTS/////////////////////////////////////////////////////////////////////
out vec4 fragmentColor;

//MAIN/////////////////////////////////////////////////////////////////////
void main()
{
  vec4 diffuseColor = texture(gDiffuseTexture, passUV);

  if(gRecolorMode == 1)
  {
    diffuseColor = diffuseColor.rbga;
  }
  else if(gRecolorMode == 2)
  {
    diffuseColor = diffuseColor.grba;
  }
  else if(gRecolorMode == 3)
  {
    diffuseColor = diffuseColor.gbra;
  }
  else if(gRecolorMode == 4)
  {
      diffuseColor = diffuseColor.bgra;
  }
  else if(gRecolorMode == 5)
  {
    diffuseColor = diffuseColor.brga;
  }
  else if(gRecolorMode == 6)
  {
    diffuseColor = vec4(diffuseColor.r * 0.21f, diffuseColor.g * 0.71f, diffuseColor.b * 0.07f, diffuseColor.a);
  }
  else if(gRecolorMode == 7)
  {
    diffuseColor = vec4(1.0f - diffuseColor.r, 1.0f - diffuseColor.g, 1.0f - diffuseColor.b, diffuseColor.a);
  }

  fragmentColor = passColor * diffuseColor;
}
