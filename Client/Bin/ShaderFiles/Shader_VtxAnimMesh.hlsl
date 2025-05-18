
// 241107 추가
#include "../../../EngineSDK/hlsl/EngineShaderDefine.hlsli"


// 241016

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_BoneMatrices[512];

Texture2D g_DiffuseTexture;


Texture2D g_NoiseTexture;
float g_NoiseMaxTime;
float g_NoiseCurTime;
bool g_NoiseStart;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    uint4 vBlendIndices : BLENDINDEX;
    float4 vBlendWeights : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

/* 1. 정점위치에 대한 기초적인 변환과정을 수행한다.(월드변환, 뷰변환, ) */

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
	float			fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x + g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y + 
        g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

    vector vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vProjPos = Out.vPosition;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vWorld : SV_TARGET3;  
};

/* PixelShader */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.1f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;

	/* -1.f ~ 1.f -> 0.f ~ 1.f */
    // 0~1사이의 노말값을 -1~1로 정규화 해준다.
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
        
    // 투영행렬을 곱한 ClipSpace상의 W에는 아직 ViewZ값이 있고 장치에서 이 W값으로 나누게 되면 NDC로 바뀐다.
    // 현재는 장치가 W나누기를 수행한 NDC상의 좌표를 가지고 있다.
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
    
	Out.vWorld = In.vWorldPos;

    return Out;
}

struct PS_OUT_NONPICKED
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};

/* PixelShader */
PS_OUT_NONPICKED PS_MAIN_NONPICKED(PS_IN In)
{
    PS_OUT_NONPICKED Out = (PS_OUT_NONPICKED) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.1f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;

	/* -1.f ~ 1.f -> 0.f ~ 1.f */
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
	
    return Out;
}

struct PS_OUT_LIGHTDEPTH
{
    float4 vLightDepth : SV_TARGET0;
};

PS_OUT_LIGHTDEPTH PS_MAIN_LIGHTDEPTH(PS_IN In)
{
    PS_OUT_LIGHTDEPTH Out = (PS_OUT_LIGHTDEPTH) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
    Out.vLightDepth = float4(In.vProjPos.w / 1000.f, 0.f, 0.f, 0.f);
    
    return Out;
}


float g_NoiseStrength = 0.3f;
float g_BurnGlowIntensity = 0.4f;

PS_OUT PS_MAIN_BOSS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    // 보스 디퓨즈 색상 샘플링
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    // 알파값이 0.1이라하면 discard (폐기)
    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
    // 그대로 대입한다.
    Out.vDiffuse = vMtrlDiffuse;
    
    // 디졸브 시작한 경우
    if (g_NoiseStart)
    {
        // fRatio : 0 (시작) -> 1 (완료) 사이의 진행 비율
        float fRatio = (g_NoiseCurTime / g_NoiseMaxTime);
    
        // 노이즈 텍스처에서 값을 샘플링
        vector vNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord);
    
        // UV 왜곡 계산: RGB 채널 중 R, G 값을 사용하여 UV 좌표를 왜곡
        float2 UVDistortion = (vNoise.rg - 0.5f) * g_NoiseStrength * fRatio;
        float2 DistortedUV = clamp(In.vTexcoord + UVDistortion, 0.f, 1.f);
            
        // 왜곡된 UV로 다시 노이즈 샘플링
        vector vDistoredNoise = g_NoiseTexture.Sample(LinearSampler, DistortedUV);
    
        // BurnAlpha는 디졸브 마스크 역할을 함
        float BurnAlpha = vDistoredNoise.r;
    
        // 디졸브 진행중인 픽셀 제거
        if (BurnAlpha < fRatio)
        {
            discard;
        }
        // 타오르는 경계 영역을 생성 (0.02의 여유를 줘서 Glow 효과를 부여)
        // BurnAlpha가 디졸브할 비율을 말한다.
        // 빨갛게 타오르는 듯한 부분의 영역을 넓히기 위해서 +- 0.02
        else if (BurnAlpha >= fRatio - 0.02f && BurnAlpha <= fRatio + 0.02f)
        {
        // 빨간 영역: 빛나는 효과 추가
        // GlowFactor를 사용하여 매끄러운 효과를 부여
            float GlowFactor = smoothstep(fRatio - 0.02f, fRatio + 0.02f, BurnAlpha);
        
            Out.vDiffuse.r = 1.0f + GlowFactor * 2.0f; // 빨강 색상 강조
            Out.vDiffuse.g = Out.vDiffuse.r * 0.2f; // 약간의 주황색 톤 추가
            Out.vDiffuse.b = Out.vDiffuse.r * 0.1f;

            // 발광 효과 강조 (강한 Glow)
            Out.vDiffuse.rgb *= GlowFactor * g_BurnGlowIntensity;

            return Out;
        }
    
        // 경계보다 조금 더 넓은 영역에서 부드러운 Glow 효과 적용
        float GlowFactor = smoothstep(fRatio - 0.05f, fRatio + 0.05f, BurnAlpha);
    
        // 기본 색상에 GlowFactor를 추가하여 빛나는 효과 부여
        Out.vDiffuse.rgb += GlowFactor * g_BurnGlowIntensity;
    
        // 전체적으로 Dissolve 비율에 따라 점점 사라지도록 함
        Out.vDiffuse.rgb *= lerp(1.f, 0.f, fRatio);
    }
    
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
    Out.vWorld = In.vWorldPos;
    return Out;
}


// VS_MAIN_MONSTER_ONLY
VS_OUT VS_MAIN_MONSTER_ONLY(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x + g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
        g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

    vector vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vProjPos = Out.vPosition;

    return Out;
}

struct PS_OUT_OUTER
{
    float4 vOuter : SV_TARGET0;
};

// 크기 2인 상태 빨간색으로 칠한다.
PS_OUT_OUTER PS_MAIN_MONSTER_ONLY(PS_IN In)
{
    PS_OUT_OUTER Out = (PS_OUT_OUTER) 0;

    vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vColor.a < 0.1f)
        discard;

    Out.vOuter = vColor;
    Out.vOuter.r = 1.0f;
    Out.vOuter.g = 0.f;
    Out.vOuter.b = 0.f;
    Out.vOuter.a = 1.f;
    
    return Out;
}


// 241228 기준으로 아래 PS 사용한다.


struct PS_OUT_OUTER_PRE
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vWorld : SV_TARGET3;
    float4 vOuterPre : SV_TARGET4; // 빨간색 외곽선 RTV
};

// 크기 1인 상태 초록색으로 칠한다.
PS_OUT_OUTER_PRE PS_MAIN_OUTER_PRE(PS_IN In)
{
    PS_OUT_OUTER_PRE Out = (PS_OUT_OUTER_PRE) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.1f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
    Out.vWorld = In.vWorldPos;
    Out.vOuterPre = float4(0.f, 1.f, 0.f, 1.f); // 빨간 외곽선을 그리기 전 초록색으로 칠하기 위한 작업 SV_TARGET4에 초록색으로 칠한다.
    return Out;
}


technique11 DefaultTechnique
{
	
    pass DefaultPass
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDefault, 0);
        SetBlendState(BSDefault, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass NonPicked
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDefault, 0);
        SetBlendState(BSDefault, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NONPICKED();
    }
    
    pass LightDepth
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDefault, 0);
        SetBlendState(BSDefault, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHTDEPTH();
    }

    pass BossDead
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDefault, 0);
        SetBlendState(BSDefault, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BOSS();
    }
    
    // 4
    pass MonsterOnly
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDefault, 0);
        SetBlendState(BSDefault, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_MONSTER_ONLY();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MONSTER_ONLY();
    }

// 5
    pass OuterPre
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDefault, 0);
        SetBlendState(BSDefault, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_OUTER_PRE();
    }
}
