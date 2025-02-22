


// 241008 수업 쉐이더
// Diffuse(명암연산), Ambient(환경광에 의한 색상), Specular(하이라이트)



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
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
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
    
    // 아래처럼 바로 In의 Normal을 return하면 안된다.
    // - In의 Normal벡터는 월드 상에 존재하는 것이므로 월드 행렬을 곱해서 같이 NDC로 넘겨주어야한다.
    // Out.vNormal = In.vNormal;
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);    
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    
    // 구조체의 시멘틱(Semantic)**은 데이터가 그래픽 파이프라인의 각 단계에서 어떻게 처리되고 전달되는지를 정의한다.
    // TEXCOORDn: 일반적인 사용자 정의 시멘틱으로, 다양한 데이터를 픽셀 셰이더로 전달할 때 사용한다.
    // 보간 과정은 데이터를 '수정'하는 것이 아니라, 각 픽셀에 대한 값을 평균하여 새로운 값을 생성하는 것
    
    //2. 시멘틱(Semantic)과 데이터 전달 방식
    //    SV_POSITION vs TEXCOORDn
    //    SV_POSITION:
        
    //    특수 시멘틱으로, 클립 공간에서의 위치를 나타냅니다.
    //    래스터라이저는 이 값을 사용하여 삼각형을 화면 공간에 매핑하고, 뷰포트 변환(Viewport Transformation)을 수행합니다.
    //    SV_POSITION은 픽셀 셰이더에 의해 화면 공간 위치로 사용되며, 일반적인 데이터 보간과는 별개로 처리됩니다.
    //    TEXCOORDn:
        
    //    일반 시멘틱으로, 다양한 사용자 정의 데이터를 픽셀 셰이더로 전달하는 데 사용됩니다.
    //    래스터라이저는 이 데이터를 퍼스펙티브 보간을 통해 픽셀 셰이더로 전달합니다.
    //    n은 인덱스를 나타내며, 여러 개의 TEXCOORD 시멘틱을 사용할 수 있습니다.
    //    보간(Interpolation)의 역할
    //    보간은 래스터라이저가 삼각형 내부의 각 픽셀에 대해 버텍스에서 전달된 데이터를 선형적으로 계산하는 과정입니다.
    //    퍼스펙티브 보간은 깊이에 따라 보간을 조정하여 원근감에 따른 왜곡을 최소화합니다.
    //    TEXCOORDn 시멘틱을 통해 전달된 데이터는 각 픽셀에서 정확하게 보간된 값을 갖게 됩니다.
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
    
    // 명암
    // float Shade = max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f);
    float4      vShade = saturate(max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
    
    // 0.f ~ 1.f
    // reflect함수는 반사 벡터를 구하는 함수이다. : Specular를 구하기 위해서 사용한다.
    float4      vReflect = float4(reflect(normalize(g_vLightDir), normalize(In.vNormal)), 0.f);
    float4      vLook = In.vWorldPos - g_vCamPosition; // 시선 벡터를 구함.
    
    float       fSpecular = pow(max(dot(normalize(vReflect) * -1, normalize(vLook)), 0.f), 50.f);
    
    // (g_vLightDiffuse * vMtrlDiffuse) : 빛을 받은 픽셀이 반사하는 색상 DIFFUSE
    // (g_vLightSpecular * vMtrlSpecular) : 빛을 받은 픽셀이 반사하는 색상.
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
