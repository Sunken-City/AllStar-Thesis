// FRAGMENT SHADER
#version 410 core

//UNIFORMS/////////////////////////////////////////////////////////////////////
uniform sampler2D gDiffuseTexture;
uniform sampler2D gEmissiveTexture;
uniform int gRecolorMode;
uniform float PaletteOffset;

//INPUTS/////////////////////////////////////////////////////////////////////
in vec4 passColor;
in vec2 passUV;

//OUTPUTS/////////////////////////////////////////////////////////////////////
out vec4 fragmentColor;

//MAIN/////////////////////////////////////////////////////////////////////
void main()
{
  vec4 diffuseColor = texture(gDiffuseTexture, passUV);
  float xPalettePosition = diffuseColor.r;
  fragmentColor = passColor * texture(gEmissiveTexture, vec2(xPalettePosition, PaletteOffset));
  fragmentColor.a = diffuseColor.a;
}
