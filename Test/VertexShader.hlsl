// Vertex Shader
struct VS_INPUT
{
    float4 Pos : POSITION;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Pos = input.Pos;
    return output;
}

// Pixel Shader
float4 main(VS_OUTPUT input) : SV_TARGET
{
    return float4(1.0f, 0.0f, 0.0f, 1.0f); // Cor vermelha
}