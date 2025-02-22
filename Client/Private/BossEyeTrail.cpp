#include "pch.h"
#include "BossEyeTrail.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "PartObject.h"

CBossEyeTrail::CBossEyeTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CEffectTrail(pDevice, pContext)
{

}

CBossEyeTrail::CBossEyeTrail(const CBossEyeTrail& Prototype)
    :
    CEffectTrail(Prototype)
{

}

HRESULT CBossEyeTrail::InitializePrototype()
{
    return S_OK;
}

HRESULT CBossEyeTrail::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    return S_OK;
}

void CBossEyeTrail::PriorityUpdate(_float fTimeDelta)
{

}

void CBossEyeTrail::Update(_float fTimeDelta)
{
    if (IsActive())
    {
        __super::Update(fTimeDelta);

        if (m_pOwner)
        {
            if (CPartObject* pPartObj = dynamic_cast<CPartObject*>(m_pOwner))
            {
                _matrix MyWorld = m_pTransformCom->GetWorldMatrix();
                _matrix WorldMatrix = pPartObj->GetSocketWorldMatrix();
                m_pVIBufferTrailCom->AddVertices(MyWorld * WorldMatrix, fTimeDelta);
            }
            else if (CGameObject* pObj = dynamic_cast<CGameObject*>(m_pOwner))
            {

            }
        }

        if (!IsActive())
        {
            m_pVIBufferTrailCom->ResetTrailBuffers();
        }
    }
}

void CBossEyeTrail::LateUpdate(_float fTimeDelta)
{
    if (IsActive())
        m_pGameInst->AddRenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CBossEyeTrail::Render()
{
    if (IsActive())
    {
        if (FAILED(BindShaderResources()))
            return E_FAIL;

        m_pShaderCom->Begin(0);

        m_pVIBufferTrailCom->BindBufferDesc();

        m_pVIBufferTrailCom->Render();
    }


    return S_OK;
}

HRESULT CBossEyeTrail::ReadyComponents()
{
    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex_Time"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    CVIBufferTrail::TRAIL_DESC Desc = {};
    Desc.fTrailLength = 1.f;    // 
    Desc.iNumMaxVertices = 1000; // 길이 (많을 수록 길어짐)
    Desc.fOffset = 0.5f;
    Desc.fLifeTime = 0.5f;

    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Trail"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferTrailCom), &Desc)))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_BossEye"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBossEyeTrail::BindShaderResources()
{
    if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    _float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
    _float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

    if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->BindShaderResource(m_pShaderCom, "g_Texture")))
        return E_FAIL;

    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShaderCom, "g_DepthTexture", TEXT("TargetDepth"))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindRawValue("g_MaxLifeTime", &m_fMaxLifeTime, sizeof _float)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindRawValue("g_CurLifeTime", &m_fAccLifeTime, sizeof _float)))
        return E_FAIL;


    return S_OK;
}

CBossEyeTrail* CBossEyeTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBossEyeTrail* pInstance = new CBossEyeTrail(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CBossEyeTrail");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CBossEyeTrail::Clone(void* pArg)
{
    CBossEyeTrail* pInstance = new CBossEyeTrail(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CBossEyeTrail");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CBossEyeTrail::Free()
{
    __super::Free();
}
