Texture2D ObjTexture;
SamplerState ObjSamplerState;
TextureCube SkyBox;

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

struct SKYBOX_VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 texCoord : TEXCOORD;
};

cbuffer cbPerObject
{
    float4x4 WVP;
};

VS_OUTPUT vs_main(float4 inPos : POSITION, float4 inColor : COLOR)
{
    VS_OUTPUT output;
    output.Pos = mul(inPos, WVP);
    output.Color = inColor;
    
    return output;
}

// Pixel Shader
float4 ps_main(VS_OUTPUT input) : SV_TARGET
{
    return input.Color;
}

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