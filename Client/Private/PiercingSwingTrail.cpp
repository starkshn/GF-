#include "pch.h"
#include "PiercingSwingTrail.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "PartObject.h"

CPiercingSwingTrail::CPiercingSwingTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CEffectTrail(pDevice, pContext)
{
}

CPiercingSwingTrail::CPiercingSwingTrail(const CPiercingSwingTrail& Prototype)
    :
    CEffectTrail(Prototype)
{
}

HRESULT CPiercingSwingTrail::InitializePrototype()
{
    return S_OK;
}

HRESULT CPiercingSwingTrail::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    return S_OK;
}

void CPiercingSwingTrail::PriorityUpdate(_float fTimeDelta)
{

}

void CPiercingSwingTrail::Update(_float fTimeDelta)
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
                m_pVIBufferTrailCom->AddVertices(WorldMatrix, fTimeDelta);
            }
            else if (CGameObject* pObj = dynamic_cast<CGameObject*>(m_pOwner))
            {

            }
        }

        if (!IsActive())
        {
            m_pVIBufferTrailCom->ResetTrailBuffers();
            m_pOwner = nullptr;
        }
    }
}

void CPiercingSwingTrail::LateUpdate(_float fTimeDelta)
{
    if (IsActive())
        m_pGameInst->AddRenderObject(CRenderer::RG_GLOW, this);
}

HRESULT CPiercingSwingTrail::Render()
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

HRESULT CPiercingSwingTrail::ReadyComponents()
{
    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex_Time"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    CVIBufferTrail::TRAIL_DESC Desc = {};
    Desc.fTrailLength = 1.f;
    Desc.iNumMaxVertices = 100;
    Desc.fOffset = 5.f;
    Desc.fLifeTime = 0.25f;
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Trail"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferTrailCom), &Desc)))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_PiercingSwingTrail"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CPiercingSwingTrail::BindShaderResources()
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

CPiercingSwingTrail* CPiercingSwingTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPiercingSwingTrail* pInstance = new CPiercingSwingTrail(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CPiercingSwingTrail");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CPiercingSwingTrail::Clone(void* pArg)
{
    CPiercingSwingTrail* pInstance = new CPiercingSwingTrail(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CPiercingSwingTrail");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CPiercingSwingTrail::Free()
{
    __super::Free();
}
