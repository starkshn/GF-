#include "pch.h"
#include "SwordTrail.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "PartObject.h"

CSwordTrail::CSwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CEffectTrail(pDevice, pContext)
{

}

CSwordTrail::CSwordTrail(const CSwordTrail& Prototype)
    :
    CEffectTrail(Prototype)
{

}

HRESULT CSwordTrail::InitializePrototype()
{
    return S_OK;
}

HRESULT CSwordTrail::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    return S_OK;
}

void CSwordTrail::PriorityUpdate(_float fTimeDelta)
{

}

void CSwordTrail::Update(_float fTimeDelta)
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

void CSwordTrail::LateUpdate(_float fTimeDelta)
{
    if (IsActive())
        m_pGameInst->AddRenderObject(CRenderer::RG_GLOW, this);
}

HRESULT CSwordTrail::Render()
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

HRESULT CSwordTrail::ReadyComponents()
{
    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex_Time"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    CVIBufferTrail::TRAIL_DESC Desc = {};
    Desc.fTrailLength = 1.f;
    Desc.iNumMaxVertices = 100;
    Desc.fOffset = 5.f;
    Desc.fLifeTime = 0.15f;
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Trail"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferTrailCom), &Desc)))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_SwordTrail"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CSwordTrail::BindShaderResources()
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

CSwordTrail* CSwordTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSwordTrail* pInstance = new CSwordTrail(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CSwordTrail");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CSwordTrail::Clone(void* pArg)
{
    CSwordTrail* pInstance = new CSwordTrail(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CSwordTrail");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CSwordTrail::Free()
{
    __super::Free();
}
