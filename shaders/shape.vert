#version 450

#define VKR_PI 3.1415926535897932384626433832795

layout(location = 0) out vec3 Color;

void main()
{
    vec2 dimensions = vec2(1280, 720);
    vec2 center = vec2(200, 200);
    float radius = 100;
    int vertCount = 20;

    int vertInTriangle = gl_VertexIndex % 3;
    int triangleIndex = gl_VertexIndex / 3;
    
    int first = min(vertInTriangle, 1);

    float angle = (2 * VKR_PI / vertCount) * (triangleIndex + vertInTriangle - 1);

    vec2 pos = center + first * radius * vec2(cos(angle), sin(angle));

    gl_Position.x = (pos.x / dimensions.x) * 2 - 1;
    gl_Position.y = (pos.y / dimensions.y) * 2 - 1;
    gl_Position.z = 1;

    Color = vec3(0.9, 0.2, 0.2);
}
