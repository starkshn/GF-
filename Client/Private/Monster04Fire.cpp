#include "pch.h"
#include "Monster04Fire.h"
#include "GameInstance.h"

CMonster04Fire::CMonster04Fire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CEffect(pDevice, pContext)
{
}

CMonster04Fire::CMonster04Fire(const CMonster04Fire& Prototype)
    :
    CEffect(Prototype)
{
}

HRESULT CMonster04Fire::InitializePrototype()
{
    return S_OK;
}

HRESULT CMonster04Fire::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    return S_OK;
}

void CMonster04Fire::PriorityUpdate(_float fTimeDelta)
{

}

void CMonster04Fire::Update(_float fTimeDelta)
{
    if (IsActive())
        __super::Update(fTimeDelta);

    /*if (m_bMove)
    {
        _vector vPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
        vPos = vPos + m_vMoveDir * m_pTransformCom->GetSpeed() * fTimeDelta;
        m_pTransformCom->SetTransformState(CTransform::STATE_POS, vPos);
    }*/
}

void CMonster04Fire::LateUpdate(_float fTimeDelta)
{
    if (IsActive())
        m_pGameInst->AddRenderObject(CRenderer::RG_GLOW, this);
}

HRESULT CMonster04Fire::Render()
{
    if (IsActive())
    {
        if (FAILED(BindShaderResources()))
            return E_FAIL;

        // pass 13¹ø
        m_pShaderCom->Begin(14);

        m_pVIBufferCom->BindBufferDesc();

        m_pVIBufferCom->Render();
    }

    return S_OK;
}

HRESULT CMonster04Fire::ReadyComponents()
{
    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Monster04Fire"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster04Fire::BindShaderResources()
{
    if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    _float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
    _float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

    if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix)))
        return E_FAIL;


    if (FAILED(m_pTextureCom->BindShaderResource(m_pShaderCom, "g_Texture", static_cast<_uint>(m_fFrame))))
        return E_FAIL;

    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShaderCom, "g_DepthTexture", TEXT("TargetDepth"))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindRawValue("g_MaxLifeTime", &m_fMaxLifeTime, sizeof _float)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindRawValue("g_CurLifeTime", &m_fAccLifeTime, sizeof _float)))
        return E_FAIL;


    return S_OK;
}

CMonster04Fire* CMonster04Fire::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMonster04Fire* pInstance = new CMonster04Fire(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CMonster04Fire");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CMonster04Fire::Clone(void* pArg)
{
    CMonster04Fire* pInstance = new CMonster04Fire(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CMonster04Fire");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CMonster04Fire::Free()
{
    __super::Free();
}
