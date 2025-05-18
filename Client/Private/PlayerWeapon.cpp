#include "pch.h"
#include "PlayerWeapon.h"
#include "GameInstance.h"
#include "GlobalFuncs.h"

#include "Player.h"
#include "PlayerBody.h"
#include "Collider.h"



#include "EffectManager.h"

CPlayerWeapon::CPlayerWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CStateObjectWeapon(pDevice, pContext)
{

}

CPlayerWeapon::CPlayerWeapon(const CPlayerWeapon& Prototype)
    :
    CStateObjectWeapon(Prototype)
{

}

HRESULT CPlayerWeapon::InitializePrototype()
{
    return S_OK;
}

HRESULT CPlayerWeapon::Initialize(void* pArg)
{
    if (nullptr != pArg)
    {
        PLAYER_WEAPON_DESC* pDesc = static_cast<PLAYER_WEAPON_DESC*>(pArg);

        for (_uint i = 0; i < pDesc->iNumModels; ++i)
            m_pSocketMatrices.push_back((*pDesc->pSocketMatrices)[i]);

        m_pPlayer = dynamic_cast<CPlayer*>(pDesc->pOwner);
        if (nullptr == m_pPlayer)
            return E_FAIL;
    }

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    
    m_pParticlePlayerGunFire = dynamic_cast<CParticlePlayerGunFire*>(m_pGameInst->GetGameObjctInLayer(LEVEL_GLOBAL, LEVEL_GLOBAL, TEXT("Layer_Particles"), "PlayerGunFire"));
    if (nullptr == m_pParticlePlayerGunFire)
        return E_FAIL;

    return S_OK;
}

void CPlayerWeapon::PriorityUpdate(_float fTimeDelta)
{
}

void CPlayerWeapon::Update(_float fTimeDelta)
{
    _uint       iCurModelIdx = m_pPlayer->GetCurrentModelIndex();
    _matrix     SocketMatrix = XMMatrixIdentity();

    _float      fOffSetX = 0.f;
    _float      fOffSetY = 0.f;
    _float      fOffSetZ = 0.f;

    if ((_uint)PLAYER_MODEL_INDEX::RIGHT_1201 == iCurModelIdx)
    {
        SocketMatrix = XMLoadFloat4x4(m_pSocketMatrices[iCurModelIdx]);
        fOffSetX = 0.1f;
        fOffSetZ = -0.7f;
    }
    else if ((_uint)PLAYER_MODEL_INDEX::RIGHT_1004 == iCurModelIdx)
    {
        SocketMatrix = XMLoadFloat4x4(m_pSocketMatrices[iCurModelIdx]);
        fOffSetZ = -1.f;
    }
    else if ((_uint)PLAYER_MODEL_INDEX::RIGHT_1503 == iCurModelIdx)
    {
        SocketMatrix = XMLoadFloat4x4(m_pSocketMatrices[iCurModelIdx]);
        fOffSetZ = -1.f;
    }
    else if ((_uint)PLAYER_MODEL_INDEX::RIGHT_1508 == iCurModelIdx)
    {
        SocketMatrix = XMLoadFloat4x4(m_pSocketMatrices[iCurModelIdx]);
        fOffSetZ = -1.f;
    }

    for (_int i = 0; i < 3; ++i)
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

    m_pTransformCom->Scaling(0.2f, 0.2f, 0.2f);
    m_pTransformCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(fOffSetX, fOffSetY, fOffSetZ, 1.f));
    XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->GetWorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

    if (m_bActivate)
    {
        m_bActivate = false;
        m_pParticlePlayerGunFire->ResetParticles();
        m_pParticlePlayerGunFire->Activate();
    }

    if (m_pParticlePlayerGunFire->IsActivate())
        m_pParticlePlayerGunFire->UpdateParticleWorldMatrix(XMLoadFloat4x4(&m_WorldMatrix));

    m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));
}

void CPlayerWeapon::LateUpdate(_float fTimeDelta)
{
    // m_pGameInst->AddRenderObject(CRenderer::RG_NONLIGHT, this);

#ifdef _DEBUG
    // m_pGameInst->AddDebugComponent(m_pColliderCom);
#endif 
}

HRESULT CPlayerWeapon::Render()
{
#ifdef _DEBUG
    m_pColliderCom->Render();
#endif

    return S_OK;
}

void CPlayerWeapon::ActivateGunMuzzleEffect(_uint iPlayerModelIdx)
{
    // m_pParticlePlayerGunFire->SetGunFireCount(iGunFireCount);
    //m_pParticlePlayerGunFire->ResetParticles();
    //m_bActivate = true;

    HIT_RESULT Hit = {};
    _vector vWorldPos = GetSocketWorldMatrix().r[3];
    // _vector vWorldPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
    _vector vLook = {}, vUp = {};
    if (m_pOwner)
    {
        CTransform* pTC = static_cast<CTransform*>(m_pOwner->GetComponent(TEXT("Com_Transform")));
        vLook = XMVector3Normalize(pTC->GetTransformState(CTransform::STATE_LOOK));
        vUp = XMVector3Normalize(pTC->GetTransformState(CTransform::STATE_UP));
    }

    XMStoreFloat3(&Hit.vHitPos, vWorldPos);
    XMStoreFloat3(&Hit.vNormal, vLook);

    vWorldPos += vLook * 0.5f;

    if (1 == iPlayerModelIdx)
        vWorldPos += vUp * 0.2f;
    else if (2 == iPlayerModelIdx)
        vWorldPos += vUp * 0.2f;
    else if (3 == iPlayerModelIdx)
        vWorldPos += vUp * 0.2f;

    XMStoreFloat3(&Hit.vHitPos, vWorldPos);

    if (0 == iPlayerModelIdx)
        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::PLAYER01_MUZZLE, Hit);
    else if (1 == iPlayerModelIdx)
        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::PLAYER02_MUZZLE, Hit);
    else if (2 == iPlayerModelIdx)
        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::PLAYER03_MUZZLE, Hit);
    else if (3 == iPlayerModelIdx)
        CEffectManager::GetInstance()->ActiveEffect(CEffectManager::PLAYER04_MUZZLE, Hit);
}

void CPlayerWeapon::Fire()
{
    m_Projectiles.clear();
    m_BulletEffectInfos.clear();
    m_BulletTrailEffectInfos.clear();

    _uint       iCurModelIdx = m_pPlayer->GetCurrentModelIndex();
    CreateBullet(iCurModelIdx);
    ActiveEffectAndParticle(iCurModelIdx);
}

_vector CPlayerWeapon::CalWorldEndPos()
{
    // 뷰, 투영(원근) 행렬을 얻어온다.
    _matrix     viewMatrix = m_pGameInst->GetTransformMatrix(CPipeLine::DTS::D3DTS_VIEW);
    _matrix     projMatrix = m_pGameInst->GetTransformMatrix(CPipeLine::DTS::D3DTS_PROJ);

    // 화면 정 가운데의 스크린 좌표를 NDC로 변환 한다.
    _float fNdcX = G_CalNDCX(g_iWinSizeX / 2.f, g_iWinSizeX);
    _float fNdcY = G_CalNDCY(g_iWinSizeY / 2.f, g_iWinSizeY);

    // 뷰, 투영 역행렬 계산
    _matrix     invV = XMMatrixInverse(nullptr, viewMatrix);
    _matrix     invP = XMMatrixInverse(nullptr, projMatrix);
    _matrix     invVP = invP * invV;

    // NDC를 월드 공간으로 변환
    // NDC상에서 가장 먼 곳의 좌표를 잡아준다.
    _vector     vFarPoint = XMVectorSet(fNdcX, fNdcY, 1.f, 1.f);

    // 이 두 점을 빼면 Ray가 나온다.
    return XMVector3TransformCoord(vFarPoint, invVP);
}

_vector CPlayerWeapon::CalWorldEndPos(_float fSX, _float fSY)
{
    _matrix     viewMatrix = m_pGameInst->GetTransformMatrix(CPipeLine::DTS::D3DTS_VIEW);
    _matrix     projMatrix = m_pGameInst->GetTransformMatrix(CPipeLine::DTS::D3DTS_PROJ);

    _float      fNdcX = G_CalNDCX(fSX, g_iWinSizeX); // 스크린 -> NDC 변환
    _float      fNdcY = G_CalNDCY(fSY, g_iWinSizeY);

    _matrix     invV = XMMatrixInverse(nullptr, viewMatrix);
    _matrix     invP = XMMatrixInverse(nullptr, projMatrix);
    _matrix     invVP = invP * invV;

    _vector     vFarPoint = XMVectorSet(fNdcX, fNdcY, 1.f, 1.f);
    return      XMVector3TransformCoord(vFarPoint, invVP);
}

void CPlayerWeapon::ActiveEffectAndParticle(_uint iModelIdx)
{
    if ((_uint)PLAYER_MODEL_INDEX::RIGHT_1201 == iModelIdx)
    {
        _vector     vWorldFar = CalWorldEndPos();
        _vector     vSrcPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
        _vector     vBulletDir = XMVector3Normalize(vWorldFar - vSrcPos);

        for (_int i = 0; i < m_Projectiles.size(); ++i)
        {
            /*HIT_RESULT Hit = {};
            XMStoreFloat3(&Hit.vNormal, vBulletDir);

            _vector vecPos = XMVectorSet(m_WorldMatrix.m[3][0], m_WorldMatrix.m[3][1], m_WorldMatrix.m[3][2], 1.f);
            XMStoreFloat3(&Hit.vHitPos, vecPos);
            CEffectManager::GetInstance()->ActiveEffect(CEffectManager::BULLET, Hit, m_Projectiles[i]);

            vecPos = vecPos + vBulletDir * 2.f;
            XMStoreFloat3(&Hit.vHitPos, vecPos);
            CEffectManager::GetInstance()->ActiveEffect(CEffectManager::BULLET_TRAIL, Hit, m_Projectiles[i]);*/

            CEffectManager::GetInstance()->ActiveEffect(CEffectManager::PLAYER01_TRAIL, HIT_RESULT{}, m_Projectiles[i]);
        }
    }
    else if ((_uint)PLAYER_MODEL_INDEX::RIGHT_1004 == iModelIdx)
    {
        for (_int i = 0; i < m_Projectiles.size(); ++i)
        {
            // CEffectManager::GetInstance()->ActiveEffect(CEffectManager::BULLET, m_BulletEffectInfos[i], m_Projectiles[i]);
            // CEffectManager::GetInstance()->ActiveEffect(CEffectManager::BULLET_TRAIL, m_BulletTrailEffectInfos[i], m_Projectiles[i]);
            CEffectManager::GetInstance()->ActiveEffect(CEffectManager::PLAYER02_TRAIL, HIT_RESULT{}, m_Projectiles[i]);
        }
    }
    else if ((_uint)PLAYER_MODEL_INDEX::RIGHT_1503 == iModelIdx)
    {
        for (_int i = 0; i < m_Projectiles.size(); ++i)
        {
            // CEffectManager::GetInstance()->ActiveEffect(CEffectManager::BULLET, m_BulletEffectInfos[i], m_Projectiles[i]);
            // CEffectManager::GetInstance()->ActiveEffect(CEffectManager::BULLET_TRAIL, m_BulletTrailEffectInfos[i], m_Projectiles[i]);

            CEffectManager::GetInstance()->ActiveEffect(CEffectManager::PLAYER03_TRAIL, HIT_RESULT{}, m_Projectiles[i]);
        }
    }
    else if ((_uint)PLAYER_MODEL_INDEX::RIGHT_1508 == iModelIdx)
    {
        for (_int i = 0; i < m_Projectiles.size(); ++i)
        {
            // CEffectManager::GetInstance()->ActiveEffect(CEffectManager::BULLET, m_BulletEffectInfos[i], m_Projectiles[i]);
            // CEffectManager::GetInstance()->ActiveEffect(CEffectManager::BULLET_TRAIL, m_BulletTrailEffectInfos[i], m_Projectiles[i]);

            CEffectManager::GetInstance()->ActiveEffect(CEffectManager::PLAYER04_TRAIL, HIT_RESULT{}, m_Projectiles[i]);
        }
    }
}

void CPlayerWeapon::CreateBullet(_uint iModelIdx)
{
    _vector     vWorldFar = CalWorldEndPos();
    _vector     vSrcPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
    _vector     vBulletDir = XMVector3Normalize(vWorldFar - vSrcPos);
    _float      fCX = (_float)(g_iWinSizeX / 2.f), g_iWinSizeX;
    _float      fCY = (_float)(g_iWinSizeY / 2.f), g_iWinSizeY;

    CProjectile::PROJECTILE_DESC Desc = {};
    Desc.bUseSpawnPos = true;


     _vector vecPos = XMVectorSet(m_WorldMatrix.m[3][0], m_WorldMatrix.m[3][1], m_WorldMatrix.m[3][2], m_WorldMatrix.m[3][3]);
     // XMStoreFloat3(&Desc.vSpawnPos, vecPos);

    _vector vPlayerLook = XMVector3Normalize(static_cast<CTransform*>(m_pOwner->GetComponent(TEXT("Com_Transform")))->GetTransformState(CTransform::STATE_LOOK));
    vecPos -= vPlayerLook * 0.8f;
    XMStoreFloat3(&Desc.vSpawnPos, vecPos);

    Desc.strObjectTag = "PlayerBullet";
    XMStoreFloat3(&Desc.vDir, vBulletDir);
    Desc.fRotSpeedPerSec = 0.f;
    Desc.iLevelIndex = m_iLevelIndex;

    _vector vUp = {};
    if (m_pOwner)
    {
        CTransform* pCom = static_cast<CTransform*>(m_pOwner->GetComponent(TEXT("Com_Transform")));
        vUp = XMVector3Normalize(pCom->GetTransformState(CTransform::STATE_UP));
    }


    if ((_uint)PLAYER_MODEL_INDEX::RIGHT_1201 == iModelIdx)
    {
        Desc.fSpeedPerSec = 50.f;
        Desc.ePJType = PROJECTILE_TYPE::PJ_DEFAULT;
        SpawnBullet(Desc);
    }
    else if ((_uint)PLAYER_MODEL_INDEX::RIGHT_1004 == iModelIdx)
    {
        Desc.fSpeedPerSec = 50.f;
        Desc.ePJType = PROJECTILE_TYPE::PJ_1004;

        static _float fOffSet = 60.f;

        XMStoreFloat3(&Desc.vSpawnPos, vecPos + vUp * 0.2f);

        for (_int i = 0; i < 4; ++i)
        {
            _float fRX = m_pGameInst->ComputeRandom(-fOffSet, fOffSet);
            _float fRY = m_pGameInst->ComputeRandom(-fOffSet, fOffSet);

            _float fSX = fCX + fRX;
            _float fSY = fCY + fRY;
            _vector vDstPos = CalWorldEndPos(fSX, fSY);
            _vector vTempDir = XMVector3Normalize(vDstPos - vSrcPos);

            XMStoreFloat3(&Desc.vDir, vTempDir);

            SpawnBullet(Desc);

            // BulletInfo
            HIT_RESULT Hit = {};
            _vector vecPos = XMVectorSet(m_WorldMatrix.m[3][0], m_WorldMatrix.m[3][1], m_WorldMatrix.m[3][2], 1.f);
            XMStoreFloat3(&Hit.vHitPos, vecPos);
            XMStoreFloat3(&Hit.vNormal, vTempDir);
            m_BulletEffectInfos.push_back(Hit);

            // BulletTrailInfo
            vecPos = vecPos + vTempDir * 2.f;
            XMStoreFloat3(&Hit.vHitPos, vecPos);
            m_BulletTrailEffectInfos.push_back(Hit);
        }
    }
    else if ((_uint)PLAYER_MODEL_INDEX::RIGHT_1503 == iModelIdx)
    {
        Desc.fSpeedPerSec = 50.f;
        Desc.ePJType = PROJECTILE_TYPE::PJ_1503_DEFAULT;


        XMStoreFloat3(&Desc.vSpawnPos, vecPos + vUp * 0.2f);

        SpawnBullet(Desc);

        // BulletInfo
        HIT_RESULT Hit = {};
        _vector vecPos = XMVectorSet(m_WorldMatrix.m[3][0], m_WorldMatrix.m[3][1], m_WorldMatrix.m[3][2], 1.f);
        XMStoreFloat3(&Hit.vHitPos, vecPos);
        XMStoreFloat3(&Hit.vNormal, vBulletDir);
        m_BulletEffectInfos.push_back(Hit);

        // BulletTrailInfo
        vecPos = vecPos + vBulletDir * 2.f;
        XMStoreFloat3(&Hit.vHitPos, vecPos);
        m_BulletTrailEffectInfos.push_back(Hit);
    }
    else if ((_uint)PLAYER_MODEL_INDEX::RIGHT_1508 == iModelIdx)
    {
        Desc.fSpeedPerSec = 50.f;

        Desc.ePJType = PROJECTILE_TYPE::PJ_1508_DEFAULT;

        _vector vDstPos = {};
        for (_int i = 0; i < 3; ++i)
        {
            _float fRX = 0.f;
            _float fRY = 0.f;
            if (!i)
            {
                fRX = 0.f;
                fRY = 0.f;
            }
            else
            {
                fRX = m_pGameInst->ComputeRandom(-15.f, 15.f);
                fRY = m_pGameInst->ComputeRandom(-15.f, 15.f);
            }

            vDstPos = CalWorldEndPos(fCX + fRX, fCY + fRY);
            _vector vTempDir = XMVector3Normalize(vDstPos - vSrcPos);
            XMStoreFloat3(&Desc.vDir, vTempDir);

            XMStoreFloat3(&Desc.vSpawnPos, vecPos + vUp * 0.2f);

            SpawnBullet(Desc);

            // BulletInfo
            HIT_RESULT Hit = {};
            _vector vecPos = XMVectorSet(m_WorldMatrix.m[3][0], m_WorldMatrix.m[3][1], m_WorldMatrix.m[3][2], 1.f);
            XMStoreFloat3(&Hit.vHitPos, vecPos);
            XMStoreFloat3(&Hit.vNormal, vTempDir);
            m_BulletEffectInfos.push_back(Hit);

            // BulletTrailInfo
            vecPos = vecPos + vTempDir * 2.f;
            XMStoreFloat3(&Hit.vHitPos, vecPos);
            m_BulletTrailEffectInfos.push_back(Hit);
        }
    }
}

void CPlayerWeapon::SpawnBullet(CProjectile::PROJECTILE_DESC Desc)
{
    if (E_FAIL == m_pGameInst->SpawnObject(LEVEL_GLOBAL, TEXT("Prototype_GameObject_PlayerBullet"), m_iLevelIndex, TEXT("Layer_PlayerBullet"), &Desc)) 
        return;

    CPlayerBullet* pBullet = static_cast<CPlayerBullet*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, m_iLevelIndex, TEXT("Layer_PlayerBullet")));

    if (pBullet)
    {
        pBullet->Fire();
        CCollider* pCol = static_cast<CCollider*>(pBullet->GetComponent(TEXT("Com_Collider")));
        pCol->SetComOwner(pBullet);
        m_Projectiles.push_back(pBullet);
    }
    else
        MSG_BOX("CPlayerWeapon::SpawnBullet Error!!!");
}

HRESULT CPlayerWeapon::ReadyComponents()
{
    /* Com_Collider */
    CBoundingOBB::BOUND_OBB_DESC		ColliderDesc{};

    ColliderDesc.vExtents = _float3(0.5f, 0.5f, 0.5f);
    ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
    ColliderDesc.vRotation = _float3(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f));

    if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayerWeapon::BindShaderResources()
{
    return S_OK;
}

CPlayerWeapon* CPlayerWeapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayerWeapon* pInstance = new CPlayerWeapon(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CPlayerWeapon");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayerWeapon::Clone(void* pArg)
{
    CPlayerWeapon* pInstance = new CPlayerWeapon(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CPlayerWeapon");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CPlayerWeapon::Free()
{
    __super::Free();

}
