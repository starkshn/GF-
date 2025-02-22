#include "pch.h"
#include "Monster05.h"
#include "GameInstance.h"
#include "Collider.h"
#include "Navigation.h"
#include "MonsterWeapon01.h"
#include "ParticleManager.h"

CMonster05::CMonster05(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CMonsterBase(pDevice, pContext)
{

}

CMonster05::CMonster05(const CMonster05& Prototype)
    :
    CMonsterBase(Prototype)
{

}

HRESULT CMonster05::InitializePrototype()
{
	return S_OK;
}

HRESULT CMonster05::Initialize(void* pArg)
{
    //MONSTER_DESC Desc = {};
    //Desc.iNumPartObjects = PART_END;
    //Desc.fSpeedPerSec = 1.5f;
    //Desc.fRotSpeedPerSec = XMConvertToRadians(180.f);
    //Desc.strObjectTag = "MONSTER_05";
    //Desc.bUseSpawnPos = true;
    //
    //_float vAttackRanges[3] = { 5.f, 6.5f, 8.f };
    //Desc.pAttackRanges = vAttackRanges;
    //Desc.iNumAttackRanges = 3;

    //// TODO -> 여기서 파일에서 읽어와서 값을 써야한다.
    //Desc.vSpawnPos = _float3{0.f, 0.f, 16.f};

    //_float vAttackCoolTimes[3] = { 0.f, 0.f, 5.f };
    //Desc.pAttackCoolTimes = vAttackCoolTimes;

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

void CMonster05::PriorityUpdate(_float fTimeDelta)
{
    __super::PriorityUpdate(fTimeDelta);
}

void CMonster05::Update(_float fTimeDelta)
{
    m_bUpdateState = UpdateStateCloseRangeAttackMonster(fTimeDelta);

    __super::Update(fTimeDelta);

    m_pColliderCom->Update(m_pTransformCom->GetWorldMatrix());

    // PlayWalkSound(fTimeDelta);
}

void CMonster05::LateUpdate(_float fTimeDelta)
{
    __super::LateUpdate(fTimeDelta);
    m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);

    // CheckColPlayerBullet();


#ifdef _DEBUG
    //m_pGameInst->AddDebugComponent(m_pColliderCom);
#endif
}

HRESULT CMonster05::Render()
{
    //m_pColliderCom->Render();

    return S_OK;
}

_bool CMonster05::UpdateStateCloseRangeAttackMonster(_float fTimeDelta)
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
        else if (MONSTER_STATE::MS_ATTACK & m_iState)
        {
            _int idx = GetAttackAnimIndex();
            if (ATTACK_SWORD_SWING == GetAttackAnimIndex() && (false == strcmp("ATTACK01", m_strCurrentAlertPointName.c_str())))
            {
                SetCurrentAttackAnimIndexInCoolTime();  
            }
        }
    }

    _vector vPos = m_pNavigationComs[GetNavMeshIdx()]->SetUpHeightOnNavigation(m_pTransformCom->GetTransformState(CTransform::STATE_POS));
    m_pTransformCom->SetTransformState(CTransform::STATE_POS, vPos);

    return bUpdate;
}

void CMonster05::CheckTrailAlertPoint()
{
    if (false == strcmp("ATTACK01START", m_strCurrentAlertPointName.c_str()))
    {
        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::SWORD_TRAIL, HIT_RESULT{}, m_PartObjects[PART_WEAPON]);
        m_strCurrentAlertPointName = "None";
    }

    if (false == strcmp("ATTACK02START", m_strCurrentAlertPointName.c_str()))
    {
        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::SWORD_TRAIL, HIT_RESULT{}, m_PartObjects[PART_WEAPON]);
        m_strCurrentAlertPointName = "None";
    }
}

HRESULT CMonster05::ReadyComponents()
{
    // Com_Collider
    {
        CBoundingAABB::BOUND_AABB_DESC		ColliderDesc{};
        ColliderDesc.vExtents = _float3(0.8f, 1.0f, 0.8f);
        ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
            return E_FAIL;

        m_pColliderCom->SetComOwner(this);

    }

    if (FAILED(ReadyNavMesh()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster05::ReadyPartObjects()
{
    // Body
    {
        CStateObjectBody::STATE_OBJECT_BODY_DESC Desc = {};
        Desc.pParentMatrix = m_pTransformCom->GetWorldMatrixPtr();
        Desc.pState = &m_iState;
        Desc.pPrevState = &m_iPrevState;
        Desc.pOwner = this;
        Desc.iNumModels = 1;

        m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_GLOBAL, TEXT("Prototype_GameObject_MonsterBody05"), &Desc));
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

        m_PartObjects[PART_WEAPON] = static_cast<CPartObject*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_GLOBAL, TEXT("Prototype_GameObject_MonsterWeapon05"), &Desc));
        if (nullptr == m_PartObjects[PART_WEAPON])
            return E_FAIL;
    }

    return S_OK;
}


CMonster05* CMonster05::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMonster05* pInstance = new CMonster05(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CMonster05");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CMonster05::Clone(void* pArg)
{
    CMonster05* pInstance = new CMonster05(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CMonster05");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CMonster05::Free()
{
    __super::Free();
}
