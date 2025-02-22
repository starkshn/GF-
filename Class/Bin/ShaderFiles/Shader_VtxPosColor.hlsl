

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float3 g_Color = { 0.f, 1.f, 0.f };

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vColor : COLOR;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vColor : COLOR;
};

RasterizerState FillModeWireFrame
{
    FillMode = Wireframe;
};


VS_OUT VS_MAIN_COL(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    
    Out.vColor = g_Color;
    
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float3 vColor : COLOR;
};


struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

// Pixel Shader
PS_OUT PS_MAIN_COL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    // Out.vColor = float4(g_Color, 1.f);
    Out.vColor = float4(0.f, 1.f, 0.f, 1.f);

    return Out;
}


technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(FillModeWireFrame);

        VertexShader = compile vs_5_0 VS_MAIN_COL(); // 컴파일 해라, 5.0버젼으로 VS_MAIN호출한다.
        GeometryShader = NULL;

        PixelShader = compile ps_5_0 PS_MAIN_COL(); // ''
    }
}