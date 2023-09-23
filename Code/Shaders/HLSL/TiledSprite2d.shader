
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

cbuffer VS_SETTINGS_BUFFER : register(b6)
{
    float4 vGlobalTint;
};

struct VOut
{
    float4 vPosition : SV_POSITION;
    float4 vTint : COLOR;
	float2 vCoord : TEXCOORD;
};

VOut VShader(float4 vPosition : POSITION, float2 vUV : TEXCOORD)
{
	VOut output;
	float4x4 mWVP = mul(mTrans, mul(mView, mProj));

    output.vPosition = mul(vPosition, mWVP);
    output.vTint = vGlobalTint;
	output.vCoord = vUV;

	return output;
}

Texture2D tex2D;

SamplerState linearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4 PShader(float4 vPosition : SV_POSITION, float4 vTint : COLOR, float2 vCoord : TEXCOORD) : SV_TARGET
{
    return tex2D.Sample(linearSampler, vCoord) * vTint;
}
