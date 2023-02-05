struct In {
  float4 pos: SV_POSITION;
  nointerpolation half4 clr: COLOR;
  half cv: COVERAGE;
};

struct Out {
  half4 clr: SV_TARGET0;
};

Out main(in In i) {
  Out o;
  o.clr = (1.0 * i.cv) * i.clr;
  return o;
}
