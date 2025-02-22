#include "pch.h"
#include "Projectile.h"
#include "GameInstance.h"
#include "WallAABB.h"
#include "Player.h"

CProjectile::CProjectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CGameObject(pDevice, pContext)
{
}

CProjectile::CProjectile(const CProjectile& Prototype)
    :
    CGameObject(Prototype)
{
}

HRESULT CProjectile::InitializePrototype()
{
    return S_OK;
}

HRESULT CProjectile::Initialize(void* pArg)
{
    if (nullptr != pArg)
    {
        PROJECTILE_DESC* pDesc = static_cast<PROJECTILE_DESC*>(pArg);
        XMStoreFloat3(&m_vDir, XMLoadFloat3(&pDesc->vDir));

        if (true == XMVector3Equal(XMLoadFloat3(&m_vDir), _vector{ 0.f, 0.f, 0.f }))
            return E_FAIL;

        m_eType = pDesc->ePJType;
    }

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pTransformCom->SetTransformState(CTransform::STATE_LOOK, XMLoadFloat3(&m_vDir));

    return S_OK;
}

void CProjectile::PriorityUpdate(_float fTimeDelta)
{

}


void CProjectile::Update(_float fTimeDelta)
{

}

void CProjectile::LateUpdate(_float fTimeDelta)
{

}

HRESULT CProjectile::Render()
{
    return S_OK;
}

_bool CProjectile::MoveProjectile(_float fTimeDelta)
{
    m_pTransformCom->GoFoward(fTimeDelta);
    return false;
}

void CProjectile::CheckColWall()
{
    if (m_bFire)
    {
        if (m_pGameInst->CheckIntersectInLayerOnlyProjectile(m_iLevelIndex, TEXT("Layer_WallAABB"), m_pColliderCom, m_HitRet))
        {
            CTransform* pPlayerCom = static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, m_iLevelIndex, TEXT("A"))->GetComponent(TEXT("Com_Transform")));

            _vector vPlayerPos = pPlayerCom->GetTransformState(CTransform::STATE_POS);
            _vector vBulletPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
            _vector vDir = XMVector3Normalize(vBulletPos - vPlayerPos);

            if (m_HitRet.pOther)
            {
                CWallAABB* pWall = static_cast<CWallAABB*>(m_HitRet.pOther);
                pWall->CalHitPosAndNormalWithType(vPlayerPos, vDir, m_eType); 
            }
        }
    }
}

void CProjectile::CheckColMonster()
{
    if (m_bFire)
    {
        for (_int i = 0; i < 7; ++i)
        {
            if (m_pGameInst->CheckIntersectInLayerOnlyProjectile(m_iLevelIndex, m_strMonsterLayers[i], m_pColliderCom, m_HitRet))
            {
                CTransform* pPlayerCom = static_cast<CTransform*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, m_iLevelIndex, TEXT("A"))->GetComponent(TEXT("Com_Transform")));
                _vector vPlayerPos = pPlayerCom->GetTransformState(CTransform::STATE_POS);
                _vector vBulletPos = m_pTransformCom->GetTransformState(CTransform::STATE_POS);
                _vector vDir = XMVector3Normalize(vBulletPos - vPlayerPos);

                if (m_HitRet.pOther)
                {
                    CMonsterBase* pMonster = static_cast<CMonsterBase*>(m_HitRet.pOther);
                    pMonster->CalHitPosAndNormal(vPlayerPos, vDir, m_eType);
                }
            }
        }
       
    }
}

void CProjectile::CheckColPlayer()
{
    if (m_bFire && !m_pColliderCom->IsEndCollision())
    {
        if (m_pGameInst->CheckIntersectInLayerOnlyProjectile(m_iLevelIndex, TEXT("A"), m_pColliderCom, m_HitRet))
        {
            if (m_HitRet.pOther)
            {
                CPlayer * pPlayer = static_cast<CPlayer*>(m_pGameInst->GetGameObjctInLayerBack(LEVEL_GLOBAL, m_iLevelIndex, TEXT("A")));

                if (pPlayer && m_HitRet.pOther == pPlayer)
                {
                    m_pColliderCom->SetEndCollision();
                    pPlayer->SetDamage(10);
                }
                else
                {
                    MSG_BOX("CProjectile::CheckColPlayer CPlayer Is NULL Error!!!");
                }
            }
        }
    }
}

void CProjectile::Free()
{
    __super::Free();

    if (nullptr != m_pShaderCom)
        SafeRelease(m_pShaderCom);

    if (nullptr != m_pTextureCom)
        SafeRelease(m_pTextureCom);
}
