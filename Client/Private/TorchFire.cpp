#include "pch.h"
#include "TorchFire.h"
#include "GameInstance.h"

CTorchFire::CTorchFire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CEffect(pDevice, pContext)
{
}

CTorchFire::CTorchFire(const CTorchFire& Prototype)
    :
    CEffect(Prototype)
{
}

HRESULT CTorchFire::InitializePrototype()
{
    return S_OK;
}

HRESULT CTorchFire::Initialize(void* pArg)
{
    if (!pArg)
        return E_FAIL;

    TORCH_FIRE_DESC* pDesc = static_cast<TORCH_FIRE_DESC*>(pArg);
    m_fNoiseMax = pDesc->fNoiseMax;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    // Active();
    DeActive();

    m_pTransformCom->Scaling(5.f, 5.f, 5.f);

    return S_OK;
}

void CTorchFire::PriorityUpdate(_float fTimeDelta)
{

}

void CTorchFire::Update(_float fTimeDelta)
{
    if (IsActive())
    {
        // 빌보드 함수
        BillBoard();

        // m_fNoiseCur : 현재 노이즈 변화값
        // DeltaTime을 m_bReverse 여부에 따라 값을 누적하거나 감쇠합니다.
        // m_fNoiseCur가 m_fNoiseMax보다 크다면 m_bReverse를 반전시킵니다.
        if (!m_bReverse)
            m_fNoiseCur += fTimeDelta;
        else
            m_fNoiseCur -= fTimeDelta;

        if (m_fNoiseCur > m_fNoiseMax)
            m_bReverse == false ? m_bReverse = true : m_bReverse = false;
    }
}

void CTorchFire::LateUpdate(_float fTimeDelta)
{
    if (IsActive())
        m_pGameInst->AddRenderObject(CRenderer::RG_GLOW, this);
}

HRESULT CTorchFire::Render()
{
    if (IsActive())
    {
        if (FAILED(BindShaderResources()))
            return E_FAIL;

        m_pShaderCom->Begin(19);
        m_pVIBufferCom->BindBufferDesc();
        m_pVIBufferCom->Render();
    }

    return S_OK;
}

HRESULT CTorchFire::ReadyComponents()
{
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_TorchFire"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Texture_Effect_TorchFireNoise"), TEXT("Com_Texture_Noise"), reinterpret_cast<CComponent**>(&m_pTextureNoise))))
        return E_FAIL;
    
    return S_OK;
}

HRESULT CTorchFire::BindShaderResources()
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
    if (FAILED(m_pTextureNoise->BindShaderResource(m_pShaderCom, "g_TextureNoise")))
        return E_FAIL;
    if (FAILED(m_pGameInst->BindRTShaderResource(m_pShaderCom, "g_DepthTexture", TEXT("TargetDepth"))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->BindRawValue("g_fNoiseMaxTime", &m_fNoiseMax, sizeof _float)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->BindRawValue("g_fNoiseCurTime", &m_fNoiseCur, sizeof _float)))
        return E_FAIL;

    return S_OK;
}

CTorchFire* CTorchFire::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTorchFire* pInstance = new CTorchFire(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CTorchFire");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CTorchFire::Clone(void* pArg)
{
    CTorchFire* pInstance = new CTorchFire(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CTorchFire");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CTorchFire::Free()
{
    __super::Free();

    SafeRelease(m_pTextureNoise);
}
