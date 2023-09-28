
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

cbuffer VS_UV_BUFFER : register(b4)
{
	float4 vUV[4];
};

cbuffer VS_SETTINGS_BUFFER : register(b6)
{
    float4 vGlobalTint;
};

struct VOut
{
	float4 vPosition : SV_POSITION;
    float4 vColor : COLOR0;
    float4 vTint : COLOR1;
	float2 vCoord : TEXCOORD;
};

VOut VShader(float4 vPosition : POSITION, uint iIndex : INDEX)
{
	VOut output;
	float4x4 mWVP = mul(mTrans, mul(mView, mProj));

	output.vPosition = mul(vPosition, mWVP);
    output.vColor = vColors[iIndex];
    output.vTint = vGlobalTint;
	output.vCoord = vUV[iIndex];

	return output;
}

Texture2D tex2D;

SamplerState linearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4 PShader(float4 vPosition : SV_POSITION, float4 vColor : COLOR0, float4 vTint : COLOR1, float2 vCoord : TEXCOORD) : SV_TARGET
{
	float4 texColor = tex2D.Sample(linearSampler, vCoord);
	if (texColor.a == 0.0f)
		return vColor * vTint;
	else
	{
		if (vColor.a == 0.0f)
			return texColor * vTint;
		else
			return texColor * vColor * vTint;
	}
}
