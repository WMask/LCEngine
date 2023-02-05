struct In {
  float2 pos: POSITION;
};

struct Out {
  float4 pos: SV_POSITION;
};

cbuffer Cb0: register(b0) {
  float4x4 m;
}

void main(in In i, out Out o) {
  o.pos = mul(float4(i.pos, 0, 1), m);
}
