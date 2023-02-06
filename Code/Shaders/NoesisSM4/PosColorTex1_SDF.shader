struct In
{
    float2 position: POSITION;
    half4 color: COLOR;
    float2 uv1: TEXCOORD1;
};

struct Out
{
    float4 position: SV_POSITION;
    nointerpolation half4 color: COLOR;
    float2 uv1: TEXCOORD1;
    float4 st1: TEXCOORD2;
};

cbuffer Buffer0: register(b0)
{
    float4x4 projectionMtx;
}

cbuffer Buffer1: register(b1)
{
    float2 textureSize;
}

void main(in In i, out Out o)
{
    o.position = mul(float4(i.position, 0, 1), projectionMtx);
    o.color = i.color;
    o.uv1 = i.uv1;
    o.st1 = float4(i.uv1 * textureSize.xy, 1.0 / (3.0 * textureSize.xy));
}
