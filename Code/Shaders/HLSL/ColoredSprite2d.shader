
cbuffer VS_PROJ_BUFFER : register(b0)
{
	float4x4 mProj;
};

cbuffer VS_VIEW_BUFFER : register(b1)
{
	float4x4 mView;
};

cbuffer VS_TRANS_BUFFER : register(b2)
{
	float4x4 mTrans;
};

cbuffer VS_COLORS_BUFFER : register(b3)
{
	float4 vColors[4];
};

struct VOut
{
	float4 vPosition : SV_POSITION;
	float4 vColor : COLOR;
};

VOut VShader(float4 vPosition : POSITION, uint iIndex : INDEX)
{
	VOut output;
	float4x4 mWVP = mul(mTrans, mul(mView, mProj));

	output.vPosition = mul(vPosition, mWVP);
	output.vColor = vColors[iIndex];

	return output;
}

float4 PShader(float4 vPosition : SV_POSITION, float4 vColor : COLOR) : SV_TARGET
{
	return vColor;
}
