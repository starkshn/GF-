
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;

float3 g_Light = (0.f, 1.f, 0.f);

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
    Out.vNormal = In.vNormal;
    
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
    
    float3 normal = normalize(In.vNormal);
    float3 light = g_Light;
    light = normalize(light);
    float4 val = dot(normal, light);
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}


technique11 DefaultTechnique
{
	
    pass DefaultPass
    {
        // SetRasterizerState(FillModeWireFrame);
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
