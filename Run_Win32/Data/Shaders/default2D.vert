// VERTEX SHADER
#version 410 core

//UNIFORMS/////////////////////////////////////////////////////////////////////
uniform mat4 gModel;
uniform mat4 gView;
uniform mat4 gProj;

struct vortex
{
  vec2 position;
  float radius;
  float padding;
};

uniform vortexInfo
{
  vortex vortices[16];
};

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
  for(int i = 0; i < 16; ++i)
  {
    vec2 warpPosition = vortices[i].position;
    float distanceToPosition = length(position - warpPosition);
    position = mix(warpPosition, position, clamp((distanceToPosition / vortices[i].radius), 0.0f, 1.0f));
  }

  // gl_Position is always a vec4 - clip space vector
  //gl_Position = vec4(inPosition, 0, 1) * mvp;
  gl_Position = vec4(position, 0, 1) * gView * gProj;
}
