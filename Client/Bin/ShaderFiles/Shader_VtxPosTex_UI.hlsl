#include "../../../EngineSDK/hlsl/EngineShaderDefine.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;
Texture2D g_TextureFontColor;
Texture2D g_TextureHPBarColor;

Texture2D g_DepthTexture;

float g_MaxLifeTime;
float g_CurLifeTime;

float g_MaxFadeInTime;
float g_CurFadeInTime;

int g_iFontColorType;


float g_fHPMax;
float g_fCurHP;
float g_fPrevHP;
float g_fCurUpdateHP;


float g_fDecreaseTime = 0.5f;
float g_fDecreaseCurTime;



float g_fTexCoordX;
bool g_bUseTexCoordFlex;


float g_MaxAlphaTime;
float g_CurAlphaTime;



int g_iAnimRow;
int g_iAnimCol;
float g_fMaxAnimTime;
float g_fCurAnimtime;


int g_iDamageFontColor;

Texture2D g_TextureMonsterHPVertical;
bool g_bUseHPVertical;



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
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord, 0.f);
    
    if (Out.vColor.a < 0.1f)
        discard;
    
    return Out;
}

PS_OUT PS_MAIN_UIBGTest(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vColor.a < 0.1f)
    {
        Out.vColor.a = 0.4f;
    }
    
    return Out;
}

PS_OUT PS_MAIN_UIHPBAR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord, 0.f);
    if (Out.vColor.a < 0.1f)
        discard;
    
    return Out;
}

PS_OUT PS_MAIN_FONT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord, 0.f);
    if (Out.vColor.a < 0.1f)
        discard;
    
    if (Out.vColor.r < 0.1f && Out.vColor.g < 0.1f && Out.vColor.b < 0.1f)
    {
        return Out;
    }
    else
    {
        if (g_iFontColorType)
        {
            Out.vColor = g_TextureFontColor.Sample(LinearSampler, In.vTexcoord, 0.f);
        }
    }
    
    return Out;
}

float g_fExpansionTime;


VS_OUT VS_MAIN_HPBar(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}


PS_OUT PS_MAIN_HPBar(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord, 0.f);
    
    if (Out.vColor.a < 0.1f)
        discard;
    
    //// 시간 기반으로 진동하는 확장 값 계산
    //float fExpansion = 0.2f * abs(sin(g_fExpansionTime * 5.f)); // // 0.02f는 확장 크기, 5.0f는 진동 속도
    
    float fCurHPRatio = g_fCurHP / g_fHPMax;
    float fUpdateHPRatio = g_fCurUpdateHP / g_fHPMax;
    
    float expandedEnd = min(1.0f, fUpdateHPRatio + 0.1f); // 오른쪽 확장
    
    // 체력 비율 넘은 부분 
    if (In.vTexcoord.x > (g_fCurHP / g_fHPMax))
    {
        // 현재 체력 비율 보다는 크고 현재 업데이트 중인 비율 보다 작다면 흰색으로 채운다. 
        if (In.vTexcoord.x <= (g_fCurUpdateHP / g_fHPMax))
        {
            Out.vColor = vector(1.f, 1.f, 0.f, 1.f);
        }
        else if (In.vTexcoord.x > (g_fCurUpdateHP / g_fHPMax))
        {
            // 현재 업데이트중인 체력 비율 보다 작은 부분은 discard해준다.
            discard;
        }
    }
    // 체력 비율 넘지 않은 부분2
    else
    {
        // 검은색 테두리 부분 그대로 return 한다.
        if (Out.vColor.r < 0.1f && Out.vColor.g < 0.1f && Out.vColor.b < 0.1f)
        {
            return Out;
        }
        else
        {
            if (g_iFontColorType)
            {
                Out.vColor = g_TextureHPBarColor.Sample(LinearSampler, float2(0.5f, 0.5f));
            }
        }
    }
    
    return Out;

}

PS_OUT PS_MAIN_UI_TEX(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord, 0.f);
    if (Out.vColor.a < 0.1f)
        discard;
    

    
    return Out;
}

PS_OUT PS_MAIN_UI_TEX_DASH(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord, 0.f);
    if (Out.vColor.a < 0.1f)
        discard;
    
    if (g_bUseTexCoordFlex == true)
    {
        if (In.vTexcoord.x > g_fTexCoordX)
            discard;
    }
    
    return Out;
}

PS_OUT PS_MAIN_UI_TEX_EFFECT_ALPHA(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    if (Out.vColor.a < 0.05f)
        discard;
    
    float fAlpha = Out.vColor.a * saturate(1.f - (g_CurAlphaTime / g_MaxAlphaTime));
    if (fAlpha > 0.9f)
        discard;
    
    Out.vColor.a = fAlpha;
    return Out;
}


VS_OUT VS_MAIN_UI_TEX_ANIM(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    
    float fOffSetRow = 1.f / g_iAnimRow;
    float fOffSetCol = 1.f / g_iAnimCol;
    
    float fRatio = g_fCurAnimtime / g_fMaxAnimTime;
    
    int iTot = g_iAnimRow * g_iAnimCol;
    int iIdx = (int) min(fRatio * iTot, iTot - 1);
    
    int iRow = iIdx / g_iAnimRow;
    int iCol = iIdx / g_iAnimCol;
    
    float fStartRow = iRow * fOffSetRow;
    float fStartCol = iCol * fOffSetCol;
    
    if (0.f == In.vTexcoord.x && 0.f == In.vTexcoord.y)
    {
        Out.vTexcoord.x = fStartCol;
        Out.vTexcoord.y = fStartRow;
    }
    else if (1.f == In.vTexcoord.x && 0.f == In.vTexcoord.y)
    {
        Out.vTexcoord.x = fStartCol + fOffSetCol;
        Out.vTexcoord.y = fStartRow;
    }
    else if (1.f == In.vTexcoord.x && 1.f == In.vTexcoord.y)
    {
        Out.vTexcoord.x = fStartCol + fOffSetCol;
        Out.vTexcoord.y = fStartRow + fOffSetRow;
    }
    else if (0.f == In.vTexcoord.x && 1.f == In.vTexcoord.y)
    {
        Out.vTexcoord.x = fStartCol;
        Out.vTexcoord.y = fStartRow + fOffSetRow;
    }
  
    return Out;
}

PS_OUT PS_MAIN_UI_TEX_ANIM(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    if (Out.vColor.a < 0.05f)
        discard;
    
    return Out;
}

PS_OUT PS_MAIN_UI_DAMAGE_FONT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    if (Out.vColor.a < 0.05f)
        discard;
    
    Out.vColor.a = g_CurAlphaTime;
    
    if (g_iDamageFontColor == 1)
    {
        if (Out.vColor.r >= 0.8f && Out.vColor.g >= 0.8f && Out.vColor.b >= 0.8f)
            Out.vColor = float4(1.0f, 1.0f, 0.0f, Out.vColor.a);
        else
            return Out;
    }
    
    return Out;
}

float g_fAlphaAccVertical;

// PS_MAIN_UI_VERTICAL
PS_OUT PS_MAIN_UI_VERTICAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    if (Out.vColor.a < 0.05f)
        discard;
    
    float fAlpha = g_fAlphaAccVertical;
    
    Out.vColor.a = min(0.8f, 0.8f + fAlpha);
    
    Out.vColor.rg = 1.f;
    Out.vColor.b = 1.f;
    
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

        VertexShader = compile vs_5_0 VS_MAIN();        // 컴파일 해라, 5.0버젼으로 VS_MAIN호출한다.
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();         // ''
    }

    pass UIBGTest
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDefault, 0);
        SetBlendState(BSAlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_UIHPBAR();
    }

    pass UIHPBAR
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDefault, 0);
        SetBlendState(BSAlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_UIHPBAR();
    }


    pass UIFont
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDefault, 0);
        SetBlendState(BSAlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FONT();
    }

    pass UIHPBar
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDefault, 0);
        SetBlendState(BSAlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_HPBar();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_HPBar();
    }
    

// 5
    pass UI_TEX
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDefault, 0);
        SetBlendState(BSAlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_UI_TEX();
    }

// 6
    pass UI_TEX_DASH
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDefault, 0);
        SetBlendState(BSAlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_UI_TEX_DASH();
    }


// 7
    pass UI_TEX_EFFECT_ALPHA
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDefault, 0);
        SetBlendState(BSAlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_UI_TEX_EFFECT_ALPHA();
    }

// 8
    pass UI_TEX_ANIM
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDefault, 0);
        SetBlendState(BSAlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_UI_TEX_ANIM();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_UI_TEX_ANIM();
    }

// 9
    pass UI_DAMAGE_FONT
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDefault, 0);
        SetBlendState(BSAlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_UI_DAMAGE_FONT();
    }


// 10
    pass UI_VERTICAL
    {
        SetRasterizerState(RSDefault);
        SetDepthStencilState(DSSDefault, 0);
        SetBlendState(BSAlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_UI_VERTICAL();
    }
}