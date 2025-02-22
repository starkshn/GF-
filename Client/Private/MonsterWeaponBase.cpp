#include "pch.h"
#include "MonsterWeaponBase.h"
#include "GameInstance.h"
#include "TestPlayer.h"
#include "Model.h"

CMonsterWeaponBase::CMonsterWeaponBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CStateObjectWeapon(pDevice, pContext)
{

}

CMonsterWeaponBase::CMonsterWeaponBase(const CMonsterWeaponBase& Prototype)
    :
    CStateObjectWeapon(Prototype)
{

}

HRESULT CMonsterWeaponBase::InitializePrototype()
{
    return S_OK;
}

HRESULT CMonsterWeaponBase::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    _uint iCurLevelID = GetOwnerLevelID();
    CCollider* pOtherCollider = static_cast<CCollider*>(m_pGameInst->GetComponent(iCurLevelID, TEXT("A"), TEXT("Com_Collider")));
    if (nullptr != pOtherCollider)
        m_pOtherCol = pOtherCollider;
    else
        return E_FAIL;

    return S_OK;
}

_uint CMonsterWeaponBase::GetOwnerLevelID()
{
    if (nullptr != m_pOwner)
        return static_cast<CMonsterBase*>(m_pOwner)->GetCurrentLevelID();

    return 0;
}

HRESULT CMonsterWeaponBase::FireProjectile(MONSTER_PROJECTILE_TYPE eType)
{
    if (m_pOwner)
    {
        _vector vWeaponWorldPos = GetSocketWorldMatrix().r[3];
        _vector vPlayerWorldPos = static_cast<CMonsterBase*>(m_pOwner)->GetPlayerPos();
        _vector vBulletDir = XMVector3Normalize(vPlayerWorldPos - vWeaponWorldPos);

        if (FAILED(CreateProjectile(eType, vBulletDir)))
            return E_FAIL;
    }
    else
    {
        MSG_BOX("CMonsterWeaponBase::FireProjectile Error Owner is nullptr");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CMonsterWeaponBase::CreateProjectile(MONSTER_PROJECTILE_TYPE eType, _fvector vBulletDir)
{
    CProjectile::PROJECTILE_DESC Desc = {};
    Desc.bUseSpawnPos = true;
    _vector WeaponPos = XMVectorSet(m_WorldMatrix.m[3][0], m_WorldMatrix.m[3][1], m_WorldMatrix.m[3][2], m_WorldMatrix.m[3][3]);
    XMStoreFloat3(&Desc.vSpawnPos, WeaponPos);
    Desc.strObjectTag = "MonsterBullet";
    XMStoreFloat3(&Desc.vDir, vBulletDir);
    Desc.fRotSpeedPerSec = 0.f;
    Desc.iLevelIndex = m_iLevelIndex;
    Desc.fSpeedPerSec = 25.f;

    if (FAILED(m_pGameInst->SpawnObject(LEVEL_GLOBAL, TEXT("Prototype_GameObject_MonsterBullet"), m_iLevelIndex, TEXT("Layer_MonsterBullet"), &Desc))) 
        return E_FAIL;

    CMonsterBullet* pBullet = static_cast<CMonsterBullet*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, m_iLevelIndex, TEXT("Layer_MonsterBullet")));

    if (pBullet)
    {
        pBullet->Fire();
        CCollider* pCol = static_cast<CCollider*>(pBullet->GetComponent(TEXT("Com_Collider")));
        if (pCol)
            pCol->SetComOwner(pBullet);
        else
        {
            MSG_BOX("CMonsterWeaponBase::CreateProjectile Error!!!");
            return E_FAIL;
        }
    }
    else
    {
        MSG_BOX("CMonsterWeaponBase::CreateProjectile Error!!!");
        return E_FAIL;
    }

    if (FAILED(ActiveProjectileEffect(eType, pBullet)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonsterWeaponBase::ActiveProjectileEffect(MONSTER_PROJECTILE_TYPE eType, CMonsterBullet* pBullet)
{
    if (!pBullet)
        return E_FAIL;

    HIT_RESULT Hit = {};
    _vector vWorldPos = GetSocketWorldMatrix().r[3];
    _vector vLook = static_cast<CTransform*>(m_pOwner->GetComponent(TEXT("Com_Transform")))->GetTransformState(CTransform::STATE_LOOK);
    XMStoreFloat3(&Hit.vHitPos, vWorldPos);
    XMStoreFloat3(&Hit.vNormal, vLook);

    switch (eType)
    {
    case Client::CMonsterWeaponBase::MPT_01:
    {
        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::MONSTER03_FIRE, Hit);
        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::MONSTER03_TRAIL, Hit, pBullet);
    }
        break;
    case Client::CMonsterWeaponBase::MPT_02:
    {
        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::MONSTER04_FIRE, Hit);
        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::MONSTER04_TRAIL, Hit, pBullet);
    }
        break;
    case Client::CMonsterWeaponBase::MPT_END:
        break;
    default:
        break;
    }

    return S_OK;
}

void CMonsterWeaponBase::Free()
{
    __super::Free();
}
