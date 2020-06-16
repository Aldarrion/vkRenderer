
static const float3 TriangleVerts[3] =
{
    float3(0, 0, 50),
    float3(10, 0, 50),
    float3(0, 10, 50)
};

struct vs_out
{
    float4 Pos : SV_POSITION;
    float3 Color : COLOR;
};

struct Vec
{
    float4x4 projection;
};

ConstantBuffer<Vec> Mat : register(b1, space2);

vs_out main(uint vertID : SV_VERTEXID)
{
    vs_out o = vs_out(0);

    o.Pos = float4(TriangleVerts[vertID], 1) * Mat.projection;
    o.Color = float3(0.8, 0.2, 0.1);

    return o;
}