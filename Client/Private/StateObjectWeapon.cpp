#include "pch.h"
#include "StateObjectWeapon.h"
#include "GameInstance.h"
#include "Model.h"
#include "StateObjectContainer.h"

CStateObjectWeapon::CStateObjectWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CPartObject(pDevice, pContext)
{

}

CStateObjectWeapon::CStateObjectWeapon(const CStateObjectWeapon& Prototype)
    :
    CPartObject(Prototype)
{

}

HRESULT CStateObjectWeapon::InitializePrototype()
{
    return S_OK;
}

HRESULT CStateObjectWeapon::Initialize(void* pArg)
{
    if (nullptr != pArg)
    {
        STATE_OBJECT_WEAPON_DESC* pDesc = static_cast<STATE_OBJECT_WEAPON_DESC*>(pArg);
        if (nullptr != pDesc)
        {
            m_pOwner = pDesc->pOwner;
            m_pState = pDesc->pState;
            m_pSocketMatrix = pDesc->pSocketMatrix;
        }

    }

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CStateObjectWeapon::Free()
{
    __super::Free();

    if (nullptr != m_pShaderCom)
        SafeRelease(m_pShaderCom);

    if (nullptr != m_pModelCom)
        SafeRelease(m_pModelCom);

    if (nullptr != m_pColliderCom)
        SafeRelease(m_pColliderCom);

    if (nullptr != m_pOwner)
        SafeRelease(m_pOwner);
}
