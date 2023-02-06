Texture2D pattern: register(t0);
SamplerState patternSampler: register(s0);

cbuffer Buffer0: register(b0)
{
    float opacity;
}

struct In
{
    float4 position: SV_POSITION;
    float2 uv0: TEXCOORD0;
};

struct Out
{
    half4 color: SV_TARGET0;
};

Out main(in In i)
{
    half4 paint = pattern.Sample(patternSampler, i.uv0);
    half opacity_ = opacity;

    Out o;
    o.color = opacity_ * paint;

    return o;
}
