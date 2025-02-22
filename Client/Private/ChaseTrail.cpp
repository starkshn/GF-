#include "pch.h"
#include "ChaseTrail.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "PartObject.h"

CChaseTrail::CChaseTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CEffectTrail(pDevice, pContext)
{
}

CChaseTrail::CChaseTrail(const CChaseTrail& Prototype)
    :
    CEffectTrail(Prototype)
{
}

HRESULT CChaseTrail::InitializePrototype()
{
    return S_OK;
}

HRESULT CChaseTrail::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    return S_OK;
}

void CChaseTrail::PriorityUpdate(_float fTimeDelta)
{

}

void CChaseTrail::Update(_float fTimeDelta)
{
    if (IsActive())
    {
        __super::Update(fTimeDelta);

        if (m_pOwner)
        {
            if (CPartObject* pPartObj = dynamic_cast<CPartObject*>(m_pOwner))
            {
                _matrix WorldMatrix = pPartObj->GetSocketWorldMatrix();
                m_pVIBufferTrailCom->AddVertices(WorldMatrix, fTimeDelta);
            }
            else if (CGameObject* pObj = dynamic_cast<CGameObject*>(m_pOwner))
            {
                _matrix WorldMatrix = static_cast<CTransform*>(pObj->GetComponent(TEXT("Com_Transform")))->GetWorldMatrix();
                m_pVIBufferTrailCom->AddVertices(WorldMatrix, fTimeDelta);
            }
        }

        if (!IsActive())
        {
            m_pVIBufferTrailCom->ResetTrailBuffers();
        }
    }
}

void CChaseTrail::LateUpdate(_float fTimeDelta)
{
    if (IsActive())
        m_pGameInst->AddRenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CChaseTrail::Render()
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

HRESULT CChaseTrail::ReadyComponents()
{
    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex_Time"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    CVIBufferTrail::TRAIL_DESC Desc = {};
    Desc.fTrailLength = 1.f;
    Desc.iNumMaxVertices = 50;
    Desc.fOffset = 10.f;
    Desc.fLifeTime = 0.25f;
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Trail"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferTrailCom), &Desc)))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_ChaseTrail"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CChaseTrail::BindShaderResources()
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

CChaseTrail* CChaseTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CChaseTrail* pInstance = new CChaseTrail(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CChaseTrail");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CChaseTrail::Clone(void* pArg)
{
    CChaseTrail* pInstance = new CChaseTrail(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CChaseTrail");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CChaseTrail::Free()
{
    __super::Free();
}
