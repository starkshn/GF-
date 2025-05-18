#include "Layer.h"
#include "GameObject.h"
#include "ColBoundingBox.h"
#include "ContainerObject.h"
#include "Component.h"
#include "Collider.h"

CLayer::CLayer()
{
    m_GameObjects.clear();
}

HRESULT CLayer::AddGameObject(CGameObject* pGameObject)
{
    if (nullptr == pGameObject)
        return E_FAIL;

    m_GameObjects.push_back(pGameObject);

    return S_OK;
}

void CLayer::PriorityUpdate(const _float& fTimeDelta)
{
    for (auto& pGameObj : m_GameObjects)
        pGameObj->PriorityUpdate(fTimeDelta);
}

void CLayer::Update(const _float& fTimeDelta)
{
    for (auto& pGameObj : m_GameObjects)
    {
        if (pGameObj)
            pGameObj->Update(fTimeDelta);
    }
}

void CLayer::Late_Update(const _float& fTimeDelta)
{
    for (auto& pGameObj : m_GameObjects)
        pGameObj->LateUpdate(fTimeDelta);
}

CGameObject* CLayer::GetGameObject(const string& strObjectTag)
{
    for (auto& pGameObj : m_GameObjects)
        if (false == strcmp(strObjectTag.c_str(), pGameObj->GetObjectTag().c_str()))
            return pGameObj;

    return nullptr;
}

CGameObject* CLayer::GetGameObjectBack()
{
    return m_GameObjects.back();
}

CGameObject* CLayer::GetPickedGameObject(_fvector vRayOrigin, _fvector vRayDir, _float& fDist, _float4& vPickingPos)
{
    for (auto& pGameObj : m_GameObjects)
    {
        CComponent* pComp = pGameObj->GetComponent(TEXT("Com_BoundingBox"));
        if (nullptr != pComp)
        {
            CColBoundingBox* pBB = dynamic_cast<CColBoundingBox*>(pComp);
            if (nullptr != pBB && pBB->Check_Picking(vRayOrigin, vRayDir, fDist, vPickingPos))
            {
                return pGameObj;
            }
        }
        else
        {
            CComponent* pColCom = pGameObj->GetComponent(TEXT("Com_Collider"));
            if (nullptr != pColCom)
            {
                CCollider* pCol = static_cast<CCollider*>(pColCom);
                // pCol->Intersect()
                fDist = 10000.f;
                if (pCol->Intersect(vRayOrigin, vRayDir, fDist, vPickingPos))
                {
                    return pGameObj;
                }
            }
                
        }
    }
    return nullptr;
}

CComponent* CLayer::GetComponent(const _wstring& strComponentTag, _uint iObjectIndex)
{
    if (iObjectIndex >= m_GameObjects.size())
        return nullptr;

    auto iter = m_GameObjects.begin();

    for (_uint i = 0; i < iObjectIndex; ++i) 
        ++iter;

    return (*iter)->GetComponent(strComponentTag);
}

CComponent* CLayer::GetComponent(_uint iPartObjectIndex, const _wstring& strPartComponentTag, _uint iObjectIndex)
{
    if (iObjectIndex >= m_GameObjects.size())
        return nullptr;

    auto iter = m_GameObjects.begin();

    for (_uint i = 0; i < iObjectIndex; ++i)
        ++iter;

    return static_cast<CContainerObject*>(*iter)->GetComponent(iPartObjectIndex, strPartComponentTag);
}

_bool CLayer::CheckIntersect(const CCollider* pSrc)
{
    for (auto pObj : m_GameObjects)
        if (nullptr != pObj->GetComponent(TEXT("Com_Collider")))
            if (static_cast<CCollider*>(pObj->GetComponent(TEXT("Com_Collider")))->Intersect(pSrc))
                return true;

    return false;
}

_bool CLayer::CheckIntersect(const CCollider* pSrcAABB, HIT_RESULT& HitResult)
{
    for (auto pObj : m_GameObjects)
    {
        if (nullptr != pObj->GetComponent(TEXT("Com_Collider")))
        {
            CCollider* pOtherCol = static_cast<CCollider*>(pObj->GetComponent(TEXT("Com_Collider")));
            if (pOtherCol->IsEndCollision())
                continue;
            if (false == pOtherCol->IsActiveCol())
                continue;
            if (pOtherCol->Intersect(pSrcAABB, HitResult))
                return true;
        }
    }

    return false;
}

// Projectile 객체만 아래 함수를 호출한다. 
// 상대방은 Wall, 또는 몬스터 이다.
_bool CLayer::CheckIntersectOnlyProjectile(CCollider* pProjectileCol, HIT_RESULT& HitResult)
{
    for (auto pObj : m_GameObjects)
    {
        if (nullptr != pObj->GetComponent(TEXT("Com_Collider")))
        {
            CCollider* pOtherCol = static_cast<CCollider*>(pObj->GetComponent(TEXT("Com_Collider")));
            if (pOtherCol->IsEndCollision())
                continue;
            if (false == pOtherCol->IsActiveCol())
                continue;
            if (pOtherCol->IntersectOnlyProjectile(pProjectileCol, HitResult))
                return true;
        }
    }

    return false;
}

_bool CLayer::RemoveFromLayer(CGameObject* pRemoveObject)
{
    auto iter = find_if(m_GameObjects.begin(), m_GameObjects.end(), [pRemoveObject](const CGameObject* pObject) { return pRemoveObject == pObject; });

    // 찾았다.
    if (iter != m_GameObjects.end())
    {
        SafeRelease((*iter));
        m_GameObjects.remove(*iter);
        return true;
    }

    return false;
}

void CLayer::ActiveMonsters()
{
    for_each(m_GameObjects.begin(), m_GameObjects.end(), [](CGameObject* pGameObject)
    {
        if (false == pGameObject->GetActive()) 
            pGameObject->SetActive(true); 
    });
}

CLayer* CLayer::Create()
{
    return new CLayer;
}

void CLayer::Free()
{
    __super::Free();

    for (auto& pGameObj : m_GameObjects)
        if (nullptr != pGameObj)
            SafeRelease(pGameObj);

    m_GameObjects.clear();
}
