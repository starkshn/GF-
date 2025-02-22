



#include "../../../EngineSDK/hlsl/EngineShaderDefine.hlsli"


// 241011 수업 기준 모델 쉐이더 파일

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_DiffuseTexture;

Texture2D g_ColorTexture;
Texture2D g_ColorTestTexture;

Texture2D g_DiffuseCustomTexture;
Texture2D g_NoiseTexture;


float g_MaxLifeTime;
float g_CurLifeTime;


struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;    // DepthTarget에 그리기 위해 하나 더 늘어남(241128)
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    
    // 월드 * 뷰 * 투영 변환 행렬 한 결과값을 vPorjPos에 담는다.
    // 월드 * 뷰 * 투영을 곱하면 ClipSpace로 변경된다.
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
    float4 vDepth : SV_TARGET2; // DepthTarget.
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
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    // vProjPos는 ClipSpace는 w나누기를 수행하지 않은 값이다.
    // ClipSpace에서 W 나누기를 수행하면 비로소 NDC 좌표계로 변경된다.
    // ClipSpace의 W는 ViewSpace의 Z와 같다.
    // x : 0 ~ 1 NDC 좌표, 뷰 스페이스 상의 0 ~ far 값 / 뷰 스페이스 상의 z값 => 0 ~ 1 이 나온다.
    // y : ViewZ를 1000(Far)로 나눈값 -> 포맷이 (0~1사이의 값만 저장할 수 있기 때문에) 결국 ViewZ를 저장.
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
    Out.vWorld = In.vWorldPos;

    return Out;
}

PS_OUT PS_MAIN_TEST_EFFECT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_ColorTexture.Sample(LinearSampler, In.vTexcoord);
    
    float fRatio = g_CurLifeTime / g_MaxLifeTime;
    
    if (vMtrlDiffuse.a < fRatio)
        discard;
    
    vector vColor = g_ColorTestTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vDiffuse.a = vMtrlDiffuse.r;
    
        
    if (Out.vDiffuse.a < fRatio)
        discard;

	/* -1.f ~ 1.f -> 0.f ~ 1.f */
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
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



PS_OUT PS_MAIN_EFFECT_SPHERE_1004(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseCustomTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord);
     
    float fRatio = g_CurLifeTime / g_MaxLifeTime;
    float fTimeOffSet = fRatio * 0.3f;
    vector vAnimatedNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord + float2(fTimeOffSet, fTimeOffSet));
    
    if (vDiffuse.a < 0.1f)
        discard;
    
    Out.vDiffuse.rgb = vDiffuse.rgb * (1.0f + vAnimatedNoise.r * 0.5f); // 밝기 증가
    
    // 알파값 계산
    float fAdjustedAlpha = vDiffuse.a * vAnimatedNoise.r; // 기본 알파값 (텍스처 기반)
    float fBlendFactor = smoothstep(0.0f, 0.8f, 1.0f - fRatio); // 알파 블렌딩 비율 (부드럽게 감소)
    
    // Out.vDiffuse.a = (vDiffuse.a * vAnimatedNoise.r);
    Out.vDiffuse.a = fAdjustedAlpha * fBlendFactor; // 최종 알파값
    
    
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
    Out.vWorld = In.vWorldPos;

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

    pass TestEffect
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDefault, 0);
        SetBlendState(BSDefault, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_TEST_EFFECT();
    }

    pass EffectSphere1004
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDefault, 0);
        SetBlendState(BSAlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFFECT_SPHERE_1004();
    }
}
