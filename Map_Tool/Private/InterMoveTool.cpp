#include "InterMoveTool.h"
#include "BoundingAABB.h"
#include "GameInstance.h"

CInterMoveTool::CInterMoveTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CGameObject(pDevice, pContext)
{
}

CInterMoveTool::CInterMoveTool(const CInterMoveTool& Prototype)
    :
    CGameObject(Prototype)
{

}

HRESULT CInterMoveTool::InitializePrototype()
{
    return S_OK;
}

HRESULT CInterMoveTool::Initialize(void* pArg)
{
    if (__super::Initialize(pArg))
        return E_FAIL;

    if (ReadyComponents())
        return E_FAIL;

    return S_OK;
}

void CInterMoveTool::PriorityUpdate(_float fTimeDelta)
{

}

void CInterMoveTool::Update(_float fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->GetWorldMatrix());
}

void CInterMoveTool::LateUpdate(_float fTimeDelta)
{
    m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CInterMoveTool::Render()
{
    m_pColliderCom->Render();

    return S_OK;
}

HRESULT CInterMoveTool::ReadyComponents()
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

CInterMoveTool* CInterMoveTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInterMoveTool* pInstance = new CInterMoveTool(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CInterMoveTool");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CInterMoveTool::Clone(void* pArg)
{
    CInterMoveTool* pInstance = new CInterMoveTool(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CInterMoveTool");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CInterMoveTool::Free()
{
    __super::Free();

    SafeRelease(m_pColliderCom);
}
