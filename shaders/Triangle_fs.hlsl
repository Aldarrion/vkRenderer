SamplerState PointSampler : register(s0, space0);
Texture2D AllTextures[] : register(t0, space1);

struct ps_in
{
    float4 Pos : SV_POSITION;
    float3 Color : COLOR;
    float2 UV : TEXCOORD0;
};

float4 main(ps_in input)
{
    float4 outColor = float4(input.Color, 1);
    outColor = outColor * AllTextures[0].Sample(PointSampler, input.UV);
    return outColor;
}
