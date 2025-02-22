#include "pch.h"
#include "Monster06.h"
#include "GameInstance.h"
#include "Collider.h"
#include "MonsterWeapon02.h"
#include "Navigation.h"
#include "ParticleManager.h"

CMonster06::CMonster06(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CMonsterBase(pDevice, pContext)
{

}

CMonster06::CMonster06(const CMonster06& Prototype)
    :
    CMonsterBase(Prototype)
{

}

HRESULT CMonster06::InitializePrototype()
{
    return S_OK;
}

HRESULT CMonster06::Initialize(void* pArg)
{
    //MONSTER_DESC Desc = {};
    //Desc.iNumPartObjects = PART_END;
    //Desc.fSpeedPerSec = 5.f;
    //Desc.fRotSpeedPerSec = XMConvertToRadians(180.f);
    //Desc.strObjectTag = "MONSTER_06";
    //Desc.bUseSpawnPos = true;

    //_float vAttackRanges[1] = { 4.5f };
    //Desc.pAttackRanges = vAttackRanges;
    //Desc.iNumAttackRanges = 1;

    //// TODO -> 여기서 파일에서 읽어와서 값을 써야한다.
    //Desc.vSpawnPos = _float3{ 0.f, 0.f, 4.f };

    //_float vAttackCoolTimes[2] = { 0.f, 5.f };
    //Desc.pAttackCoolTimes = vAttackCoolTimes;

    m_bCanRoll = true;
    m_fRollRange = 3.f;

    SetForceState(MONSTER_STATE::MS_DEACTIVE);
    m_iPrevState = MONSTER_STATE::MS_DEACTIVE;

    // SetForceState(MONSTER_STATE::MS_IDLE);
    // m_iPrevState = MONSTER_STATE::MS_IDLE;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    if (FAILED(ReadyPartObjects()))
        return E_FAIL;

    return S_OK;
}

void CMonster06::PriorityUpdate(_float fTimeDelta)
{
    __super::PriorityUpdate(fTimeDelta);
}

void CMonster06::Update(_float fTimeDelta)
{
    m_bUpdateState = UpdateStateCloseRangeAttackMonster(fTimeDelta);
    
    m_pColliderCom->Update(m_pTransformCom->GetWorldMatrix());

    __super::Update(fTimeDelta);
}

void CMonster06::LateUpdate(_float fTimeDelta)
{
    m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);

    __super::LateUpdate(fTimeDelta);

    PlayWalkSound(fTimeDelta);

#ifdef _DEBUG
    //m_pGameInst->AddDebugComponent(m_pColliderCom);
#endif
}

HRESULT CMonster06::Render()
{
    //m_pColliderCom->Render();

    return S_OK;
}

_bool CMonster06::UpdateStateCloseRangeAttackMonster(_float fTimeDelta)
{
    _bool bUpdate = __super::UpdateStateCloseRangeAttackMonster(fTimeDelta);

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


    if (false == bUpdate)
    {
        if (MONSTER_STATE::MS_ATTACK & m_iState)
        {
            if (ATTACK_RUSH == GetAttackAnimIndex() && (false == strcmp("RUSH", m_strCurrentAlertPointName.c_str())))
            {
                m_fRushAcctime += fTimeDelta;

                if (AlmostEqual(m_fRushAcctime, m_fRushTime))
                {
                    m_fRushAcctime = 0.f;
                    m_strCurrentAlertPointName = "NONE";
                    SetCurrentAttackAnimIndexInCoolTime();
                }
                else
                    m_pTransformCom->GoFoward(fTimeDelta * m_fRushSpeed, m_pNavigationComs[GetNavMeshIdx()]);
            }
        }
        else if (MONSTER_STATE::MS_CHASE & m_iState)
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
                {
                    m_pTransformCom->GoLeft(fTimeDelta * m_fRollSpeed, m_pNavigationComs[GetNavMeshIdx()]);
                }
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
                {
                    m_pTransformCom->GoRight(fTimeDelta * m_fRollSpeed, m_pNavigationComs[GetNavMeshIdx()]);
                }
            }
        }
        else if (MONSTER_STATE::MS_ACTIVE & m_iState)
        {
            // ACTIVE알람이 울려서 이제 종료를 할 차례이다.
            if (false == strcmp(m_strCurrentAlertPointName.c_str(), "ACTIVE"))
            {
                m_strCurrentAlertPointName = "NONE";
                EndActiveAnim();
                return true;
            }
        }
    }

    _vector vPos = m_pNavigationComs[GetNavMeshIdx()]->SetUpHeightOnNavigation(m_pTransformCom->GetTransformState(CTransform::STATE_POS));
    m_pTransformCom->SetTransformState(CTransform::STATE_POS, vPos);

    return bUpdate;
}

HRESULT CMonster06::ReadyComponents()
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

HRESULT CMonster06::ReadyPartObjects()
{
    // Body
    {
        CStateObjectBody::STATE_OBJECT_BODY_DESC Desc = {};
        Desc.pParentMatrix = m_pTransformCom->GetWorldMatrixPtr();
        Desc.pState = &m_iState;
        Desc.pPrevState = &m_iPrevState;
        Desc.pOwner = this;
        Desc.iNumModels = 1;

        m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_GLOBAL, TEXT("Prototype_GameObject_MonsterBody06"), &Desc));
        if (nullptr == m_PartObjects[PART_BODY])
            return E_FAIL;
    }

    // Weapon
    {
        CStateObjectWeapon::STATE_OBJECT_WEAPON_DESC Desc = {};
        Desc.pOwner = this;
        Desc.pState = &m_iState;
        Desc.pParentMatrix = m_pTransformCom->GetWorldMatrixPtr();
        Desc.pSocketMatrix = static_cast<CMonsterBody02*>(m_PartObjects[PART_BODY])->GetBoneCombiedMatrix("Bip001 Prop1");

        m_PartObjects[PART_WEAPON] = static_cast<CPartObject*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_GLOBAL, TEXT("Prototype_GameObject_MonsterWeapon06"), &Desc));
        if (nullptr == m_PartObjects[PART_BODY])
            return E_FAIL;
    }

    return S_OK;
}

void CMonster06::CheckTrailAlertPoint()
{
    if (false == strcmp("ATTACK01START", m_strCurrentAlertPointName.c_str()))
    {
        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::PIERCING_TRAIL, HIT_RESULT{}, m_PartObjects[PART_WEAPON]);
        m_strCurrentAlertPointName = "None";
    }
}

CMonster06* CMonster06::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMonster06* pInstance = new CMonster06(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CMonster06");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CMonster06::Clone(void* pArg)
{
    CMonster06* pInstance = new CMonster06(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CMonster06");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CMonster06::Free()
{
    __super::Free();
}
