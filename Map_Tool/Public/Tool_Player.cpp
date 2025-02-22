#include "Tool_Player.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Transform.h"
#include "Collider.h"
#include "ColBoundingBox.h"
#include "Navigation.h"

CTool_Player::CTool_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CGameObject(pDevice, pContext)
{
}

CTool_Player::CTool_Player(const CTool_Player& Prototype)
    :
    CGameObject(Prototype)
{
}

HRESULT CTool_Player::InitializePrototype()
{
    return S_OK;
}

HRESULT CTool_Player::Initialize(void* pArg)
{
    if (__super::Initialize(pArg))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->SetSpeed(1.f);

    return S_OK;
}

void CTool_Player::PriorityUpdate(_float fTimeDelta)
{
    
}

void CTool_Player::Update(_float fTimeDelta)
{
    m_pModelComs[m_iCurrentModelIndex]->PlayAnimation(fTimeDelta);

    m_pBoundingBox->Update_Bouding_Box(m_pTransformCom);

    m_pColliderCom->Update(m_pTransformCom->GetWorldMatrix());


    m_pNavCom->Update(XMMatrixIdentity());
}

void CTool_Player::LateUpdate(_float fTimeDelta)
{
    m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTool_Player::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    // 현재 모델이 사용하고 있는 전체 메쉬수를 가지고 온다.
    _uint		iNumMeshes = m_pModelComs[m_iCurrentModelIndex]->GetNumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        // i 번째에 해당하는 텍스쳐를 Bind 해준다.
        if (FAILED(m_pModelComs[m_iCurrentModelIndex]->BindMaterial(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
            return E_FAIL;

        if (FAILED(m_pModelComs[m_iCurrentModelIndex]->BindBoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        // 0 default, 1 wireframe
        m_pShaderCom->Begin(0);

        m_pModelComs[m_iCurrentModelIndex]->Render(i);
    }

    // 바운딩 박스 렌더
    {
        _float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
        _float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

        if (FAILED(m_pBoundingBox->Render_Bounding_Box(m_pTransformCom, XMLoadFloat4x4(&viewMatrix), XMLoadFloat4x4(&projMatrix))))
            return E_FAIL;
    }

    m_pColliderCom->Render();

    // m_pNavCom->Render();

    return S_OK;
}

HRESULT CTool_Player::Ready_Components()
{
    // Com_Shader
    {
        if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
            return E_FAIL;
    }

    // Com_Model
    {
        m_pModelComs.resize(m_iNumModels);

        // 기본 권총
        if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_1201"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelComs[0]))))
            return E_FAIL;

        // 맥크리 권총
        if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_1205"), TEXT("Com_Model_1"), reinterpret_cast<CComponent**>(&m_pModelComs[1]))))
            return E_FAIL;

        // 불꽃 비늘
        if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_1004"), TEXT("Com_Model_2"), reinterpret_cast<CComponent**>(&m_pModelComs[2]))))
            return E_FAIL;

        // 레인보우 아치
        if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_1003"), TEXT("Com_Model_3"), reinterpret_cast<CComponent**>(&m_pModelComs[3]))))
            return E_FAIL;

        // 대포 총
        if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_1401"), TEXT("Com_Model_4"), reinterpret_cast<CComponent**>(&m_pModelComs[4]))))
            return E_FAIL;

        if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_1008"), TEXT("Com_Model_5"), reinterpret_cast<CComponent**>(&m_pModelComs[5]))))
            return E_FAIL;


        if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_1503"), TEXT("Com_Model_6"), reinterpret_cast<CComponent**>(&m_pModelComs[6]))))
            return E_FAIL;


        if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Model_Player_1508"), TEXT("Com_Model_7"), reinterpret_cast<CComponent**>(&m_pModelComs[7]))))
            return E_FAIL;
    }

    // Com_BoudingBox
    {
        if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_BoundingBox"), TEXT("Com_BoundingBox"), reinterpret_cast<CComponent**>(&m_pBoundingBox))))
            return E_FAIL;
    }


    /* Com_Collider */
    CBoundingAABB::BOUND_AABB_DESC		ColliderDesc{};
    ColliderDesc.vExtents = _float3(0.3f, 0.7f, 0.3f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

    if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;


    CNavigation::NAVIGATION_DESC		NaviDesc{};

    NaviDesc.iStartIndex = 0;

    if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavCom), &NaviDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTool_Player::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->BindShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    _float4x4 viewMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_VIEW);
    _float4x4 projMatrix = m_pGameInst->GetTransformMatrixFloat4x4(CPipeLine::DTS::D3DTS_PROJ);

    if (FAILED(m_pShaderCom->BindMatrix("g_ViewMatrix", &viewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->BindMatrix("g_ProjMatrix", &projMatrix)))
        return E_FAIL;

    const LIGHT_DESC* pLightDesc = m_pGameInst->GetLightDesc(0);

    if (FAILED(m_pShaderCom->BindRawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->BindRawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->BindRawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->BindRawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->BindRawValue("g_vCamPosition", m_pGameInst->GetCameraPosition(), sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

CTool_Player* CTool_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTool_Player* pInst = new CTool_Player(pDevice, pContext);

    if (FAILED(pInst->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CTool_Player");
        SafeRelease(pInst);
    }

    return pInst;
}

CGameObject* CTool_Player::Clone(void* pArg)
{
    CTool_Player* pInst = new CTool_Player(*this);

    if (FAILED(pInst->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CTool_Player");
        SafeRelease(pInst);
    }

    return pInst;
}

void CTool_Player::Free()
{
    __super::Free();

    for (auto pModel : m_pModelComs)
        SafeRelease(pModel);
    m_pModelComs.clear();

    SafeRelease(m_pShaderCom);
    SafeRelease(m_pBoundingBox);
}
