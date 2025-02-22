
#include "../../../EngineSDK/hlsl/EngineShaderDefine.hlsli"





// 241114 파티클 수업

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;


struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;

    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
    float4 vTranslation : TEXCOORD4;
    float2 vLifeTime : TEXCOORD5;
    
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    
};

/* 1. 정점위치에 대한 기초적인 변환과정을 수행한다.(월드변환, 뷰변환, ) */

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vLifeTime = In.vLifeTime;

    return Out;
}





struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_LOOP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vColor.a < 0.2f)
        discard;
    
    // 시간에 따라 알파 값을 감소시키기
    float lifetimeProgress = saturate(In.vLifeTime.y / In.vLifeTime.x); // 경과 시간 / 총 수명
    Out.vColor.a *= 1.f - lifetimeProgress; // 시간이 지남에 따라 알파 값 감소
    
    if (Out.vColor.a < 0.1f)
        discard;
    
    
    return Out;
}

PS_OUT PS_MAIN_ONCE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;


    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if (Out.vColor.a < 0.3f)
        discard;

    Out.vColor.gb = 0.f;

    Out.vColor.a = saturate(In.vLifeTime.x - In.vLifeTime.y) * 2.f;

    Out.vColor.r = 1.f - Out.vColor.a;

    if (In.vLifeTime.x <= In.vLifeTime.y)
        discard;

    return Out;
}


technique11 DefaultTechnique
{
    pass LOOP
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDefault, 0);
        SetBlendState(BSDefault, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LOOP();
    }

    pass Once
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDefault, 0);
        SetBlendState(BSAlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ONCE();
    }
}





