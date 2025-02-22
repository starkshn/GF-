#include "PartObject.h"

CPartObject::CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CGameObject(pDevice, pContext)
{
}

CPartObject::CPartObject(const CPartObject& Prototype)
    :
    CGameObject(Prototype)
{
}

HRESULT CPartObject::InitializePrototype()
{
    return S_OK;
}

HRESULT CPartObject::Initialize(void* pArg)
{
    PARTOBJECT_DESC* pDesc = static_cast<PARTOBJECT_DESC*>(pArg);
    if (nullptr != pDesc)
    {
        m_pParentMatrix = pDesc->pParentMatrix;
    }

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    return S_OK;
}

void CPartObject::PriorityUpdate(_float fTimeDelta)
{
}

void CPartObject::Update(_float fTimeDelta)
{
}

void CPartObject::LateUpdate(_float fTimeDelta)
{
}

HRESULT CPartObject::Render()
{
    return S_OK;
}

void CPartObject::Free()
{
    __super::Free();
}
