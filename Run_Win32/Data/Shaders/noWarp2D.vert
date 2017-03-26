#version 410 core

uniform mat4 gModel;
uniform mat4 gView;
uniform mat4 gProj;
in vec2 inPosition;
in vec4 inColor;
in vec2 inUV0;
out vec4 passColor;
out vec2 passUV;

void main()
{
    mat4 mvp = gModel * gView * gProj;
    passUV = inUV0;
    passColor = inColor;
    gl_Position = vec4(inPosition, 0, 1) * mvp;
}
