
#include "../../../EngineSDK/hlsl/EngineShaderDefine.hlsli"


// 241115 수업 GS로 점으로 부터 정점을 만들어서 인스턴스들을 렌더링 할 것이다.

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;
vector g_vCamPosition;
float g_StartX, g_StartY;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vPSize : PSIZE;
    row_major float4x4 TransformMatrix : WORLD;
    float2 vLifeTime : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix matWV, matWVP;
    
    float fScaleX = length(In.TransformMatrix._11_12_13);
    float fScaleY = length(In.TransformMatrix._21_22_23);
    float fScaleZ = length(In.TransformMatrix._31_32_33);
    
    matrix TransformMatrix = float4x4(float4(1.f, 0.f, 0.f, 0.f), float4(0.f, 1.f, 0.f, 0.f), float4(0.f, 0.f, 1.f, 0.f), In.TransformMatrix._41_42_43_44);
    
    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
    
    Out.vPosition = mul(vPosition, g_WorldMatrix);
    Out.vPSize = float2(In.vPSize.x * fScaleX, In.vPSize.y * fScaleY);
    Out.vLifeTime = In.vLifeTime;
    
    return Out;
}


/* VS : 정점하나 당 호출되는 함수다. */
/* GS : 그리고자하는 도형(POINT, LINE, TRIANGLE) 하나당 호출되는 함수이다. */
/* PS : 픽셀하나 당 호출되는 함수다. */
struct GS_IN
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
};

// SV를 붙인다. 변환이 완료된 좌표를 PS로 던진다.
struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};


//GS_MAIN(triangle GS_IN In[3])
//GS_MAIN(line GS_IN In[2])
[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
    GS_OUT Out[4];
    vector vLook = g_vCamPosition - In[0].vPosition;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook.xyz, vRight)) * In[0].vPSize.y * 0.5f;
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    float fGravity = -30.8f; // 중력 상수
    float fDeltaTime = In[0].vLifeTime.y; // 경과 시간
    float fGravityEffect = 0.5f * fGravity * (fDeltaTime * fDeltaTime); // y = 0.5 * g * t^2
    
    // 점의 월드 위치에서 중력 효과 적용
    float3 AdjustedPosition = In[0].vPosition.xyz;
    AdjustedPosition.y += fGravityEffect; // 중력 적용
    
    Out[0].vPosition = float4(AdjustedPosition.xyz + vRight + vUp, 1.f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord = float2(g_StartX, g_StartY);
    Out[0].vLifeTime = In[0].vLifeTime;
    
    Out[1].vPosition = float4(AdjustedPosition.xyz - vRight + vUp, 1.f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord = float2(g_StartX + 0.5f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    Out[2].vPosition = float4(AdjustedPosition.xyz - vRight - vUp, 1.f);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexcoord = float2(g_StartX + 0.5f, g_StartY + 0.5f);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    Out[3].vPosition = float4(AdjustedPosition.xyz + vRight - vUp, 1.f);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    Out[3].vTexcoord = float2(g_StartX, g_StartY + 0.5f);
    Out[3].vLifeTime = In[0].vLifeTime;

    OutStream.Append(Out[0]);
    OutStream.Append(Out[1]);
    OutStream.Append(Out[2]);
    
    OutStream.RestartStrip();
    
    OutStream.Append(Out[0]);
    OutStream.Append(Out[2]);
    OutStream.Append(Out[3]);

    OutStream.RestartStrip();
    
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
    
    if (Out.vColor.a < 0.1f)
        discard;
    
    // 시간에 따라 알파 값을 감소시키기
    float lifetimeProgress = saturate(In.vLifeTime.y / In.vLifeTime.x); // 경과 시간 / 총 수명
    // Out.vColor.a *= 1.f - lifetimeProgress; // 시간이 지남에 따라 알파 값 감소
    Out.vColor.a = 1.f - lifetimeProgress;
    
    return Out;
}

PS_OUT PS_MAIN_ONCE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if (Out.vColor.a < 0.3f)
        discard;

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
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_LOOP();
    }

    pass Once
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDefault, 0);
        SetBlendState(BSAlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_ONCE();
    }
}
