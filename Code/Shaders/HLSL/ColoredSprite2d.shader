
cbuffer VS_PROJ_BUFFER : register(b0)
{
	float4x4 mProj;
};

cbuffer VS_TRANS_BUFFER : register(b1)
{
	float4x4 mTrans;
	float4 vColors[4];
};

struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VOut VShader(float4 position : POSITION, uint index : INDEX)
{
	VOut output;

	output.position = mul(position, mul(mTrans, mProj));
	output.color = vColors[index];

	return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
	return color;
}
