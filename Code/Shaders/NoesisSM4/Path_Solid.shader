struct In
{
    float4 position: SV_POSITION;
    nointerpolation half4 color: COLOR;
};

struct Out
{
    half4 color: SV_TARGET0;
};

Out main(in In i)
{
    half4 paint = i.color;
    half opacity_ = 1.0;

    Out o;
    o.color = opacity_ * paint;

    return o;
}
