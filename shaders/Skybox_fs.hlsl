struct BindingUBO
{
    uint4 SRV[2]; // TODO use constant
};

#define BindingIdx(x) Bindings.SRV[x >> 2][x & 3]

TextureCube AllTexturesCube[] : register(t0, space1);
Texture2D AllTextures2D[] : register(t0, space1);
ConstantBuffer<BindingUBO> Bindings : register(b0, space2);

SamplerState SamplerSkybox : register(s32, space0);

struct ps_in
{
    float4 SkyboxCoord : TEXCOORD0;
};

float4 main(ps_in input)
{
    float4 col = AllTexturesCube[BindingIdx(0)].Sample(SamplerSkybox, input.SkyboxCoord.xyz);

    float4 c2 = AllTextures2D[2].Sample(SamplerSkybox, input.SkyboxCoord.xy);

    return c2;
}
