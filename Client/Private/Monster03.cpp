#include "pch.h"
#include "Monster03.h"
#include "GameInstance.h"
#include "Collider.h"
#include "Navigation.h"
#include "MonsterWeapon01.h"
#include "ParticleManager.h"

CMonster03::CMonster03(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CMonsterBase(pDevice, pContext)
{

}

CMonster03::CMonster03(const CMonster03& Prototype)
    :
    CMonsterBase(Prototype)
{

}

HRESULT CMonster03::InitializePrototype()
{
	return S_OK;
}

HRESULT CMonster03::Initialize(void* pArg)
{
    m_bCanRoll = true;
    m_fRollRange = 3.f;
    
    m_fAimingRange = 9.f;
    m_fMinDistanceFromTarget = 6.f;

    SetForceState(MONSTER_STATE::MS_DEACTIVE);
    m_iPrevState = MONSTER_STATE::MS_DEACTIVE;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    if (FAILED(ReadyPartObjects()))
        return E_FAIL;

    return S_OK;
}

void CMonster03::PriorityUpdate(_float fTimeDelta)
{
    __super::PriorityUpdate(fTimeDelta);
}

void CMonster03::Update(_float fTimeDelta)
{
    m_bUpdateState = UpdateStateFarRangeAttackMonster(fTimeDelta);

    __super::Update(fTimeDelta);
    m_pColliderCom->Update(m_pTransformCom->GetWorldMatrix());
    PlayWalkSound(fTimeDelta);
}

void CMonster03::LateUpdate(_float fTimeDelta)
{
    __super::LateUpdate(fTimeDelta);
    m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);

    // CheckColPlayerBullet();

#ifdef _DEBUG
    //m_pGameInst->AddDebugComponent(m_pColliderCom);
#endif
}

HRESULT CMonster03::Render()
{
    //m_pColliderCom->Render();

    return S_OK;
}

// todo
_bool CMonster03::UpdateStateFarRangeAttackMonster(_float fTimeDelta)
{
    _bool bUpdate = __super::UpdateStateFarRangeAttackMonster(fTimeDelta);

    if (MONSTER_STATE::MS_ACTIVE & m_iState)
    {
        // ACTIVE알람이 울려서 이제 종료를 할 차례이다.
        if (false == strcmp(m_strCurrentAlertPointName.c_str(), "ACTIVE"))
        {
            m_strCurrentAlertPointName = "NONE";
            EndActiveAnim();
            SetForceState(CMonsterBase::MONSTER_STATE::MS_IDLE);
            return true;
        }
    }

    
    // 상태를 업데이트 하는 경우가 아닌 경우
    if (false == bUpdate)
    {
        if (MONSTER_STATE::MS_CHASE & m_iState)
        {
            m_pTransformCom->GoFoward(fTimeDelta, m_pNavigationComs[GetNavMeshIdx()]);
            RotateMonsterToPlayer(fTimeDelta);
        }
        else if (MONSTER_STATE::MS_ROLL_LEFT & m_iState)
        {
            if ((false == strcmp("ROLLLEFT", m_strCurrentAlertPointName.c_str())))
            {
                m_fRollAccTime += fTimeDelta;

                if (AlmostEqual(m_fRollTime, m_fRollAccTime))
                {
                    m_fRollAccTime = 0.f;
                    m_strCurrentAlertPointName = "NONE";
                }
                else
                    m_pTransformCom->GoLeft(fTimeDelta * m_fRollSpeed, m_pNavigationComs[GetNavMeshIdx()]);
            }

        }
        else if (MONSTER_STATE::MS_ROLL_RIGHT & m_iState)
        {
            if ((false == strcmp("ROLLRIGHT", m_strCurrentAlertPointName.c_str())))
            {
                m_fRollAccTime += fTimeDelta;

                if (AlmostEqual(m_fRollTime, m_fRollAccTime))
                {
                    m_fRollAccTime = 0.f;
                    m_strCurrentAlertPointName = "NONE";
                }
                else
                    m_pTransformCom->GoRight(fTimeDelta * m_fRollSpeed, m_pNavigationComs[GetNavMeshIdx()]);
            }
        }
        else if (MONSTER_STATE::MS_MOVE_FRONT & m_iState)
        {
            m_pTransformCom->GoFoward(fTimeDelta * 0.3f, m_pNavigationComs[GetNavMeshIdx()]);
            RotateMonsterToPlayer(fTimeDelta);
        }
        else if (MONSTER_STATE::MS_MOVE_RIGHT & m_iState)
        {
            m_pTransformCom->GoRight(fTimeDelta * 0.3f, m_pNavigationComs[GetNavMeshIdx()]);
            m_pTransformCom->GoFoward(fTimeDelta * 0.3f, m_pNavigationComs[GetNavMeshIdx()]);
            RotateMonsterToPlayer(fTimeDelta);
        }
        else if (MONSTER_STATE::MS_MOVE_LEFT & m_iState)
        {
            m_pTransformCom->GoLeft(fTimeDelta * 0.3f, m_pNavigationComs[GetNavMeshIdx()]);
            m_pTransformCom->GoFoward(fTimeDelta * 0.3f, m_pNavigationComs[GetNavMeshIdx()]);
            RotateMonsterToPlayer(fTimeDelta);
        }
        else if (MONSTER_STATE::MS_MOVE_BACK & m_iState)
        {
            m_pTransformCom->GoBackward(fTimeDelta * 0.5f, m_pNavigationComs[GetNavMeshIdx()]);
            RotateMonsterToPlayer(fTimeDelta);
        }
    }

    _vector vPos = m_pNavigationComs[GetNavMeshIdx()]->SetUpHeightOnNavigation(m_pTransformCom->GetTransformState(CTransform::STATE_POS));
    m_pTransformCom->SetTransformState(CTransform::STATE_POS, vPos);

    return bUpdate;
}


HRESULT CMonster03::ReadyComponents()
{
    // Com_Collider
    {
        CBoundingAABB::BOUND_AABB_DESC		ColliderDesc{};
        ColliderDesc.vExtents = _float3(0.7f, 1.0f, 0.7f);
        ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
            return E_FAIL;

        m_pColliderCom->SetComOwner(this);

    }

    if (FAILED(ReadyNavMesh()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster03::ReadyPartObjects()
{
    // Body
    {
        CStateObjectBody::STATE_OBJECT_BODY_DESC Desc = {};
        Desc.pOwner = this;
        Desc.pParentMatrix = m_pTransformCom->GetWorldMatrixPtr();
        Desc.pState = &m_iState;
        Desc.pPrevState = &m_iPrevState;
        Desc.iNumModels = 1;
        Desc.iLevelIndex = m_iLevelIndex;

        m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_GLOBAL, TEXT("Prototype_GameObject_MonsterBody03"), &Desc));
        if (nullptr == m_PartObjects[PART_BODY])
            return E_FAIL;
    }


    // Weapon
    {
        CStateObjectWeapon::STATE_OBJECT_WEAPON_DESC Desc = {};
        Desc.pOwner = this;
        Desc.pState = &m_iState;
        Desc.pParentMatrix = m_pTransformCom->GetWorldMatrixPtr();
        Desc.pSocketMatrix = static_cast<CMonsterBody03*>(m_PartObjects[PART_BODY])->GetBoneCombiedMatrix("Bip001 Prop1");
        Desc.iLevelIndex = m_iLevelIndex;

        m_PartObjects[PART_WEAPON] = static_cast<CPartObject*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_GLOBAL, TEXT("Prototype_GameObject_MonsterWeapon03"), &Desc));
        if (nullptr == m_PartObjects[PART_WEAPON])
            return E_FAIL;
    }

    return S_OK;
}

void CMonster03::CheckTrailAlertPoint()
{
    if (false == strcmp("ATTACK01_START", m_strCurrentAlertPointName.c_str()))
    {
        CMonsterWeaponBase* pWeapon = static_cast<CMonsterWeaponBase*>(m_PartObjects[PART_WEAPON]);
        if (pWeapon)
        {
            if (FAILED(pWeapon->FireProjectile(CMonsterWeaponBase::MPT_01)))
            {
                MSG_BOX("CMonster03::CheckTrailAlertPoint Fire Projectile Error!");
            }
        }

        m_strCurrentAlertPointName = "None";
    }
}

CMonster03* CMonster03::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMonster03* pInstance = new CMonster03(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CMonster03");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CMonster03::Clone(void* pArg)
{
    CMonster03* pInstance = new CMonster03(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CMonster03");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CMonster03::Free()
{
    __super::Free();
}
