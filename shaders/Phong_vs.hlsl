
static const float3 TriangleVerts[3] =
{
    float3(-100, 100, 0.5),
    float3(100, 100, 0.5),
    float3(0, -100, 0.5)
};

struct vs_out
{
    float4 Pos : SV_POSITION;
    float3 Color : COLOR;
};

struct Vec
{
    float4 v;
};

ConstantBuffer<Vec> TestVec : register(b1, space2);

struct BindingUBO
{
    uint4 SRV[2]; // TODO use constant
};
ConstantBuffer<BindingUBO> Bindings : register(b0, space2);

vs_out main(uint vertID : SV_VERTEXID)
{
    vs_out o = vs_out(0);

    o.Pos = float4(TriangleVerts[vertID], 1);
    o.Color = float3(1, 0, 0);

    o.Color = Bindings.SRV[0].rgb;

    return o;
}