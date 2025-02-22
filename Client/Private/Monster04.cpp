#include "pch.h"
#include "Monster04.h"
#include "GameInstance.h"
#include "Collider.h"
#include "Navigation.h"
#include "MonsterWeapon01.h"
#include "ParticleManager.h"

CMonster04::CMonster04(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CMonsterBase(pDevice, pContext)
{

}

CMonster04::CMonster04(const CMonster04& Prototype)
    :
    CMonsterBase(Prototype)
{

}

HRESULT CMonster04::InitializePrototype()
{
    return S_OK;
}

HRESULT CMonster04::Initialize(void* pArg)
{
    //MONSTER_DESC Desc = {};
    //Desc.iNumPartObjects = PART_END;
    //Desc.fSpeedPerSec = 4.5f;
    //Desc.fRotSpeedPerSec = XMConvertToRadians(180.f);
    //Desc.strObjectTag = "MONSTER_04";
    //Desc.bUseSpawnPos = true;

    //_float vAttackRanges[1] = { 8.f };
    //Desc.pAttackRanges = vAttackRanges;
    //Desc.iNumAttackRanges = 1;

    //// TODO -> 여기서 파일에서 읽어와서 값을 써야한다.

    //_float vAttackCoolTimes[1] = { 5.f };
    //Desc.pAttackCoolTimes = vAttackCoolTimes;

    //Desc.vSpawnPos = _float3{ 0.f, 0.f, 13.f };

    m_bCanRoll = true;
    m_fRollRange = 3.f;
    m_fRollSpeed = 5.5;



    m_fAimingRange = 9.f;
    m_fMinDistanceFromTarget = 6.f;

    SetForceState(MONSTER_STATE::MS_DEACTIVE);
    m_iPrevState = MONSTER_STATE::MS_DEACTIVE;

    m_bCanRollBack = true;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    if (FAILED(ReadyPartObjects()))
        return E_FAIL;


    return S_OK;
}

void CMonster04::PriorityUpdate(_float fTimeDelta)
{
    __super::PriorityUpdate(fTimeDelta);
}

void CMonster04::Update(_float fTimeDelta)
{
    m_bUpdateState = UpdateStateFarRangeAttackMonster(fTimeDelta);

    __super::Update(fTimeDelta);
    m_pColliderCom->Update(m_pTransformCom->GetWorldMatrix());
    PlayWalkSound(fTimeDelta);
}

void CMonster04::LateUpdate(_float fTimeDelta)
{
    __super::LateUpdate(fTimeDelta);
    m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);

    // CheckColPlayerBullet();


#ifdef _DEBUG
    //m_pGameInst->AddDebugComponent(m_pColliderCom);
#endif
}

HRESULT CMonster04::Render()
{
    //m_pColliderCom->Render();

    return S_OK;
}

// todo
_bool CMonster04::UpdateStateFarRangeAttackMonster(_float fTimeDelta)
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
        else if (MONSTER_STATE::MS_ROLL_BACK & m_iState)
        {
            if ((false == strcmp("ROLLBACK", m_strCurrentAlertPointName.c_str())))
            {
                m_fRollAccTime += fTimeDelta;

                if (AlmostEqual(m_fRollTime, m_fRollAccTime))
                {
                    m_fRollAccTime = 0.f;
                    m_strCurrentAlertPointName = "NONE";
                }
                else
                    m_pTransformCom->GoBackward(fTimeDelta * m_fRollSpeed, m_pNavigationComs[GetNavMeshIdx()]);
            }
        }
        else if (MONSTER_STATE::MS_ACTIVE & m_iState)
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
        else if (MONSTER_STATE::MS_MOVE_FRONT & m_iState)
        {
            m_pTransformCom->GoFoward(fTimeDelta, m_pNavigationComs[GetNavMeshIdx()]);
            RotateMonsterToPlayer(fTimeDelta);
        }
        else if (MONSTER_STATE::MS_MOVE_RIGHT & m_iState)
        {
            m_pTransformCom->GoRight(fTimeDelta, m_pNavigationComs[GetNavMeshIdx()]);
            // m_pTransformCom->GoFoward(fTimeDelta * 0.3);
            RotateMonsterToPlayer(fTimeDelta);
        }
        else if (MONSTER_STATE::MS_MOVE_LEFT & m_iState)
        {
            m_pTransformCom->GoLeft(fTimeDelta, m_pNavigationComs[GetNavMeshIdx()]);
            // m_pTransformCom->GoFoward(fTimeDelta * 0.3);
            RotateMonsterToPlayer(fTimeDelta);
        }
        else if (MONSTER_STATE::MS_MOVE_BACK & m_iState)
        {
            m_pTransformCom->GoBackward(fTimeDelta, m_pNavigationComs[GetNavMeshIdx()]);
            RotateMonsterToPlayer(fTimeDelta);
        }
    }

    _vector vPos = m_pNavigationComs[GetNavMeshIdx()]->SetUpHeightOnNavigation(m_pTransformCom->GetTransformState(CTransform::STATE_POS));
    m_pTransformCom->SetTransformState(CTransform::STATE_POS, vPos);

    return bUpdate;
}

void CMonster04::CheckTrailAlertPoint()
{
    if (false == strcmp("ATTACK01_START", m_strCurrentAlertPointName.c_str()))
    {
        CMonsterWeaponBase* pWeapon = static_cast<CMonsterWeaponBase*>(m_PartObjects[PART_WEAPON]);

        if (pWeapon)
        {
            if (FAILED(pWeapon->FireProjectile(CMonsterWeaponBase::MPT_02)))
            {
                MSG_BOX("CMonster04::CheckTrailAlertPoint Fire Projectile Error!");
            }
        }

        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::SWORD_TRAIL, HIT_RESULT{}, m_PartObjects[PART_WEAPON]);
        m_strCurrentAlertPointName = "None";
    }
}


HRESULT CMonster04::ReadyComponents()
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

HRESULT CMonster04::ReadyPartObjects()
{
    // Body
    {
        CStateObjectBody::STATE_OBJECT_BODY_DESC Desc = {};
        Desc.pParentMatrix = m_pTransformCom->GetWorldMatrixPtr();
        Desc.pState = &m_iState;
        Desc.pPrevState = &m_iPrevState;
        Desc.pOwner = this;
        Desc.iNumModels = 1;
        Desc.iLevelIndex = m_iLevelIndex;


        m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_GLOBAL, TEXT("Prototype_GameObject_MonsterBody04"), &Desc));
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

        m_PartObjects[PART_WEAPON] = static_cast<CPartObject*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_GLOBAL, TEXT("Prototype_GameObject_MonsterWeapon04"), &Desc));
        if (nullptr == m_PartObjects[PART_WEAPON])
            return E_FAIL;
    }

    return S_OK;
}


CMonster04* CMonster04::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMonster04* pInstance = new CMonster04(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CMonster04");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CMonster04::Clone(void* pArg)
{
    CMonster04* pInstance = new CMonster04(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CMonster04");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CMonster04::Free()
{
    __super::Free();
}
