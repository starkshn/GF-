

/* D3D11_SAMPLER_DESC */
sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

sampler LinearSamplerClamp = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = clamp;
    AddressV = clamp;
};


sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = wrap;
    AddressV = wrap;
};


RasterizerState RSDefault
{
    FillMode = SOLID;
    CullMode = BACK;
    FrontCounterClockwise = false;
};



RasterizerState RSCullFront
{
    FillMode = SOLID;
    CullMode = Front;
    FrontCounterClockwise = false;
};

RasterizerState RSWireframe
{
    FillMode = Wireframe;
    CullMode = BACK;
    FrontCounterClockwise = false;
};

RasterizerState RSCullNone
{
    FillMode = SOLID;
    CullMode = None;
    FrontCounterClockwise = false;
};


// Depth Stencil State
DepthStencilState DSSDefault
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;         // 깊이 값이 작거나 같은 경우
};

DepthStencilState DSSDisable
{
    DepthEnable = false;
    DepthWriteMask = zero;          // 깊이 버퍼에 어떻게 기록할 것인지에 대한 부분이다.
};


BlendState BSDefault
{
    BlendEnable[0] = false;
};

BlendState BSAlphaBlend
{
    // 0 번 렌더 타겟 뷰에 블렌드를 할 건지 말건지를 의미하는것이다.
    BlendEnable[0] = true;


    // SrcColor.rgb * SrcColor.a + DstColor.rgb * (1.f - SrcColor.a)

    // 현재 내가 그릴려고 햇던 rgb에 알파를 곱한다.
    SrcBlend = SRC_ALPHA;

    // 이미 그려진 색에 알파의 역치를 곱한다.
    DestBlend = INV_SRC_ALPHA;
    
    // 더한다.
    BlendOp = Add;
};

// BSAlaphaBlend는 기존에 그렸던 색과 섞는것인데, BSOneBlend는 색을 합치는 것이다.
BlendState BSOneBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    // srcColor.rgb * 1 + dstColor.rgb * 1
    SrcBlend = one;
    DestBlend = one;
    BlendOp = Add;
};


