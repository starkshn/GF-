

// 241007 수업 쉐이더 코드
// Diffuse(명암), Ambient(환경광에 의한 색상) 만 표현한 쉐이더

float4x4        g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector          g_vLightDir = float4(1.f, -1.f, 1.f, 0.f);          // 빛의 방향
vector          g_vLightDiffuse = float4(1.f, 1.f, 1.f, 1.f);       // 빛 Diffuse 색상
vector          g_vLightAmbient = float4(1.f, 1.f, 1.f, 1.f);       // 빛 Ambient 색상

Texture2D       g_DiffuseTexture;                                   // 구 Texture, 현 Diffuse 재질
vector          g_vMtrlAmbient = float4(0.3f, 0.3f, 0.3f, 1.f);     // 재질 Ambient


/* D3D11_SAMPLER_DESC */
sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

RasterizerState FillModeWireFrame
{
    FillMode = Wireframe;
};


struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
};

/* 1. 정점위치에 대한 기초적인 변환과정을 수행한다.(월드변환, 뷰변환, ) */

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    // 아래처럼 바로 In의 Normal을 return하면 안된다.
    // - In의 Normal벡터는 월드 상에 존재하는 것이므로 월드 행렬을 곱해서 같이 NDC로 넘겨주어야한다.
    // Out.vNormal = In.vNormal;
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

/* PixelShader */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord * 3.f);
    
    // float Shade = max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f);
    float4 vShade = saturate(max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
    
    // Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * Shade;
    Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * vShade;

    return Out;
}


technique11 DefaultTechnique
{
	
    pass DefaultPass
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;

        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass p1

    {
        SetRasterizerState(FillModeWireFrame);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;

        PixelShader = compile ps_5_0 PS_MAIN();
    }
}
