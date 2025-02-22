

// ============================================================================
// 241028 수업 쉐이더, Terrain Masking, Brush UV 직접 계산
// ============================================================================


float4x4    g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector      g_vLightDir = float4(1.f, -1.f, 1.f, 0.f);
vector      g_vLightDiffuse = float4(1.f, 1.f, 1.f, 1.f);
vector      g_vLightAmbient = float4(1.f, 1.f, 1.f, 1.f);
vector      g_vLightSpecular = float4(1.f, 1.f, 1.f, 1.f);

Texture2D   g_DiffuseTexture[2];
Texture2D   g_MaskTexture;
Texture2D   g_BrushTexture;

vector      g_vMtrlAmbient = float4(0.3f, 0.3f, 0.3f, 1.f);
vector      g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);
vector      g_vCamPosition;

vector      g_vBrushPos = float4(70.f, 0.f, 50.f, 1.f);     // 월드 상의 위치를 임의로 잡아준다.
float       g_fBrushRange = 10.f;

sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
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
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
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
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);    
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
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
    
    // 풀 샘플링
    vector vSrcDiffuse = g_DiffuseTexture[0].Sample(LinearSampler, In.vTexcoord * 50.f);
    
    // 땅 샘플링
    vector vDstDiffuse = g_DiffuseTexture[1].Sample(LinearSampler, In.vTexcoord * 50.f);
    
    // vMask는 UV를 크게 늘리지 않아도 된다.
    vector vMask = g_MaskTexture.Sample(PointSampler, In.vTexcoord);
    
    // 임시로 잡아준 g_vBrushPos의 위치는 월드의 위치이기 때문에 In.vWorldPos와 비교해준다.
    // - 월드 상에 위치하는 좌표를 기준으로 새로운 UV를 잡아줄 것이다.
    vector vBrush = (vector)0.f;
    
    if (g_vBrushPos.x - g_fBrushRange <= In.vWorldPos.x  && In.vWorldPos.x < g_vBrushPos.x + g_fBrushRange
        && g_vBrushPos.z - g_fBrushRange <= In.vWorldPos.z && In.vWorldPos.z < g_vBrushPos.z + g_fBrushRange)
    {
        float2 vTexcoord = 0.f;
        
        vTexcoord.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (2.f * g_fBrushRange);
        vTexcoord.y = ((g_vBrushPos.z + g_fBrushRange - In.vWorldPos.z) / (2.f * g_fBrushRange));
        
        vBrush = g_BrushTexture.Sample(LinearSampler, vTexcoord);

    }
    
    // vMask텍스쳐 자체가 흰색 검은색으로 이루어져 있다.
    // 흰색 부분은 땅을 칠하고, 검은색 부분은 풀때기로 칠하려는 목적이다.
    vector vMtrlDiffuse = vDstDiffuse * vMask + vSrcDiffuse * (1.f - vMask) + vBrush;
    
    float4 vShade = saturate(max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
    
    /* 0.f ~ 1.f */
	/* 빛이 들어와서 맞고 튕긴 반사벡터와 이 픽셀을 바라보는 시선벡터가 이루는 각이 180도 일때 최대 */
    float4 vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    float4 vLook = In.vWorldPos - g_vCamPosition;

    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);
    
    Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * vShade + (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
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
