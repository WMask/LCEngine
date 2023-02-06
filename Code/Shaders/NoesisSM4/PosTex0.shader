struct In
{
    float2 position: POSITION;
    float2 uv0: TEXCOORD0;
};

struct Out
{
    float4 position: SV_POSITION;
    float2 uv0: TEXCOORD0;
};

cbuffer Buffer0: register(b0)
{
    float4x4 projectionMtx;
}

void main(in In i, out Out o)
{
    o.position = mul(float4(i.position, 0, 1), projectionMtx);
    o.uv0 = i.uv0;
}
