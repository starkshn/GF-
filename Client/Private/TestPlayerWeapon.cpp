#include "pch.h"
#include "TestPlayerWeapon.h"

#include "GameInstance.h"
#include "TestPlayer.h"

#include "Model.h"

CTestPlayerWeapon::CTestPlayerWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CPartObject(pDevice, pContext)
{
}

CTestPlayerWeapon::CTestPlayerWeapon(const CTestPlayerWeapon& Prototype)
    :
    CPartObject(Prototype)
{
}

HRESULT CTestPlayerWeapon::InitializePrototype()
{
    return S_OK;
}

HRESULT CTestPlayerWeapon::Initialize(void* pArg)
{
    TESTWEAPON_DESC* pDesc = static_cast<TESTWEAPON_DESC*>(pArg);

    m_pState = pDesc->pState;
    m_pSocketMatrix = pDesc->pSocketMatrix;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    return S_OK;
}

void CTestPlayerWeapon::PriorityUpdate(_float fTimeDelta)
{
}

void CTestPlayerWeapon::Update(_float fTimeDelta)
{
    _matrix     SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    for (_int i = 0; i < 3; ++i)
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

    m_pTransformCom->Scaling(0.1f, 0.1f, 0.1f);
    m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
    m_pTransformCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(0.6f, 0.f, 0.f, 1.f));

    XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->GetWorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
}

void CTestPlayerWeapon::LateUpdate(_float fTimeDelta)
{
    m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTestPlayerWeapon::Render()
{
    if (FAILED(BindShaderResources())) return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->GetNumMeshes();
    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->BindMaterial(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0))) return E_FAIL;
        m_pShaderCom->Begin(0);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CTestPlayerWeapon::ReadyComponents()
{
    /* Com_Shader */
    if (FAILED(AddComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)))) return E_FAIL;

    /* Com_Model */
    if (FAILED(AddComponent(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom)))) return E_FAIL;

    return S_OK;
}

HRESULT CTestPlayerWeapon::BindShaderResources()
{
    if (FAILED(m_pShaderCom->BindMatrix("g_WorldMatrix", &m_WorldMatrix))) return E_FAIL;

    _float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
    _float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

    if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix))) return E_FAIL;
    if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix))) return E_FAIL;

    return S_OK;
}

CTestPlayerWeapon* CTestPlayerWeapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTestPlayerWeapon* pInstance = new CTestPlayerWeapon(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CTestPlayerWeapon");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CTestPlayerWeapon::Clone(void* pArg)
{
    CTestPlayerWeapon* pInstance = new CTestPlayerWeapon(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CTestPlayerWeapon");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CTestPlayerWeapon::Free()
{
    __super::Free();

    SafeRelease(m_pShaderCom);
    SafeRelease(m_pModelCom);
}
