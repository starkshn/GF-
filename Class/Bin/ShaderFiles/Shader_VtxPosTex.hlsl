
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
Texture2D g_DepthTexture;


// HLSL 는 절차지향 언어라 함수로 시작한다.
// 함수의 이름이 대문자인 이유는 별 다른 뜻없다. 소문자도 노상관이다.


struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0; // 만약 TEXCOORD가 하나라면 뒤에 붙은 0생략이 가능하지만 그게아니라면 0 1 2 이런식으로 붙여야한다.
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

// 1. 정점 위치에 대한 기초적인 변환 과정을 수행한다. (월드 변환, 뷰변환, 투영 변환)


// VS_MAIN은 정점 쉐이더의 진입점 함수이다.
// VS
// VS 구동시점 : m_pContext->DrawIndexed();
// 그릴때 사용하고자 했던 정점들중, 
// 인덱스 버퍼(0, 1, 2, 0, 2, 3)가 지정해주고 있는 정점들 중 하나를 골라서 VS_MAIN함수를 호출하며 인자로 정점한개를 전달해준다.
// VS_MAIN은 정점을 반환한다. -> 이것을 (현재는) PS가 받아서 색을 채워야한다.
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    // XMVector3TransformCoord함수는 Z나누기까지 수행하지만(원근 투영을 알아서 수행하지만) mul함수는 그렇지 않다.
    // mul함수는 행렬 곱만을 수행한다.
    // https://cjbworld.tistory.com/72 <- 투영 변환 행렬 (내글)
    // 위치에다가 matWVP를 곱했다. Clip Space로 변환되었는데 Z나누기는 하지 않음 -> 레스터라이져 단계에서 장치가 알아서 해준다.
    // Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    
    // Out.vPosition = vector(In.vPosition, 1.f);
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}
// VS 설명을 이어가면 호출이 일단 먼저 3번 이루어진다.
// 0번째 정점, 1번째 정점, 2번째 정점을 호출하고 멈춘다. 우리가 m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
// 위처럼 설정했기 때문에 3개의 정점으로 하나의 폴리곤을 만들어냄 -> 픽셀을 생성해냄 (삼각형에 해당하는)
// 이렇게 정점이 3개가 다모이면 눈에 보이지 않는 연산을 스스로 수행한다.
// 1. 정점의 위치벡터를 Z나누기를 수행한다.
// 2. WVP -> ViewPort 변환을 수행한다.
// 그다음에 레스터 라이즈 단계이고 여기서 픽셀의 정보를 생성한다. 이 만들어진 픽셀을 PS_MAIN에다가 전달한다.


// 레스터 라이져 단계
// https://www.notion.so/10c4bccaf9eb80fdb17df69cd2fe37f4?pvs=4
// 간단하게 정리하자면 PS 사용은 선택적이지만 레스터 라이져 단계는 항상 클리핑, 원근 투영을 수행하여 점을 같은 공간으로 변환하고
// 뷰포트 변환한다.(매핑한다)
// WVP를 수행하면 Clip Space라 간주할 수 있다.

// 정점의 위치 벡터를 위치벡터의 W로 나눈다.
// 뷰포트 영역으로 정점들을 변환한다.
// 레스터라이즈(정점 정보를 보간하여 픽셀의 정보가 생성된다)
// 이렇게 생성된 픽셀을 PS_MAIN() 호출하면서 전달해준다 (이때 픽셀의 수만큼 PS_MAIN이 호출된다)


// PS_IN 은 결국 VS_OUT과 완전히 같은데 가독성을 위해 또 따로 구조체를 정의해주도록 한다.
// PS_MAIN에서 PS_IN을 보면 vPosition
// SV_POSITION이라 되어 있는데 SV는 Shader Value의 약자로 쉐이더 연산이 끝난 변수라고 명시한 것이다.
// VS < > PS < > 이 공간에 다른 쉐이더 단계가 들어갈 수 있는데 PS로 오기전에 연산이 안 끝났다면 그대로 POSITION이라는 이름으로 린턴한다.

struct VS_OUT_SOFT_EFFECT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

VS_OUT_SOFT_EFFECT VS_MAIN_SOFT_EFFECT(VS_IN In)
{
    VS_OUT_SOFT_EFFECT Out = (VS_OUT_SOFT_EFFECT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition; // ClipSpace로 변경된 값을 그대로 대입 해준다.
    
    return Out;
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

// 장치는 8개의 렌더 타겟 뷰를 가질 수 있는데 우리는 지금 장치의 0번째에 백버퍼 렌더타겟 뷰가 바인딩 되어 있기때문에
// 이 0번째에 vColor를 매핑하라는 의도에서 SV_TARGET0으로 설정한다.
// 0번째 RTV에 그리는 연산이 끝난 색생이라고 알려준다.
struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

// Pixel Shader
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    // 텍스쳐를 로드 해와서 해당 텍스쳐의 색을 보간해야하는데 현재 텍스쳐 로드 기능이 없으므로 흰색으로 리턴해준다.
    // Out.vColor = float4(0.5f, 0.5f, 0.5f, 1.f);
    
    // Out.vColor = g_Texture.Sample(어떻게얻어올래?, In.vTexcoord);*/
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    // 회색 가능
    // Out.vColor.gb = Out.vColor.r;

    return Out;
}

struct PS_IN_SOFT_EFFECT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

PS_OUT PS_MAIN_SOFT_EFFECT(PS_IN_SOFT_EFFECT In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    float2 vTexcoord = 0.f;
    
    // 현재 w나누기를 수행하지 않은 vProjPos.x 를 In.vProjPos.w로 나누고 있다.
    // VS_MAIN_SOFT_EFFECT 단계에서 vPos * matWVP를 수행한 값을 vProjPos에 넣어주었는데, vPos * matWVP는 ClipSpace상의 위치를 말하고 vProjPos의 w값에는 뷰스페이스상의 Z값이 들어가 있다. 
    // VS_OUT_SOFT_EFFECT의 vProjPos의 시멘틱에는 SV(ShaderValue)붙이지 않았으므로 장치가 W나누기 수행을 하지 않는다. (반면 vPosition의 경우 SV를 붙였기 때문에 연산이 다 끝난 것이라 장치가 판단하여 W나누기를 수행하여 뷰포트 매핑이된다)
    // - 따라서 위의 이유로 직접 w나누기를 수행하여 NDC좌표로 만드는 것이고 * 0.5 + 0.5의 값들은 NDC의 좌표로 부터 텍스쳐 픽셀 좌표로 보정해주는 값이다.
    // - 즉 NDC상에 -1, 1은 좌상단인데 뷰포트 상에서는 0, 0이다. 즉 좌상단 (-1, 1)을 (0, 0)으로 만드는 과정이다.
    vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;
    
    // 위에서 설명한대로 vTexcoord에 해당하는 픽셀을 샘플링한다.
    vector vDepth = g_DepthTexture.Sample(LinearSampler, vTexcoord);
    
    float fOldZ = vDepth.y * 1000.f;
    
    Out.vColor.a = Out.vColor.a * saturate(fOldZ - In.vProjPos.w);
    
    return Out;
}


technique11 DefaultTechnique
{
    // 우리가 수행해야할 정점, 픽셀 쉐이더의 진입점 함수를 지정한다.
    pass DefaultPass
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDefault, 0);
        SetBlendState(BSDefault, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN(); // 컴파일 해라, 5.0버젼으로 VS_MAIN호출한다.
        GeometryShader = NULL;

        PixelShader = compile ps_5_0 PS_MAIN(); // ''
    }

    pass SoftEffect
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDefault, 0);
        SetBlendState(BSAlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_SOFT_EFFECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SOFT_EFFECT();
    }
}