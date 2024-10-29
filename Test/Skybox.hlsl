Texture2D ObjTexture;
SamplerState ObjSamplerState;
TextureCube SkyBox;

cbuffer cbPerObject
{
    float4x4 WVP;
};

struct SKYBOX_VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 texCoord : TEXCOORD;
};

SKYBOX_VS_OUTPUT SKYBOX_VS(float3 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL)
{
    SKYBOX_VS_OUTPUT output = (SKYBOX_VS_OUTPUT) 0;

    output.Pos = mul(float4(inPos, 1.0f), WVP).xyww;

    output.texCoord = inPos;

    return output;
}

float4 SKYBOX_PS(SKYBOX_VS_OUTPUT input) : SV_Target
{
    return SkyBox.Sample(ObjSamplerState, input.texCoord);
}