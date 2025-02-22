

// 241011 수업 기준 VtxMesh 쉐이더 파일

float4x4    g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector      g_vLightDir = float4(1.f, -1.f, 1.f, 0.f);
vector      g_vLightDiffuse = float4(1.f, 1.f, 1.f, 1.f);
vector      g_vLightAmbient = float4(1.f, 1.f, 1.f, 1.f);
vector      g_vLightSpecular = float4(1.f, 1.f, 1.f, 1.f);

Texture2D   g_DiffuseTexture;
vector      g_vMtrlAmbient = float4(0.3f, 0.3f, 0.3f, 1.f);
vector      g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);

vector      g_vCamPosition;


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
    float3 vTangent : TANGENT0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT0;
    float4 vWorldPos : TEXCOORD1;
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
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    
    Out.vTangent = mul(float4(In.vTangent, 1.f), g_WorldMatrix);
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT0;
    float4 vWorldPos : TEXCOORD1;
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
    
    float4 vShade = saturate(max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
    
    float4 vReflect = float4(reflect(normalize(g_vLightDir), normalize(In.vNormal)), 0.f);
    float4 vLook = In.vWorldPos - g_vCamPosition; // 시선 벡터를 구함.
    
    float fSpecular = pow(max(dot(normalize(vReflect) * -1, normalize(vLook)), 0.f), 50.f);
    
    Out.vColor = (1.f, 1.f, 1.f, 1.f) * vShade + (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
    // Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * vShade + (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
    
    return Out;
}


technique11 DefaultTechnique
{
	
    pass DefaultPass
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass p1

    {
        SetRasterizerState(FillModeWireFrame);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}
