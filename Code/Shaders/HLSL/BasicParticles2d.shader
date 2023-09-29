
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
    float2 vCoord : TEXCOORD;
    float4 vTint : COLOR;
};

VOut VShader(float4 vPosition : POSITION0, float4 vUV : POSITION1, int iAnimType : BLENDINDICES)
{
    VOut output;
	float4x4 mWVP = mul(mTrans, mul(mView, mProj));
  
    float parTimeOffset = vPosition.w;
    float parSpeed = vParticleSettings.x;
    float parLifetime = vParticleSettings.y;
    float parMovementRadius = vParticleSettings.z;
    float parTime = vParticleSettings.w;
 
    float u = vUV.x;
    float v = vUV.y;
    float fadeInRate = vUV.z;
    float fadeOutRate = vUV.w;

    // animation
    float2 vPos2d = float2(vPosition.x, vPosition.y);

    switch (iAnimType)
    {
        // horizontal
        case 0: vPos2d += float2(parMovementRadius * parSpeed * cos(parTime), 0.0f); break;
        // vertical
        case 1: vPos2d += float2(0.0f, parMovementRadius * parSpeed * sin(parTime)); break;
        // cw
        case 2: vPos2d += float2(parMovementRadius * parSpeed * cos(parTime), parMovementRadius * parSpeed * sin(parTime)); break;
        // ccw
        case 3: vPos2d += float2(parMovementRadius * parSpeed * cos(-parTime), parMovementRadius * parSpeed * sin(-parTime)); break;
    }

    // opacity
    parTime += parTimeOffset;
    float fadeIn = parLifetime * fadeInRate;
    float fadeOut = parLifetime * fadeOutRate;
    float4 vTint = vGlobalTint;
    if (parLifetime > 0.0f)
    {
        float time = parTime % parLifetime;
        if (time < fadeIn && (fadeInRate > 0.0f))
        {
            vTint.a = sin(time / fadeIn);
        }
        else if (((parLifetime - time) < fadeOut) && (fadeOutRate > 0.0f))
        {
            vTint.a = cos(fadeOut - (parLifetime - time));
        }
    }

    output.vPosition = mul(float4(vPos2d.x, vPos2d.y, vPosition.z, 1.0f), mWVP);
    output.vCoord = float2(u, v);
    output.vTint = vTint;

	return output;
}

Texture2D tex2D;

SamplerState linearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4 PShader(float4 vPosition : SV_POSITION, float2 vCoord : TEXCOORD, float4 vTint : COLOR) : SV_TARGET
{
    return tex2D.Sample(linearSampler, vCoord) * vTint;
}
