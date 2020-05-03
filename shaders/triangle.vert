#version 450

vec3 TriangleVerts[3] =
{
    vec3(-0.5, 0.5, 1),
    vec3(0.5, 0.5, 1),
    vec3(0, -0.5, 1)
};

void main()
{
    gl_Position = vec4(TriangleVerts[gl_VertexIndex], 1);
}
