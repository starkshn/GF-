#include "pch.h"
#include "Monster03Fire.h"
#include "GameInstance.h"

CMonster03Fire::CMonster03Fire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CEffect(pDevice, pContext)
{
}

CMonster03Fire::CMonster03Fire(const CMonster03Fire& Prototype)
    :
    CEffect(Prototype)
{
}

HRESULT CMonster03Fire::InitializePrototype()
{
    return S_OK;
}

HRESULT CMonster03Fire::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;


    return S_OK;
}

void CMonster03Fire::PriorityUpdate(_float fTimeDelta)
{
    
}

void CMonster03Fire::Update(_float fTimeDelta)
{
    if (IsActive())
    {
        __super::Update(fTimeDelta);
        // BillBoard();
        // m_pTransformCom->Scaling(3.f, 3.f, 3.f);
    }

    /*if (m_bMove)
    {
        _vector vPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
        vPos = vPos + m_vMoveDir * m_pTransformCom->GetSpeed() * fTimeDelta;
        m_pTransformCom->SetTransformState(CTransform::STATE_POS, vPos);
    }*/
}

void CMonster03Fire::LateUpdate(_float fTimeDelta)
{
    if (IsActive())
        m_pGameInst->AddRenderObject(CRenderer::RG_GLOW, this);
}

HRESULT CMonster03Fire::Render()
{
    if (IsActive())
    {
        if (FAILED(BindShaderResources()))
            return E_FAIL;

        // pass 12¹ø monster03_fire
        m_pShaderCom->Begin(13);

        m_pVIBufferCom->BindBufferDesc();

        m_pVIBufferCom->Render();
    }

    return S_OK;
}

HRESULT CMonster03Fire::ReadyComponents()
{
    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_Monster03Fire"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster03Fire::BindShaderResources()
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

CMonster03Fire* CMonster03Fire::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMonster03Fire* pInstance = new CMonster03Fire(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CMonster03Fire");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CMonster03Fire::Clone(void* pArg)
{
    CMonster03Fire* pInstance = new CMonster03Fire(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CMonster03Fire");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CMonster03Fire::Free()
{
    __super::Free();
}
