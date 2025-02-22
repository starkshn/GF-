#include "pch.h"
#include "Monster01.h"
#include "GameInstance.h"
#include "Collider.h"
#include "Navigation.h"
#include "MonsterWeapon01.h"
#include "ParticleManager.h"
#include "EffectManager.h"

CMonster01::CMonster01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CMonsterBase(pDevice, pContext)
{

}

CMonster01::CMonster01(const CMonster01& Prototype)
    :
    CMonsterBase(Prototype)
{

}

HRESULT CMonster01::InitializePrototype()
{
	return S_OK;
}

HRESULT CMonster01::Initialize(void* pArg)
{
    //MONSTER_DESC Desc = {};
    //Desc.iNumPartObjects = PART_END;
    //Desc.fSpeedPerSec = 3.f;
    //Desc.fRotSpeedPerSec = XMConvertToRadians(180.f);
    //Desc.strObjectTag = "Monster01";
    //Desc.bUseSpawnPos = true;
    //
    //Desc.iNumAttackRanges = 2;

    //_float vAttackRanges[2] = { 2.f, 3.f };
    //Desc.pAttackRanges = vAttackRanges;

    //_float vAttackCoolTimes[2] = { 0.f, 0.f };
    //Desc.pAttackCoolTimes = vAttackCoolTimes;

    //// TODO -> 여기서 파일에서 읽어와서 값을 써야한다.
    //Desc.vSpawnPos = _float3{0.f, 0.f, 1.f};

    m_bCanRoll = true;
    m_fRollRange = 2.2f;

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

void CMonster01::PriorityUpdate(_float fTimeDelta)
{
    __super::PriorityUpdate(fTimeDelta);
}

void CMonster01::Update(_float fTimeDelta)
{
    m_bUpdateState = UpdateStateCloseRangeAttackMonster(fTimeDelta);

    __super::Update(fTimeDelta);

    m_pColliderCom->Update(m_pTransformCom->GetWorldMatrix());

    PlayWalkSound(fTimeDelta);
}

void CMonster01::LateUpdate(_float fTimeDelta)
{
    __super::LateUpdate(fTimeDelta);

    m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);

    // CheckColPlayerBullet();

#ifdef _DEBUG
    // m_pGameInst->AddDebugComponent(m_pColliderCom);
#endif
}

HRESULT CMonster01::Render()
{
    // m_pColliderCom->Render();
    return S_OK;
}

_bool CMonster01::UpdateStateCloseRangeAttackMonster(_float fTimeDelta)
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

void CMonster01::CheckTrailAlertPoint()
{
    if (false == strcmp(m_strCurrentAlertPointName.c_str(), "Attack01Start"))
    {
        HIT_RESULT Hit = {};
        _vector vWorldPos = GetSocketWorldPos();
        Hit.vHitPos;
        XMStoreFloat3(&Hit.vNormal, vWorldPos);
        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::SWORD_TRAIL, Hit, m_PartObjects[PART_WEAPON]);

        m_strCurrentAlertPointName = "None";
    }
    else if (false == strcmp(m_strCurrentAlertPointName.c_str(), "Attack02Start"))
    {
        HIT_RESULT Hit = {};
        _vector vWorldPos = GetSocketWorldPos();
        Hit.vHitPos;
        XMStoreFloat3(&Hit.vNormal, vWorldPos);
        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::PIERCING_TRAIL, Hit, m_PartObjects[PART_WEAPON]);

        m_strCurrentAlertPointName = "None";
    }
    else if (false == strcmp(m_strCurrentAlertPointName.c_str(), "ROLLLEFT_START"))
    {
        HIT_RESULT Hit = {};
        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::CHASE_TRAIL, Hit, this);
        m_strCurrentAlertPointName = "None";
    }
    else if (false == strcmp(m_strCurrentAlertPointName.c_str(), "ROLLRIGHT_START"))
    {
        HIT_RESULT Hit = {};
        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::CHASE_TRAIL, Hit, this);
        m_strCurrentAlertPointName = "None";
    }
}

void CMonster01::EndTrailEffect()
{
    // m_bStartTrailEffect = false;
}

HRESULT CMonster01::ReadyComponents()
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

HRESULT CMonster01::ReadyPartObjects()
{
    // Body
    {
        CStateObjectBody::STATE_OBJECT_BODY_DESC Desc = {};
        Desc.pParentMatrix = m_pTransformCom->GetWorldMatrixPtr();
        Desc.pState = &m_iState;
        Desc.pPrevState = &m_iPrevState;
        Desc.pOwner = this;
        Desc.iNumModels = 1;

        m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_GLOBAL, TEXT("Prototype_GameObject_MonsterBody01"), &Desc));
        if (nullptr == m_PartObjects[PART_BODY])
            return E_FAIL;
    }


    // Weapon
    {
        CStateObjectWeapon::STATE_OBJECT_WEAPON_DESC Desc = {};
        Desc.pOwner = this;
        Desc.pState = &m_iState;
        Desc.pParentMatrix = m_pTransformCom->GetWorldMatrixPtr();
        Desc.pSocketMatrix = static_cast<CMonsterBody01*>(m_PartObjects[PART_BODY])->GetBoneCombiedMatrix("Bip001 Prop1");

        m_PartObjects[PART_WEAPON] = static_cast<CPartObject*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_GLOBAL, TEXT("Prototype_GameObject_MonsterWeapon01"), &Desc));
        if (nullptr == m_PartObjects[PART_WEAPON])
            return E_FAIL;
    }

    return S_OK;
}

CMonster01* CMonster01::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMonster01* pInstance = new CMonster01(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CMonster01");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CMonster01::Clone(void* pArg)
{
    CMonster01* pInstance = new CMonster01(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CMonster01");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CMonster01::Free()
{
    __super::Free();
}
