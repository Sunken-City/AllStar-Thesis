// VERTEX SHADER
#version 410 core

//UNIFORMS/////////////////////////////////////////////////////////////////////
uniform mat4 gModel;
uniform mat4 gView;
uniform mat4 gProj;
uniform vec3 gWarpPosition;

//INPUTS/////////////////////////////////////////////////////////////////////
in vec2 inPosition;
in vec2 inUV0;
in vec4 inColor;

//OUTPUTS/////////////////////////////////////////////////////////////////////
out vec2 passUV;
out vec4 passColor;

//MAIN/////////////////////////////////////////////////////////////////////
void main()
{
  mat4 mvp = gModel * gView * gProj;

  passUV = inUV0;
  passColor = inColor;

  vec2 position = (vec4(inPosition, 0, 1) * gModel).xy;
  vec2 warpPosition = gWarpPosition.xy;
  float distanceToPosition = length(position - warpPosition);
  vec2 finalPosition = mix(warpPosition, position, clamp((distanceToPosition - 0.5f) / 3.0f, 0.0f, 1.0f));

  // gl_Position is always a vec4 - clip space vector
  //gl_Position = vec4(inPosition, 0, 1) * mvp;
  gl_Position = vec4(finalPosition, 0, 1) * gView * gProj;
}
