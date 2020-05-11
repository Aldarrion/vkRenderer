#version 450

#define VKR_PI 3.1415926535897932384626433832795

layout(location = 0) in vec4 InPos;
layout(location = 1) in vec4 InCol;

layout(location = 0) out vec3 Color;


void main()
{
    vec2 dimensions = vec2(1280, 720);

    gl_Position.x = (InPos.x / dimensions.x) * 2 - 1;
    gl_Position.y = (InPos.y / dimensions.y) * 2 - 1;
    gl_Position.z = 1;

    Color = vec3(0.9, 0.2, 0.2);
}
