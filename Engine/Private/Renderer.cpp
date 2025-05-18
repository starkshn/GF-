#include "Renderer.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Level.h"
#include "AlphaObject.h"
#include "Shadow.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : 
    m_pDevice(pDevice),
    m_pContext(pContext),
    m_pGameInst(CGameInstance::GetInstance())
{
    SafeAddRef(m_pDevice);
    SafeAddRef(m_pContext);
    SafeAddRef(m_pGameInst);
}

HRESULT CRenderer::Initialize()
{
    // 뷰포트 정보를 가져온다.
    _uint iNumViewPorts = 1;
    D3D11_VIEWPORT ViewPortDesc = {};
    m_pContext->RSGetViewports(&iNumViewPorts, &ViewPortDesc);

    m_iOriginalViewPortWidth =  (_uint)ViewPortDesc.Width;
    m_iOriginalViewPortHeight = (_uint)ViewPortDesc.Height;
    
    // Target Diffuse
    // - Clear 색상은 일단 검은색, PixelFormat은 색을 저장할 것이기 때문에 8비트로 설정
    if (FAILED(m_pGameInst->AddRenderTarget(TEXT("TargetDiffuse"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    // Target_Normal 
    // - 노말의 경우 32비트로하면 프레임 드랍이 발생할 수 있다. 따라서 16비트로 한다. 문제 없으면 32비트도 ㄱㅊ
    if (FAILED(m_pGameInst->AddRenderTarget(TEXT("TargetNormal"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    // TargetDepth (241128)
    if (FAILED(m_pGameInst->AddRenderTarget(TEXT("TargetDepth"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 1.f, 0.f, 1.f)))) 
        return E_FAIL;

    // PickingWorld (241129)
    if (FAILED(m_pGameInst->AddRenderTarget(TEXT("TargetPickingWorld"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)))) 
        return E_FAIL;

    // 초록색으로 그려지는 렌더링 타겟
    if (FAILED(m_pGameInst->AddRenderTarget(TEXT("TargetOuterPre"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    // Target_Shade
    if (FAILED(m_pGameInst->AddRenderTarget(TEXT("TargetShade"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    // TargetSpecular
    if (FAILED(m_pGameInst->AddRenderTarget(TEXT("TargetSpecular"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;
    
    // TargetHeight
    //if (FAILED(m_pGameInst->AddRenderTarget(TEXT("TargetHeight"), (_uint)g_iMaxWidth, (_uint)g_iMaxHeight, DXGI_FORMAT_R32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
    //    return E_FAIL;

    // Target_LightDepth
    // - Shadow에 사용한다
    if (FAILED(m_pGameInst->AddRenderTarget(TEXT("TargetLightDepth"), g_iMaxWidth, g_iMaxHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
        return E_FAIL;

    // TargetFinal
    if (FAILED(m_pGameInst->AddRenderTarget(TEXT("TargetFinal"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
        return E_FAIL;

    // TargetBlurX
    if (FAILED(m_pGameInst->AddRenderTarget(TEXT("TargetBlurX"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
        return E_FAIL;
    
    if (FAILED(m_pGameInst->AddRenderTarget(TEXT("TargetOuter"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    // Glow
    {

        ReadyDepthStencilView((_uint)ViewPortDesc.Width / 2, (_uint)ViewPortDesc.Height / 2, m_pGlowDepthStencilView);

        // TargetGlow
        if (FAILED(m_pGameInst->AddRenderTarget(TEXT("TargetGlow"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
            return E_FAIL;

        // GlowCopy
        if (FAILED(m_pGameInst->AddRenderTarget(TEXT("TargetGlowCopy"), (_uint)(ViewPortDesc.Width / 2), (_uint)(ViewPortDesc.Height / 2), DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
            return E_FAIL;

        // GlowCopyBlurX
        if (FAILED(m_pGameInst->AddRenderTarget(TEXT("TargetGlowCopyBlurX"), (_uint)(ViewPortDesc.Width / 2), (_uint)(ViewPortDesc.Height / 2), DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
            return E_FAIL;

        // GlowCopyBlurY
        if (FAILED(m_pGameInst->AddRenderTarget(TEXT("TargetGlowCopyBlurY"), (_uint)(ViewPortDesc.Width / 2), (_uint)(ViewPortDesc.Height / 2), DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
            return E_FAIL;

        // FinalGlow
        if (FAILED(m_pGameInst->AddRenderTarget(TEXT("TargetGlowFinal"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
            return E_FAIL;
    }

    // Distortion
    {
        if (FAILED(m_pGameInst->AddRenderTarget(TEXT("TargetDistortion"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
            return E_FAIL;
    }



    // 장치에 동시에 바인딩할 RTV들을 모아 놓는다.
    // - 이름도 고민이 많았는데 게임오브젝트들의 Diffuse, Normal 정보를 담아 두기 때문에 MRTGameObjects로 설정했다.
    {
        if (FAILED(m_pGameInst->AddMRT(TEXT("MRTGameObjects"), TEXT("TargetDiffuse"))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddMRT(TEXT("MRTGameObjects"), TEXT("TargetNormal"))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddMRT(TEXT("MRTGameObjects"), TEXT("TargetDepth"))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddMRT(TEXT("MRTGameObjects"), TEXT("TargetPickingWorld"))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddMRT(TEXT("MRTGameObjects"), TEXT("TargetOuterPre"))))
            return E_FAIL;
    }
   

    // MRTLights
    {
        if (FAILED(m_pGameInst->AddMRT(TEXT("MRTLightAcc"), TEXT("TargetShade"))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddMRT(TEXT("MRTLightAcc"), TEXT("TargetSpecular"))))
            return E_FAIL;
    }

    // MRTHeight
    {
        /*if (FAILED(m_pGameInst->AddMRT(TEXT("MRTHeight"), TEXT("TargetHeight"))))
            return E_FAIL;*/
    }

    // MRTShadow
    {
        if (FAILED(m_pGameInst->AddMRT(TEXT("MRTShadow"), TEXT("TargetLightDepth"))))
            return E_FAIL;
    }

    // MRTFinal
    {
        if (FAILED(m_pGameInst->AddMRT(TEXT("MRTFinal"), TEXT("TargetFinal"))))
            return E_FAIL;
    }

    // MRTBlurX
    {
        if (FAILED(m_pGameInst->AddMRT(TEXT("MRTBlurX"), TEXT("TargetBlurX"))))
            return E_FAIL;
    }

    // Glow
    {
        // MRTGlow
        {
            if (FAILED(m_pGameInst->AddMRT(TEXT("MRTGlow"), TEXT("TargetGlow"))))
                return E_FAIL;
        }

        // MRTGlowCopy
        {
            if (FAILED(m_pGameInst->AddMRT(TEXT("MRTGlowCopy"), TEXT("TargetGlowCopy"))))
                return E_FAIL;
        }

        // MRTGlowBlurX
        {
            if (FAILED(m_pGameInst->AddMRT(TEXT("MRTGlowBlurX"), TEXT("TargetGlowCopyBlurX"))))
                return E_FAIL;
        }

        // MRTGlowBlurY
        {
            if (FAILED(m_pGameInst->AddMRT(TEXT("MRTGlowBlurY"), TEXT("TargetGlowCopyBlurY"))))
                return E_FAIL;
        }

        // MRTGlowFinal
        {
            if (FAILED(m_pGameInst->AddMRT(TEXT("MRTGlowFinal"), TEXT("TargetGlowFinal"))))
                return E_FAIL;
        }
    }

    // MRTDistortion
    {
        if (FAILED(m_pGameInst->AddMRT(TEXT("MRTDistortion"), TEXT("TargetDistortion"))))
            return E_FAIL;
    }


    // Outer
    {
        if (FAILED(m_pGameInst->AddMRT(TEXT("MRTOuter"), TEXT("TargetOuter"))))
            return E_FAIL;
    }

    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../EngineSDK/hlsl/ShaderDeferred.hlsl"), VTX_POSTEX::Elements, VTX_POSTEX::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;

    m_pVIBuffer = CVIBufferRect::Create(m_pDevice, m_pContext);
    if (nullptr == m_pVIBuffer)
        return E_FAIL;

    // ShadeRenderTarget을 위해 월드 행렬 저장
    // - 스케일만 조절하면 위치는 상관없다? 241125 수업 중
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewPortDesc.Width, ViewPortDesc.Height, 1.f));
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewPortDesc.Width, ViewPortDesc.Height, 0.f, 1.f));

    if (FAILED(ReadyDepthStencilView(g_iMaxWidth, g_iMaxHeight)))
        return E_FAIL;

#ifdef _DEBUG

    _float Y = ViewPortDesc.Height;
    // m_pGameInst->ReadyRTVDebug(TEXT("TargetDiffuse"), 50.f, Y, 100.f, 100.f);
    // m_pGameInst->ReadyRTVDebug(TEXT("TargetNormal"), 150.f, Y, 100.f, 100.f);
    // m_pGameInst->ReadyRTVDebug(TEXT("TargetShade"), 250.f, Y, 100.f, 100.f);
    // m_pGameInst->ReadyRTVDebug(TEXT("TargetSpecular"), 350.f, Y, 100.f, 100.f);
    // m_pGameInst->ReadyRTVDebug(TEXT("TargetPickingWorld"), 450.f, Y, 100.f, 100.f);
    // m_pGameInst->ReadyRTVDebug(TEXT("TargetHeight"), 550.f, Y, 100.f, 100.f);
    // m_pGameInst->ReadyRTVDebug(TEXT("TargetLightDepth"), 650.f, Y, 100.f, 100.f);

    // m_pGameInst->ReadyRTVDebug(TEXT("TargetGlow"), 150.f, Y -150.f, 300.f, 300.f);
    // m_pGameInst->ReadyRTVDebug(TEXT("TargetGlowFinal"), 150.f, Y - 450.f, 300.f, 300.f);


    // m_pGameInst->ReadyRTVDebug(TEXT("TargetGlowCopy"), 350.f, Y - 50.f, 100.f, 100.f);
    // m_pGameInst->ReadyRTVDebug(TEXT("TargetGlowCopyBlurX"), 450.f, Y - 150.f, 300.f, 300.f);
    // m_pGameInst->ReadyRTVDebug(TEXT("TargetGlowCopyBlurY"), 550.f, Y - 50.f, 100.f, 100.f);


    // m_pGameInst->ReadyRTVDebug(TEXT("TargetDistortion"), 150.f, Y - 150.f, 300.f, 300.f);

    // m_pGameInst->ReadyRTVDebug(TEXT("TargetDistortion"), 150.f, Y - 150.f, 300.f, 300.f);

    // 외곽선
    // m_pGameInst->ReadyRTVDebug(TEXT("TargetOuter"), 150.f, Y - 100.f, 300.f, 300.f);
    // m_pGameInst->ReadyRTVDebug(TEXT("TargetOuterPre"), 450.f, Y - 100.f, 300.f, 300.f);

   /* m_pGameInst->ReadyRTVDebug(TEXT("TargetFinal"), 150.f, Y - 150.f, 300.f, 300.f);
    m_pGameInst->ReadyRTVDebug(TEXT("TargetShade"), 150.f, Y - 450.f, 300.f, 300.f);*/

#endif

    return S_OK;
}

HRESULT CRenderer::AddRenderObject(RENDER_GROUP eRenderGroup, CGameObject* pRenderObject)
{
    if (_uint(eRenderGroup) >= _uint(RG_END) || nullptr == pRenderObject)
        return E_FAIL;

    m_RenderObjects[_uint(eRenderGroup)].push_back(pRenderObject);
    
    SafeAddRef(pRenderObject);
    
    return S_OK;
}

HRESULT CRenderer::DrawRenderObject()
{
    if (FAILED(RenderPriority()))
        return E_FAIL;

    //if (FAILED(RenderHeight()))
    //    return E_FAIL;

    if (FAILED(RenderShadow()))
        return E_FAIL;

    if (FAILED(RenderNonBlend()))
        return E_FAIL;

    // NonBlend의 렌더가 끝나야 NormalRTV의 값이 기록이 되어 있기때문에 이때 호출한다.
    // 아래 함수가 호출되고 나면 빛연산을 수행한 뒤, TargetShade, TargetSpecular에 값을 기록한 상태가 끝난다.
    if (FAILED(RenderLights()))
        return E_FAIL;

    if (FAILED(RenderOuter()))
        return E_FAIL;

    // RenderFinal(수업 기준과 같다)
    if (FAILED(RenderShade()))
        return E_FAIL;

    if (m_bAdjustBlur)
    {
        if (FAILED(RenderBlurX()))
            return E_FAIL;

        if (FAILED(RenderBlurY()))
            return E_FAIL;
    }


    if(FAILED(RenderGlow()))
        return E_FAIL;

    if (FAILED(RenderGlowCopy()))
        return E_FAIL;

    if (FAILED(RenderGlowBlurX()))
        return E_FAIL;

    if (FAILED(RenderGlowBlurY()))
        return E_FAIL;

    if (FAILED(RenderGlowFinal()))
        return E_FAIL;

    if (FAILED(RenderDistortion()))
        return E_FAIL;

    if (FAILED(RenderNonLight()))
        return E_FAIL;

    if (FAILED(RenderBlend()))
        return E_FAIL;

    if (FAILED(RenderUI()))
        return E_FAIL;



#ifdef _DEBUG

    if (FAILED(RenderRTVDebug()))
        return E_FAIL;

#endif

    return S_OK;
}

HRESULT CRenderer::RenderPriority()
{
    // 241206
    // - 스카이 박스도 Blur 대상이기 때문에 TargetFinal에 그리도록 한다.
   /* if (FAILED(m_pGameInst->BeginMRT(TEXT("MRTFinal"))))
        return E_FAIL;*/

    for (auto& pRenderObj : m_RenderObjects[RG_PRIORITY])
    {
        if (nullptr != pRenderObj)
            pRenderObj->Render();

        SafeRelease(pRenderObj);
    }

    m_RenderObjects[RG_PRIORITY].clear();

    //if (FAILED(m_pGameInst->EndMRT()))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::RenderHeight()
{
    if (FAILED(m_pGameInst->BeginMRT(TEXT("MRTHeight"), m_pMaxDepthStencilView)))
        return E_FAIL;

    if (FAILED(SetUpViewPort(g_iMaxWidth, g_iMaxHeight)))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[RG_HEIGHT])
    {
        if (nullptr != pRenderObject)
            pRenderObject->RenderHeight();

        SafeRelease(pRenderObject);
    }
    m_RenderObjects[RG_HEIGHT].clear();


    if (FAILED(m_pGameInst->EndMRT()))
        return E_FAIL;

    if (FAILED(SetUpViewPort(m_iOriginalViewPortWidth, m_iOriginalViewPortHeight)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::RenderShadow()
{
    if (FAILED(m_pGameInst->BeginMRT(TEXT("MRTShadow"), m_pMaxDepthStencilView)))
        return E_FAIL;

    if (FAILED(SetUpViewPort(g_iMaxWidth, g_iMaxHeight)))
        return E_FAIL;

    for (auto& pRenderObj : m_RenderObjects[RG_SHADOW])
    {
        if (nullptr != pRenderObj)
            pRenderObj->RenderShadow();

        SafeRelease(pRenderObj);
    }
    m_RenderObjects[RG_SHADOW].clear();

    
    if (FAILED(m_pGameInst->EndMRT()))
        return E_FAIL;

    if (FAILED(SetUpViewPort(m_iOriginalViewPortWidth, m_iOriginalViewPortHeight)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::RenderNonBlend()
{
    /* Diffuse + vNormal */
    // 241119 이제 이곳에서 뭔가를 해야한다.


    /* 이 그룹에서 그려지는 객체들은 빛 연산이 필요하다. */
    /* 객체들을 마다 수행한다(x), 후처리로 한번(해상도픽셀갯수만큼)에 수행한다. */
    /* 후처리로 빛연산을 수행하기위해 어떤 데이터가 필요한가?! */
    /* 명암만 계산한다라과 가정한다면 Normal정보가 필요하다.(후처리로 명암연산이 가능) */
    /* 색상을 입혀서 그려주기위해 Diffuse정보도 필요했다. */

    if (FAILED(m_pGameInst->BeginMRT(TEXT("MRTGameObjects")))) 
        return E_FAIL;

    for (auto& pRenderObj : m_RenderObjects[RG_NONBLEND])
    {
        if (nullptr != pRenderObj)
            pRenderObj->Render();

        SafeRelease(pRenderObj);
    }
    m_RenderObjects[RG_NONBLEND].clear();

    for (auto& pRenderObj : m_RenderObjects[RG_NONPICKING])
    {
        if (nullptr != pRenderObj)
            pRenderObj->Render();

        SafeRelease(pRenderObj);
    }
    m_RenderObjects[RG_NONPICKING].clear();

    if (FAILED(m_pGameInst->EndMRT())) 
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::RenderLights()
{
    if (FAILED(m_pGameInst->BeginMRT(TEXT("MRTLightAcc"))))
        return E_FAIL;

    // 행렬들을 바인딩한다.
    m_pShader->BindMatrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->BindMatrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->BindMatrix("g_ProjMatrix", &m_ProjMatrix);
    

    // 뷰스페이스 변환 행렬의 역행렬, 원근 투영 변환 행렬의 역행렬을 바인딩한다. (241128)
    {
        _float4x4 ViewInv = m_pGameInst->GetTransformMatrixFloat4x4Inverse(CPipeLine::D3DTS_VIEW);
        m_pShader->BindMatrix("g_ViewMatrixInv", &ViewInv);

        _float4x4 ProjInv = m_pGameInst->GetTransformMatrixFloat4x4Inverse(CPipeLine::D3DTS_PROJ);
        m_pShader->BindMatrix("g_ProjMatrixInv", &ProjInv);
    }


    m_pShader->BindRawValue("g_vCamPosition", m_pGameInst->GetCameraPosition(), sizeof _float4);

    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShader, "g_NormalTexture", TEXT("TargetNormal"))))
        return E_FAIL;

    // 깊이기록을 한 DepthTexture 바인딩
    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShader, "g_DepthTexture", TEXT("TargetDepth"))))
        return E_FAIL;

    m_pVIBuffer->BindBufferDesc();

    // LightManager에 현재 자신의 Shader, VIBuffer를 넘겨준다.
    if (FAILED(m_pGameInst->RenderLights(m_pShader, m_pVIBuffer)))
        return E_FAIL;

    if (FAILED(m_pGameInst->EndMRT()))
        return E_FAIL;
    
    return S_OK;
}

// Render Final
HRESULT CRenderer::RenderShade()
{   
    if (m_bAdjustBlur)
    {
        if (FAILED(m_pGameInst->BeginMRT(TEXT("MRTFinal"), nullptr, false)))
            return E_FAIL;
    }

    m_pShader->BindMatrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->BindMatrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->BindMatrix("g_ProjMatrix", &m_ProjMatrix);

    _float4x4 ViewInv = m_pGameInst->GetTransformMatrixFloat4x4Inverse(CPipeLine::D3DTS_VIEW);
    _float4x4 ProjInv = m_pGameInst->GetTransformMatrixFloat4x4Inverse(CPipeLine::D3DTS_PROJ);
    m_pShader->BindMatrix("g_ViewMatrixInv", &ViewInv);
    m_pShader->BindMatrix("g_ProjMatrixInv", &ProjInv);

    m_pShader->BindMatrix("g_LightViewMatrix", m_pGameInst->GetShadowTransform(CShadow::D3DTS_VIEW));
    m_pShader->BindMatrix("g_LightProjMatrix", m_pGameInst->GetShadowTransform(CShadow::D3DTS_PROJ));

    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShader, "g_DiffuseTexture", TEXT("TargetDiffuse"))))
        return E_FAIL;

    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShader, "g_ShadeTexture", TEXT("TargetShade"))))
        return E_FAIL;

    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShader, "g_SpecularTexture", TEXT("TargetSpecular"))))
        return E_FAIL;

    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShader, "g_LightDepthTexture", TEXT("TargetLightDepth"))))
        return E_FAIL;

    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShader, "g_DepthTexture", TEXT("TargetDepth"))))
        return E_FAIL;

    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShader, "g_OuterTexture", TEXT("TargetOuter"))))
        return E_FAIL;

    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShader, "g_TargetGlowFinal", TEXT("TargetGlowFinal"))))
        return E_FAIL;

    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShader, "g_TargetDistortion", TEXT("TargetDistortion"))))
        return E_FAIL;

    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShader, "g_NormalTexture", TEXT("TargetNormal"))))
        return E_FAIL;

    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShader, "g_FinalFinalTexture", TEXT("TargetFinal"))))
        return E_FAIL;

    m_pShader->Begin(3);

    m_pVIBuffer->BindBufferDesc();

    m_pVIBuffer->Render();


    /*if (FAILED(m_pGameInst->EndMRT()))
        return E_FAIL;*/

    if (m_bAdjustBlur) 
    {
        if (FAILED(m_pGameInst->EndMRT()))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CRenderer::RenderBlurX()
{
    if (FAILED(m_pGameInst->BeginMRT(TEXT("MRTBlurX"))))
        return E_FAIL;

    m_pShader->BindMatrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->BindMatrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->BindMatrix("g_ProjMatrix", &m_ProjMatrix);

    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShader, "g_FinalTexture", TEXT("TargetFinal"))))
        return E_FAIL;

    m_pShader->Begin(4);
    m_pVIBuffer->BindBufferDesc();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInst->EndMRT()))
        return E_FAIL;
    return S_OK;
}

HRESULT CRenderer::RenderBlurY()
{
    m_pShader->BindMatrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->BindMatrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->BindMatrix("g_ProjMatrix", &m_ProjMatrix);

    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShader, "g_BlurXTexture", TEXT("TargetBlurX"))))
        return E_FAIL;
    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShader, "g_FinalTexture", TEXT("TargetFinal"))))
        return E_FAIL;

    m_pShader->Begin(5);
    m_pVIBuffer->BindBufferDesc();
    m_pVIBuffer->Render();

    return S_OK;
}

HRESULT CRenderer::RenderOuter()
{
    if (FAILED(m_pGameInst->BeginMRT(TEXT("MRTOuter"))))
        return E_FAIL;

    for (auto& pRenderObj : m_RenderObjects[RG_OUTER])
    {
        if (nullptr != pRenderObj)
            pRenderObj->Render();

        SafeRelease(pRenderObj);
    }

    m_RenderObjects[RG_OUTER].clear();

    m_pShader->BindMatrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->BindMatrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->BindMatrix("g_ProjMatrix", &m_ProjMatrix);

    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShader, "g_OuterPreTexture", TEXT("TargetOuterPre"))))
        return E_FAIL;

    m_pShader->Begin(6);
    m_pVIBuffer->BindBufferDesc();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInst->EndMRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::RenderGlow()
{
    if (FAILED(m_pGameInst->BeginMRT(TEXT("MRTGlow"))))
        return E_FAIL;

    for (auto& pRenderObj : m_RenderObjects[RG_GLOW])
    {
        if (nullptr != pRenderObj)
            pRenderObj->Render();

        SafeRelease(pRenderObj);
    }
    m_RenderObjects[RG_GLOW].clear();

    /*m_pShader->Begin(7);
    m_pVIBuffer->BindBufferDesc();
    m_pVIBuffer->Render();*/

    if (FAILED(m_pGameInst->EndMRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::RenderGlowCopy()
{
    _uint iNumViewPorts = 1;
    D3D11_VIEWPORT ViewPortDesc = {};
    m_pContext->RSGetViewports(&iNumViewPorts, &ViewPortDesc);
    SetUpViewPort((_uint)(ViewPortDesc.Width / 2), (_uint)(ViewPortDesc.Height / 2));

    if (FAILED(m_pGameInst->BeginMRT(TEXT("MRTGlowCopy"), m_pGlowDepthStencilView)))
        return E_FAIL;

    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShader, "g_TargetGlow", TEXT("TargetGlow"))))
        return E_FAIL;

    m_pShader->Begin(8);
    m_pVIBuffer->BindBufferDesc();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInst->EndMRT()))
        return E_FAIL;

    SetUpViewPort((_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height);

    return S_OK;
}

HRESULT CRenderer::RenderGlowBlurX()
{
    _uint iNumViewPorts = 1;
    D3D11_VIEWPORT ViewPortDesc = {};
    m_pContext->RSGetViewports(&iNumViewPorts, &ViewPortDesc);

    SetUpViewPort(_uint(ViewPortDesc.Width / 2), _uint(ViewPortDesc.Height / 2));

    if (FAILED(m_pGameInst->BeginMRT(TEXT("MRTGlowBlurX"), m_pGlowDepthStencilView)))
        return E_FAIL;

    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShader, "g_TargetGlowCopy", TEXT("TargetGlowCopy"))))
        return E_FAIL;

    m_pShader->Begin(9);
    m_pVIBuffer->BindBufferDesc();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInst->EndMRT()))
        return E_FAIL;

    SetUpViewPort(_uint(ViewPortDesc.Width), _uint(ViewPortDesc.Height));

    return S_OK;
}

HRESULT CRenderer::RenderGlowBlurY()
{
    _uint iNumViewPorts = 1;
    D3D11_VIEWPORT ViewPortDesc = {};
    m_pContext->RSGetViewports(&iNumViewPorts, &ViewPortDesc);

    SetUpViewPort(_uint(ViewPortDesc.Width / 2), _uint(ViewPortDesc.Height / 2));

    if (FAILED(m_pGameInst->BeginMRT(TEXT("MRTGlowBlurY"), m_pGlowDepthStencilView)))
        return E_FAIL;

    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShader, "g_TargetGlowCopyBlurX", TEXT("TargetGlowCopyBlurX"))))
        return E_FAIL;

    m_pShader->Begin(10);
    m_pVIBuffer->BindBufferDesc();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInst->EndMRT()))
        return E_FAIL;

    SetUpViewPort(_uint(ViewPortDesc.Width), _uint(ViewPortDesc.Height));

    return S_OK;
}

HRESULT CRenderer::RenderGlowFinal()
{
    if (FAILED(m_pGameInst->BeginMRT(TEXT("MRTGlowFinal"))))
        return E_FAIL;

    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShader, "g_TargetGlowCopyBlurY", TEXT("TargetGlowCopyBlurY"))))
        return E_FAIL;

    m_pShader->Begin(11);
    m_pVIBuffer->BindBufferDesc();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInst->EndMRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::RenderDistortion()
{
    if (FAILED(m_pGameInst->BeginMRT(TEXT("MRTDistortion"))))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[RG_DISTORTION])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        SafeRelease(pRenderObject);
    }
    m_RenderObjects[RG_DISTORTION].clear();

    if (FAILED(m_pGameInst->EndMRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::RenderNonLight()
{
    for (auto& pRenderObject : m_RenderObjects[RG_NONLIGHT])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        SafeRelease(pRenderObject);
    }
    m_RenderObjects[RG_NONLIGHT].clear();

    return S_OK;
}

HRESULT CRenderer::RenderBlend()
{
    // 알파 블렌딩을 해야하는 객체들을 카메라 기준 Z값을 기준으로 하여 정렬을 수행한다. : Alpha Sorting
    m_RenderObjects[RG_BLEND].sort([](CGameObject* pSrc, CGameObject* pDst)
        {
            return static_cast<CAlphaObject*>(pSrc)->GetViewZ() > static_cast<CAlphaObject*>(pDst)->GetViewZ();
        });

    for (auto& pRenderObj : m_RenderObjects[RG_BLEND])
    {
        if (nullptr != pRenderObj)
            pRenderObj->Render();

        SafeRelease(pRenderObj);
    }

    m_RenderObjects[RG_BLEND].clear();

    return S_OK;
}

HRESULT CRenderer::RenderUI()
{
    m_RenderObjects[RG_UI].sort([](CGameObject* pSrc, CGameObject* pDst)
        {
            return static_cast<CUIObject*>(pSrc)->GetDepth() > static_cast<CUIObject*>(pDst)->GetDepth();
        });

    for (auto& pRenderObj : m_RenderObjects[RG_UI])
    {

        if (nullptr != pRenderObj)
            pRenderObj->Render();

        SafeRelease(pRenderObj);
    }

    m_RenderObjects[RG_UI].clear();

    return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderer::RenderRTVDebug()
{
    /*if (FAILED(m_pShader->BindMatrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->BindMatrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;
    if (FAILED(m_pVIBuffer->BindBufferDesc()))
        return E_FAIL;*/

   /* if (FAILED(m_pGameInst->RenderRTVDebug(TEXT("MRTGameObjects"), m_pShader, m_pVIBuffer)))
        return E_FAIL;*/

    //if (FAILED(m_pGameInst->RenderRTVDebug(TEXT("MRTLightAcc"), m_pShader, m_pVIBuffer)))
    //    return E_FAIL;
    //// if (FAILED(m_pGameInst->RenderRTVDebug(TEXT("MRTHeight"), m_pShader, m_pVIBuffer)))
    //    // return E_FAIL;


    //if (FAILED(m_pGameInst->RenderRTVDebug(TEXT("MRTShadow"), m_pShader, m_pVIBuffer)))
    //    return E_FAIL;

    // Outer
   /* if (FAILED(m_pGameInst->RenderRTVDebug(TEXT("MRTOuter"), m_pShader, m_pVIBuffer)))
        return E_FAIL;*/

    // Glow 디버그 렌더 타겟
    //if (FAILED(m_pGameInst->RenderRTVDebug(TEXT("MRTGlow"), m_pShader, m_pVIBuffer)))
    //    return E_FAIL;

    // 아래 주석을 해제하면 단계적으로 디버그 렌더 타겟을 볼 수 있다.
    // if (FAILED(m_pGameInst->RenderRTVDebug(TEXT("MRTGlowCopy"), m_pShader, m_pVIBuffer))) return E_FAIL;
    // if (FAILED(m_pGameInst->RenderRTVDebug(TEXT("MRTGlowBlurX"), m_pShader, m_pVIBuffer))) return E_FAIL;
    // if (FAILED(m_pGameInst->RenderRTVDebug(TEXT("MRTGlowBlurY"), m_pShader, m_pVIBuffer))) return E_FAIL;
    // if (FAILED(m_pGameInst->RenderRTVDebug(TEXT("MRTGlowFinal"), m_pShader, m_pVIBuffer))) return E_FAIL;

    /*if (FAILED(m_pGameInst->RenderRTVDebug(TEXT("MRTDistortion"), m_pShader, m_pVIBuffer)))
        return E_FAIL;*/

    //if (FAILED(m_pGameInst->RenderRTVDebug(TEXT("MRTFinal"), m_pShader, m_pVIBuffer)))
    //    return E_FAIL;

    //if (FAILED(m_pGameInst->RenderRTVDebug(TEXT("MRTLightAcc"), m_pShader, m_pVIBuffer)))
    //    return E_FAIL;

    return S_OK;
}

#endif

HRESULT CRenderer::ReadyDepthStencilView(_uint iWidth, _uint iHeight)
{
    if (nullptr == m_pDevice)
        return E_FAIL;

    ID3D11Texture2D*        pDST = nullptr;
    D3D11_TEXTURE2D_DESC    TexDesc;
    ZeroMemory(&TexDesc, sizeof D3D11_TEXTURE2D_DESC);

    TexDesc.Width = iWidth;
    TexDesc.Height = iHeight;
    TexDesc.MipLevels = 1;
    TexDesc.ArraySize = 1;
    TexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    TexDesc.SampleDesc.Quality = 0;
    TexDesc.SampleDesc.Count = 1;
    TexDesc.Usage = D3D11_USAGE_DEFAULT;
    TexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    TexDesc.CPUAccessFlags = 0;
    TexDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TexDesc, nullptr, &pDST)))
        return E_FAIL;
    
    if (FAILED(m_pDevice->CreateDepthStencilView(pDST, nullptr, &m_pMaxDepthStencilView)))
        return E_FAIL;
    
    SafeRelease(pDST);

    return S_OK;
}

HRESULT CRenderer::ReadyDepthStencilView(_uint iWidth, _uint iHeight, ID3D11DepthStencilView* pDSV)
{
    if (nullptr == m_pDevice)
        return E_FAIL;

    ID3D11Texture2D* pDST = nullptr;
    D3D11_TEXTURE2D_DESC    TexDesc;
    ZeroMemory(&TexDesc, sizeof D3D11_TEXTURE2D_DESC);

    TexDesc.Width = iWidth;
    TexDesc.Height = iHeight;
    TexDesc.MipLevels = 1;
    TexDesc.ArraySize = 1;
    TexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    TexDesc.SampleDesc.Quality = 0;
    TexDesc.SampleDesc.Count = 1;
    TexDesc.Usage = D3D11_USAGE_DEFAULT;
    TexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    TexDesc.CPUAccessFlags = 0;
    TexDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TexDesc, nullptr, &pDST)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateDepthStencilView(pDST, nullptr, &pDSV)))
        return E_FAIL;

    SafeRelease(pDST);

    return S_OK;
}

HRESULT CRenderer::SetUpViewPort(_uint iWidth, _uint iHeight)
{
    D3D11_VIEWPORT  Desc = {};
    ZERO(&Desc, sizeof D3D11_VIEWPORT);
    Desc.TopLeftX = 0;
    Desc.TopLeftY = 0;
    Desc.Width = (_float)iWidth;
    Desc.Height = (_float)iHeight;
    Desc.MinDepth = 0.f;
    Desc.MaxDepth = 1.f;

    m_pContext->RSSetViewports(1, &Desc);

    return S_OK;
}

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRenderer* pInst = new CRenderer(pDevice, pContext);

    if (FAILED(pInst->Initialize()))
    {
        MSG_BOX("Failed to Created : CRenderer");
        SafeRelease(pInst);
    }

    return pInst;
}

void CRenderer::Free()
{
    __super::Free();

    for (_uint i = 0; i < RG_END; ++i)
    {
        for (auto& pRenderObj : m_RenderObjects[i])
            SafeRelease(pRenderObj);

        m_RenderObjects[i].clear();
    }

    SafeRelease(m_pDevice);
    SafeRelease(m_pContext);
    SafeRelease(m_pGameInst);
    SafeRelease(m_pShader);
    SafeRelease(m_pVIBuffer);
    SafeRelease(m_pMaxDepthStencilView);
}