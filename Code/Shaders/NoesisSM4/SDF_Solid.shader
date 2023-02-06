#define SDF_SCALE 7.96875
#define SDF_BIAS 0.50196078431
#define SDF_AA_FACTOR 0.65
#define SDF_BASE_MIN 0.125
#define SDF_BASE_MAX 0.25
#define SDF_BASE_DEV -0.65

Texture2D glyphs: register(t3);
SamplerState glyphsSampler: register(s3);

struct In
{
    float4 position: SV_POSITION;
    nointerpolation half4 color: COLOR;
    float2 uv1: TEXCOORD1;
    float4 st1: TEXCOORD2;
};

struct Out
{
    half4 color: SV_TARGET0;
};

half4 GetCustomPattern(in In i);

Out main(in In i)
{
    half4 paint = i.color;
    half opacity_ = 1.0;

    Out o;
    half4 color = glyphs.Sample(glyphsSampler, i.uv1);
    half distance = SDF_SCALE * (color.r - SDF_BIAS);
    half2 grad = ddx(i.st1.xy);

    half gradLen = (half)length(grad);
    half scale = 1.0 / gradLen;
    half base = SDF_BASE_DEV * (1.0 - (clamp(scale, SDF_BASE_MIN, SDF_BASE_MAX) - SDF_BASE_MIN) / (SDF_BASE_MAX - SDF_BASE_MIN));
    half range = SDF_AA_FACTOR * gradLen;
    half alpha = smoothstep(base - range, base + range, distance);

    o.color = (alpha * opacity_) * paint;

    return o;
}
