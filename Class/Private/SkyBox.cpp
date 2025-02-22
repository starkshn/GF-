#include "SkyBox.h"
#include "GameInstance.h"

CSkyBox::CSkyBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CGameObject(pDevice, pContext)
{
}

CSkyBox::CSkyBox(const CSkyBox& Prototype)
    :
    CGameObject(Prototype)
{
}

HRESULT CSkyBox::InitializePrototype()
{
    return S_OK;
}

HRESULT CSkyBox::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    return S_OK;
}

void CSkyBox::PriorityUpdate(_float fTimeDelta)
{

}

void CSkyBox::Update(_float fTimeDelta)
{
    m_pTransformCom->SetTransformState(CTransform::STATE_POS, XMLoadFloat4(m_pGameInst->GetCameraPosition()));
}

void CSkyBox::LateUpdate(_float fTimeDelta)
{
    // 가장 먼저 그릴 것이기 때문에 RG_PRIORITY에 넣는다. (당연)
    m_pGameInst->AddRenderObject(CRenderer::RG_PRIORITY, this);
}

HRESULT CSkyBox::Render()
{
    if (FAILED(BindShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->BindBufferDesc();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CSkyBox::ReadyComponents()
{
    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxCube_SkyBox"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_SkyBox"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Texture_SkyBox"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CSkyBox::BindShaderResources()
{
    if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    _float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
    _float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

    if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->BindShaderResource(m_pShaderCom, "g_Texture", 3)))
        return E_FAIL;

    return S_OK;
}

CSkyBox* CSkyBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSkyBox* pInstance = new CSkyBox(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CSkyBox");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CSkyBox::Clone(void* pArg)
{
    CSkyBox* pInstance = new CSkyBox(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CSkyBox");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CSkyBox::Free()
{
    __super::Free();

    SafeRelease(m_pShaderCom);
    SafeRelease(m_pTextureCom);
    SafeRelease(m_pVIBufferCom);
}
