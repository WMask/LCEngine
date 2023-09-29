
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

cbuffer VS_ANIM_BUFFER : register(b5)
{
    float4 vParticleSettings;
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

VOut VShader(float4 vPosition : POSITION, float2 vUV : TEXCOORD, int iAnimType : BLENDINDICES)
{
    VOut output;
	float4x4 mWVP = mul(mTrans, mul(mView, mProj));
    
    float parSpeed = vParticleSettings.x;
    float parMovementRadius = vParticleSettings.y;
    float parLifetime = vParticleSettings.z;
    float parTime = vParticleSettings.w;

    float2 pos2D = float2(vPosition.x, vPosition.y);

    switch (iAnimType)
    {
        // horizontal
        case 0: pos2D += float2(parMovementRadius * parSpeed * cos(parTime), 0.0f); break;
        // vertical
        case 1: pos2D += float2(0.0f, parMovementRadius * parSpeed * sin(parTime)); break;
        // cw
        case 2: pos2D += float2(parMovementRadius * parSpeed * cos(parTime), parMovementRadius * parSpeed * sin(parTime)); break;
        // ccw
        case 3: pos2D += float2(parMovementRadius * parSpeed * cos(-parTime), parMovementRadius * parSpeed * sin(-parTime)); break;
    }

    output.vPosition = mul(float4(pos2D.x, pos2D.y, vPosition.z, 1.0f), mWVP);
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
