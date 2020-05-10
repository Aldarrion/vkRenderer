#version 450

layout(set = 0, binding = 0) uniform texture2D Diffuse;
layout(set = 0, binding = 1) uniform sampler PointSampler;

layout(location = 0) in vec3 FragmentColor;
layout(location = 1) in vec2 UV;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(FragmentColor, 1);
    outColor = outColor * texture(sampler2D(Diffuse, PointSampler), UV);
}
