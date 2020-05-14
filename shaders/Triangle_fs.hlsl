Texture2D Diffuse : register(t0);
SamplerState PointSampler : register(s1);

struct ps_in
{
    float4 Pos : SV_POSITION;
    float3 Color : COLOR;
    float2 UV : TEXCOORD0;
};

float4 main(ps_in input)
{
    float4 outColor = float4(input.Color, 1);
    outColor = outColor * Diffuse.Sample(PointSampler, input.UV);
    return outColor;
}
