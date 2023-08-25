
Texture2D tex2D;

SamplerState linearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

cbuffer VS_PROJ_BUFFER : register(b0)
{
	float4x4 mProj;
};

cbuffer VS_TRANS_BUFFER : register(b1)
{
	float4x4 mTrans;
};

struct VOut
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VOut VShader(float4 position : POSITION, float2 uv : TEXCOORD)
{
	VOut output;

	output.position = mul(position, mul(mTrans, mProj));
	output.uv = uv;

	return output;
}

float4 PShader(float4 position : SV_POSITION, float2 uv : TEXCOORD) : SV_TARGET
{
	return tex2D.Sample(linearSampler, uv);
}
