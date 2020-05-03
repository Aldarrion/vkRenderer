#version 450

vec3 TriangleVerts[3] =
{
    vec3(-0.5, 0.5, 0.5),
    vec3(0.5, 0.5, 0.5),
    vec3(0, -0.5, 0.5)
};

vec3 TriangleColors[3] = 
{
    vec3(1, 0, 0),
    vec3(0, 1, 0),
    vec3(0, 0, 1)
};

layout(location = 0) out vec3 fragColor;

void main()
{
    gl_Position = vec4(TriangleVerts[gl_VertexIndex], 1);
    fragColor = TriangleColors[gl_VertexIndex];
}
