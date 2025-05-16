cbuffer Camera : register(b0)
{
    matrix matWorld;
    matrix matView;
    matrix matProj;
}

struct VSInput
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

PSInput main(VSInput input)
{
    PSInput output;
    float4 worldPos = mul(float4(input.pos, 1.0f), matWorld);
    float4 viewPos = mul(worldPos, matView);
    output.pos = mul(viewPos, matProj);
    
    output.color = input.color;
    output.uv = input.uv;
    return output;
}
