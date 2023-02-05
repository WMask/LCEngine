struct In {
  float2 pos: POSITION;
  half4 clr: COLOR;
  half cv: COVERAGE;
};

struct Out {
  float4 pos: SV_POSITION;
  nointerpolation half4 clr: COLOR;
  half cv: COVERAGE;
};

cbuffer Cb0: register(b0) {
  float4x4 m;
}

void main(in In i, out Out o) {
  o.pos = mul(float4(i.pos, 0, 1), m);
  o.clr = i.clr;
  o.cv = i.cv;
}
