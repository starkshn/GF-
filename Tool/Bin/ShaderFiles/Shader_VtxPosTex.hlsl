

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;



sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexCoord : TEXCOORD0; // 만약 TEXCOORD가 하나라면 뒤에 붙은 0생략이 가능하지만 그게아니라면 0 1 2 이런식으로 붙여야한다.
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    
    Out.vTexCoord = In.vTexCoord;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexCooord : TEXCOORD0;
};


struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

// Pixel Shader
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexCooord);    
    // Out.vColor = float4(In.vTexCooord.x, In.vTexCooord.y, 0.0f, 1.0f);
    
    return Out;
}


technique11 DefaultTechnique
{
    pass DefaultPass
    {
        VertexShader = compile vs_5_0 VS_MAIN(); // 컴파일 해라, 5.0버젼으로 VS_MAIN호출한다.
        PixelShader = compile ps_5_0 PS_MAIN(); // ''
    }
}