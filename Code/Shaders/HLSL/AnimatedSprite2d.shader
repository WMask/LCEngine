
cbuffer VS_PROJ_BUFFER : register(b0)
{
	float4x4 mProj;
};

cbuffer VS_TRANS_BUFFER : register(b1)
{
	float4x4 mTrans;
};

cbuffer VS_COLORS_BUFFER : register(b2)
{
	float4 vColors[4];
};

cbuffer VS_ANIM_BUFFER : register(b3)
{
	float4 vAnim;
};

struct VOut
{
	float4 vPosition : SV_POSITION;
	float4 vColor : COLOR;
	float2 vCoord : TEXCOORD;
};

VOut VShader(float4 vPosition : POSITION, float2 vCoord : TEXCOORD, uint iIndex : INDEX)
{
	VOut output;

	output.vPosition = mul(vPosition, mul(mTrans, mProj));
	output.vColor = vColors[iIndex];
	output.vCoord = float2(vCoord.x * vAnim.x + vAnim.z, vCoord.y * vAnim.y + vAnim.w);

	return output;
}

Texture2D tex2D;

SamplerState linearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4 PShader(float4 vPosition : SV_POSITION, float4 vColor : COLOR, float2 vCoord : TEXCOORD) : SV_TARGET
{
	return tex2D.Sample(linearSampler, vCoord) * vColor;
}
