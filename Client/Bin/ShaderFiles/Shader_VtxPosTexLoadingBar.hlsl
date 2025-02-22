
// 정점 쉐이더는 정점 위치에 대한 변환을 수행해야한다. -> 행렬이 필요하다.
// 월드 변환, 뷰 변환, 투영 행렬은 쉐이더의 외부에 존재하는데 이 외부에 있는 행렬들을 가져오고 싶다면 아래처럼 전역변수로 선언한다.
// 나중에 쉐이더 파일을 빌드해서 나온 객체에 다가 행렬을 전달할 수 있는 기능이 쉐이더 객체에 있다.
// 아래 쉐이더 전역 변수들을 '상수 테이블'이라 부른다.
// - 테이블이란 어떤 데이터의 집합을 의미한다.
// - 가상함수 테이블 : 실제 호출해야할 함수들이 있는 집합
// - 상수 테이블 : 상수들의 집합을 의미한다.
// 이름 그대로 상수 테이블 이기때문에 '초기화'는 가능하지만 값 변경은 불가능하다.



#include "../../../EngineSDK/hlsl/EngineShaderDefine.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;
Texture2D g_TextureColor;

int g_iTotLoadCnt;
int g_iCurLoadCnt;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
   
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};


struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    
   
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    if (Out.vColor.a < 0.1f)
        discard;
    
    float fRatio = float(g_iCurLoadCnt) / float(g_iTotLoadCnt);
    
    if (In.vTexcoord.x > fRatio)
        discard;
    
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

   
}