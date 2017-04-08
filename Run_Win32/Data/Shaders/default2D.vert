// VERTEX SHADER
#version 410 core

//UNIFORMS/////////////////////////////////////////////////////////////////////
uniform mat4 gModel;
uniform mat4 gView;
uniform mat4 gProj;
//uniform vec3 gWarpPosition = vec3(20000.0f, 20000.0f, 0.0f); layout(location=5) 
uniform vec2 gWarpPositions[16] =
{vec2(20000.0f, 20000.0f), vec2(20000.0f, 20000.0f), vec2(20000.0f, 20000.0f), vec2(20000.0f, 20000.0f),
 vec2(20000.0f, 20000.0f), vec2(20000.0f, 20000.0f), vec2(20000.0f, 20000.0f), vec2(20000.0f, 20000.0f),
 vec2(20000.0f, 20000.0f), vec2(20000.0f, 20000.0f), vec2(20000.0f, 20000.0f), vec2(20000.0f, 20000.0f),
 vec2(20000.0f, 20000.0f), vec2(20000.0f, 20000.0f), vec2(20000.0f, 20000.0f), vec2(20000.0f, 20000.0f)};
uniform float gVortexRadii[16] =
{1, 1, 1, 1,
1, 1, 1, 1,
1, 1, 1, 1,
1, 1, 1, 1};

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
    vec2 warpPosition = gWarpPositions[i];
    float distanceToPosition = length(position - warpPosition);
    position = mix(warpPosition, position, clamp((distanceToPosition / gVortexRadii[i]), 0.0f, 1.0f));
  }

  // gl_Position is always a vec4 - clip space vector
  //gl_Position = vec4(inPosition, 0, 1) * mvp;
  gl_Position = vec4(position, 0, 1) * gView * gProj;
}
