#include "AABB.h"
#include "BoundingAABB.h"
#include "GameInstance.h"

CAABB::CAABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CGameObject(pDevice, pContext)
{
}

CAABB::CAABB(const CAABB& Prototype)
    :
    CGameObject(Prototype)
{
    
}

HRESULT CAABB::InitializePrototype()
{
    return S_OK;
}

HRESULT CAABB::Initialize(void* pArg)
{
    if (__super::Initialize(pArg))
        return E_FAIL;

    if (ReadyComponents())
        return E_FAIL;


    return S_OK;
}

void CAABB::PriorityUpdate(_float fTimeDelta)
{

}

void CAABB::Update(_float fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->GetWorldMatrix());
}

void CAABB::LateUpdate(_float fTimeDelta)
{
    m_pGameInst->AddRenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CAABB::Render()
{
    m_pColliderCom->Render();

    return S_OK;
}

HRESULT CAABB::ReadyComponents()
{
    // Com_Collider
    {
        CBoundingAABB::BOUND_AABB_DESC		ColliderDesc{};
        ColliderDesc.vExtents = _float3(0.5f, 0.5f, 0.5f);
        ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

        if (FAILED(AddComponent(LEVEL_TOOL, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
            return E_FAIL;
    }

    return S_OK;
}

CAABB* CAABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAABB* pInstance = new CAABB(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CAABB");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CAABB::Clone(void* pArg)
{
    CAABB* pInstance = new CAABB(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CAABB");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CAABB::Free()
{
    __super::Free();

    SafeRelease(m_pColliderCom);
}
