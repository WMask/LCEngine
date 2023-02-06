struct In
{
    float2 position: POSITION;
    half4 color: COLOR;
};

struct Out
{
    float4 position: SV_POSITION;
    nointerpolation half4 color: COLOR;
};

cbuffer Buffer0: register(b0)
{
    float4x4 projectionMtx;
}

void main(in In i, out Out o)
{
    o.position = mul(float4(i.position, 0, 1), projectionMtx);
    o.color = i.color;
}
