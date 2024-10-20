struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
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