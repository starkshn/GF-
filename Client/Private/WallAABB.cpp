#include "pch.h"
#include "WallAABB.h"
#include "BoundingAABB.h"
#include "GameInstance.h"
#include "Particle.h"
#include "Layer.h"
#include "ParticleManager.h"
#include "EffectManager.h"
#include "BossRock.h"

CWallAABB::CWallAABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CGameObject(pDevice, pContext)
{
}

CWallAABB::CWallAABB(const CWallAABB& Prototype)
    :
    CGameObject(Prototype)
{

}

HRESULT CWallAABB::InitializePrototype()
{
    return S_OK;
}

HRESULT CWallAABB::Initialize(void* pArg)
{
    if (__super::Initialize(pArg))
        return E_FAIL;

    if (ReadyComponents(pArg))
        return E_FAIL;

    return S_OK;
}

void CWallAABB::PriorityUpdate(_float fTimeDelta)
{

}

static _float fAcc = 0.f;
static _bool bOnce = 1;

void CWallAABB::Update(_float fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->GetWorldMatrix());

    if (!bOnce)
    {
        fAcc += fTimeDelta;
        if (fAcc > 0.3f)
        {
            bOnce = 1; 
            fAcc = 0.f;
        }
    }
}

void CWallAABB::LateUpdate(_float fTimeDelta)
{
    if (m_pGameInst->CheckIntersectInLayer(m_iLevelIndex, TEXT("Layer_BossRock"), m_pColliderCom, m_HitRet))
    {
        auto pObj = m_pGameInst->GetGameObjctInLayerBack(LEVEL_BOSS, LEVEL_BOSS, TEXT("Layer_BossRock"));
        if (nullptr != pObj)
        {
            CBossRock* pBossRock = static_cast<CBossRock*>(pObj);
            if (pBossRock->IsFire())
            {
                CParticleManager::GetInstance()->ActiveParticle(CParticleManager::BOSS_ROCK_COL, m_HitRet);
                CParticleManager::GetInstance()->ActiveParticle(CParticleManager::BOSS_ROCK_COL_DUST, m_HitRet);
                CParticleManager::GetInstance()->ActiveParticle(CParticleManager::BOSS_ROCK_COL_DUST_LIGHTING, m_HitRet);
                CEffectManager::GetInstance()->DeActiveEffect(CEffectManager::BOSS_ROCK_ALERT);

                m_pGameInst->PlayEffect(TEXT("BossRockCrash.mp3"), 1.f);
            }
        }
    }
}

HRESULT CWallAABB::Render()
{
    // m_pColliderCom->Render();

    return S_OK;
}

void CWallAABB::CalNotAccuarteHitPosAndNormal(_fvector vPlayerPos, _fvector vNotAccuarteFromPos, _fvector vFromDir, _float3& vOutPos, _float3& vOutNormal)
{
    _vector vDir = vNotAccuarteFromPos - vPlayerPos;
    vDir = XMVector3Normalize(vDir);

    CBoundingAABB* pAABB = static_cast<CBoundingAABB*>(m_pColliderCom->GetBoundingRaw());
    if (nullptr == pAABB)
        return;

    _float fDist = 0.f;
    _vector vHitPos = {};
    if (pAABB->GetBounding()->Intersects(vPlayerPos, vDir, fDist))
    {
        vHitPos = vPlayerPos + vDir * fDist;
        XMStoreFloat3(&vOutPos, vHitPos);
    }

    _float3 vCenter = const_cast<CBoundingAABB*>(pAABB)->GetBounding()->Center;
    _float3 vExtents = const_cast<CBoundingAABB*>(pAABB)->GetBounding()->Extents;
    _float3 vMin = const_cast<CBoundingAABB*>(pAABB)->GetMin();
    _float3 vMax = const_cast<CBoundingAABB*>(pAABB)->GetMax();

    // 충돌 지점과 AABB 중심의 상대 위치 계산
    _float3 vTempHitPos;
    XMStoreFloat3(&vTempHitPos, vHitPos);
    _float3 vRelativePos = { vTempHitPos.x - vCenter.x, vTempHitPos.y - vCenter.y, vTempHitPos.z - vCenter.z };

    // 노말 벡터 초기화
    _float3 vNormal = { 0.0f, 0.0f, 0.0f };

    // 충돌 방향을 기준으로 가장 가까운 축 계산
    if (abs(vRelativePos.x) / vExtents.x > abs(vRelativePos.y) / vExtents.y && abs(vRelativePos.x) / vExtents.x > abs(vRelativePos.z) / vExtents.z) 
    {
        // X축 방향
        vNormal = (vRelativePos.x > 0) ? _float3(1, 0, 0) : _float3(-1, 0, 0);
    }
    else if (abs(vRelativePos.y) / vExtents.y > abs(vRelativePos.z) / vExtents.z) 
    {
        // Y축 방향
        vNormal = (vRelativePos.y > 0) ? _float3(0, 1, 0) : _float3(0, -1, 0);
    }
    else 
    {
        // Z축 방향
        vNormal = (vRelativePos.z > 0) ? _float3(0, 0, 1) : _float3(0, 0, -1);
    }

    vOutNormal = vNormal;
}

void CWallAABB::CalHitPosAndNormal(_fvector vPlayerPos, _fvector vPlayertoBulletDir)
{
    CBoundingAABB* pAABB = static_cast<CBoundingAABB*>(m_pColliderCom->GetBoundingRaw());
    if (nullptr == pAABB)
        return;

    _float fDist = 0.f;
    _vector vHitPos = {};
    if (pAABB->GetBounding()->Intersects(vPlayerPos, vPlayertoBulletDir, fDist))
        vHitPos = vPlayerPos + vPlayertoBulletDir * fDist;

    _float3 vCenter = const_cast<CBoundingAABB*>(pAABB)->GetBounding()->Center;
    _float3 vExtents = const_cast<CBoundingAABB*>(pAABB)->GetBounding()->Extents;
    _float3 vMin = const_cast<CBoundingAABB*>(pAABB)->GetMin();
    _float3 vMax = const_cast<CBoundingAABB*>(pAABB)->GetMax();

    // 충돌 지점과 AABB 중심의 상대 위치 계산
    _float3 vTempHitPos;
    XMStoreFloat3(&vTempHitPos, vHitPos);
    _float3 vRelativePos = { vTempHitPos.x - vCenter.x, vTempHitPos.y - vCenter.y, vTempHitPos.z - vCenter.z };

    // 노말 벡터 초기화
    _float3 vNormal = { 0.0f, 0.0f, 0.0f };

    // 충돌 방향을 기준으로 가장 가까운 축 계산
    if (abs(vRelativePos.x) / vExtents.x > abs(vRelativePos.y) / vExtents.y && abs(vRelativePos.x) / vExtents.x > abs(vRelativePos.z) / vExtents.z)
    {
        // X축 방향
        vNormal = (vRelativePos.x > 0) ? _float3(1, 0, 0) : _float3(-1, 0, 0);
    }
    else if (abs(vRelativePos.y) / vExtents.y > abs(vRelativePos.z) / vExtents.z)
    {
        // Y축 방향
        vNormal = (vRelativePos.y > 0) ? _float3(0, 1, 0) : _float3(0, -1, 0);
    }
    else
    {
        // Z축 방향
        vNormal = (vRelativePos.z > 0) ? _float3(0, 0, 1) : _float3(0, 0, -1);
    }

    XMStoreFloat3(&m_HitRet.vHitPos, vHitPos);
    m_HitRet.vNormal = vNormal;

    // ActiveWallHitEP(m_HitRet);
}

void CWallAABB::CalHitPosAndNormalWithType(_fvector vPlayerPos, _fvector vPlayertoBulletDir, PROJECTILE_TYPE eType)
{
    CBoundingAABB* pAABB = static_cast<CBoundingAABB*>(m_pColliderCom->GetBoundingRaw());
    if (nullptr == pAABB)
        return;

    _float fDist = 0.f;
    _vector vHitPos = {};
    if (pAABB->GetBounding()->Intersects(vPlayerPos, vPlayertoBulletDir, fDist))
    {
        vHitPos = vPlayerPos + vPlayertoBulletDir * fDist;
        
        switch (eType)
        {
        case Client::PROJECTILE_TYPE::PJ_DEFAULT:
            break;
        case Client::PROJECTILE_TYPE::PJ_1004:
            break;
        case Client::PROJECTILE_TYPE::PJ_1503_DEFAULT:
            vHitPos = vPlayerPos + vPlayertoBulletDir * fDist * 0.9f;
            break;
        case Client::PROJECTILE_TYPE::PJ_1508_DEFAULT:
            vHitPos = vPlayerPos + vPlayertoBulletDir * fDist * 0.90f;
            break;
        case Client::PROJECTILE_TYPE::PJ_END:
            break;
        default:
            break;
        }

       
    }

    _float3 vCenter = const_cast<CBoundingAABB*>(pAABB)->GetBounding()->Center;
    _float3 vExtents = const_cast<CBoundingAABB*>(pAABB)->GetBounding()->Extents;
    _float3 vMin = const_cast<CBoundingAABB*>(pAABB)->GetMin();
    _float3 vMax = const_cast<CBoundingAABB*>(pAABB)->GetMax();

    // 충돌 지점과 AABB 중심의 상대 위치 계산
    _float3 vTempHitPos;
    XMStoreFloat3(&vTempHitPos, vHitPos);
    _float3 vRelativePos = { vTempHitPos.x - vCenter.x, vTempHitPos.y - vCenter.y, vTempHitPos.z - vCenter.z };

    // 노말 벡터 초기화
    _float3 vNormal = { 0.0f, 0.0f, 0.0f };

    // 충돌 방향을 기준으로 가장 가까운 축 계산
    if (abs(vRelativePos.x) / vExtents.x > abs(vRelativePos.y) / vExtents.y && abs(vRelativePos.x) / vExtents.x > abs(vRelativePos.z) / vExtents.z)
    {
        // X축 방향
        vNormal = (vRelativePos.x > 0) ? _float3(1, 0, 0) : _float3(-1, 0, 0);
    }
    else if (abs(vRelativePos.y) / vExtents.y > abs(vRelativePos.z) / vExtents.z)
    {
        // Y축 방향
        vNormal = (vRelativePos.y > 0) ? _float3(0, 1, 0) : _float3(0, -1, 0);
    }
    else
    {
        // Z축 방향
        vNormal = (vRelativePos.z > 0) ? _float3(0, 0, 1) : _float3(0, 0, -1);
    }

    XMStoreFloat3(&m_HitRet.vHitPos, vHitPos);
    m_HitRet.vNormal = vNormal;

    switch (eType)
    {
    case Client::PROJECTILE_TYPE::PJ_DEFAULT:
    {
        ActiveWallHitEP(eType, m_HitRet);
        if (bOnce)
        {
            bOnce = 0;
            m_pGameInst->PlayEffect(TEXT("WallHit.ogg"), 1.f);
        }
    }
    break;
    case Client::PROJECTILE_TYPE::PJ_1004:
    {
        XMStoreFloat3(&m_HitRet.vHitPos, vHitPos);
        ActiveWallHitEP(eType, m_HitRet);
    }
    break;
    case Client::PROJECTILE_TYPE::PJ_1503_DEFAULT:
    {
        XMStoreFloat3(&m_HitRet.vHitPos, vHitPos);
        ActiveWallHitEP(eType, m_HitRet);
        if (bOnce)
        {
            bOnce = 0;
            m_pGameInst->PlayEffect(TEXT("WallHit.ogg"), 1.f);
        }
    }
        break;
    case Client::PROJECTILE_TYPE::PJ_1508_DEFAULT:
    {
        XMStoreFloat3(&m_HitRet.vHitPos, vHitPos);
        ActiveWallHitEP(eType, m_HitRet);
        if (bOnce)
        {
            bOnce = 0;
            m_pGameInst->PlayEffect(TEXT("WallHit.ogg"), 1.f);
        }
    }
        break;
    case Client::PROJECTILE_TYPE::PJ_END:
        break;
    default:
        break;
    }
}

void CWallAABB::ActiveWallHitEP(PROJECTILE_TYPE eType, HIT_RESULT HitRet)
{
    switch (eType)
    {
    case Client::PROJECTILE_TYPE::PJ_DEFAULT:
    {
        CParticleManager::GetInstance()->ActiveParticle(CParticleManager::PARTICLE_TYPE::WALL_HIT, HitRet);
        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::PROJECTILE_WALL_HIT, HitRet);
        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::PROJECTILE_WALL_HIT_FRONT, HitRet);
        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::PROJECTILE_WALL_HIT_SPOT, HitRet);
    }
        break;
    case Client::PROJECTILE_TYPE::PJ_1004:
    {
        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::SPHERE_1004, HitRet);
        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::HIT_1004, HitRet);
    }
        break;
    case Client::PROJECTILE_TYPE::PJ_1503_DEFAULT:
    {
        CParticleManager::GetInstance()->ActiveParticle(CParticleManager::PARTICLE_TYPE::WALL_HIT, HitRet);
        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::BULLET_HIT03, HitRet);
        // CEffectManager::GetInstance()->ActiveEffect(CEffectManager::PROJECTILE_WALL_HIT, HitRet);
        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::PROJECTILE_WALL_HIT_SPOT, HitRet);
    }
        break;
    case Client::PROJECTILE_TYPE::PJ_1508_DEFAULT:
    {
        CParticleManager::GetInstance()->ActiveParticle(CParticleManager::PARTICLE_TYPE::WALL_HIT, HitRet);
        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::BULLET_HIT04, HitRet);
        // CEffectManager::GetInstance()->ActiveEffect(CEffectManager::PROJECTILE_WALL_HIT, HitRet);
        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::PROJECTILE_WALL_HIT_SPOT, HitRet);
    }
        break;
    case Client::PROJECTILE_TYPE::PJ_END:
        break;
    default:
        break;
    }
}

HRESULT CWallAABB::ReadyComponents(void* pArg)
{
    // Com_Collider
    {
        if (nullptr != pArg)
        {
            WALL_AABB_DESC* pDesc = static_cast<WALL_AABB_DESC*>(pArg);
            CBoundingAABB::BOUND_AABB_DESC		ColliderDesc{};
            ColliderDesc.vCenter = pDesc->vCenter;
            ColliderDesc.vExtents = pDesc->vExtents;
            if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
                return E_FAIL;
            m_pColliderCom->SetComOwner(this);
        }
    }

    return S_OK;
}

CWallAABB* CWallAABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWallAABB* pInstance = new CWallAABB(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CWallAABB");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CWallAABB::Clone(void* pArg)
{
    CWallAABB* pInstance = new CWallAABB(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CWallAABB");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CWallAABB::Free()
{
    __super::Free();

    SafeRelease(m_pColliderCom);
}
