#version 450

vec3 TriangleVerts[3] =
{
    vec3(-0.5, 0.5, 0.5),
    vec3(0.5, 0.5, 0.5),
    vec3(0, -0.5, 0.5)
};

vec2 TriangleVertsPx[3] =
{
    vec2(100, 500),
    vec2(500, 500),
    vec2(250, 100)
};

vec3 TriangleColors[3] = 
{
    vec3(1, 0, 0),
    vec3(0, 1, 0),
    vec3(0, 0, 1)
};

vec2 TriangleUVs[3] = 
{
    vec2(0, 1),
    vec2(1, 1),
    vec2(0.5, 0)
};

layout(location = 0) out vec3 Color;
layout(location = 1) out vec2 UV;

void main()
{
    vec2 dimensions = vec2(1280, 720);

    /*vec2 pos = TriangleVertsPx[gl_VertexIndex];

    gl_Position.x = (pos.x / dimensions.x) * 2 - 1;
    gl_Position.y = (pos.y / dimensions.y) * 2 - 1;
    gl_Position.z = 1;*/

    gl_Position = vec4(TriangleVerts[gl_VertexIndex], 1);
    Color = TriangleColors[gl_VertexIndex];
    UV = TriangleUVs[gl_VertexIndex];
}
