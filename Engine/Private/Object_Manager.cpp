#include "Object_Manager.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Layer.h"

CObject_Manager::CObject_Manager()
    :
    m_pGameInst(CGameInstance::GetInstance())
{
    SafeAddRef(m_pGameInst);
}


HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
    if (nullptr != m_pLayers)
        return E_FAIL;

    // 동적 배열이기 때문에 전달 받은 인자 수 == 레벨 수 만큼 동적 할당 해준다.
    m_pLayers = new LAYERS[iNumLevels];

    m_iNumLevel = iNumLevels;

    return S_OK;
}

HRESULT CObject_Manager::AddGameObjectToLayer(_uint iPrototypeLevelIdx, const _wstring& strPrototypeTag, _uint iLevelIdx, const _wstring& strLayerTag, void* pArg)
{
    // 실제 게임 오브젝트를 추가할 때 프로토 매니져로 부터 클론한 객체를 Layer에 담아준다.
    CGameObject* pGameObj = dynamic_cast<CGameObject*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_GAMEOBJ, iPrototypeLevelIdx, strPrototypeTag, pArg));
    
    if (nullptr == pGameObj)
        return E_FAIL;

    // Layer를 찾아 해당하는 Layer에 클론한 객체를 넣어준다.
    // LevelIdx, tag를 넘겨주어 해당하는 Layer를 찾아낸다.
    CLayer* pLayer = FindLayer(iLevelIdx, strLayerTag);

    // layer가 없는 경우 바로 return 하는게 아니라 레이어를 생성하고 추가를 해주어야한다.
    if (nullptr == pLayer)
    {
        pLayer = CLayer::Create();

        pLayer->AddGameObject(pGameObj);

        // Layer를 넘겨주어야 한다. (GameObject가 아니라)
        m_pLayers[iLevelIdx].emplace(strLayerTag, pLayer);
    }
    // Layer가 있는 경우 해당 레이어 바로 오브젝트 추가 해준다.
    else pLayer->AddGameObject(pGameObj);

    return S_OK;
}

void CObject_Manager::PriorityUpdate(const _float& fTimeDelta)
{
    // 레벨 수 만큼 for문 돈다.
    for (_uint i = 0; i < m_iNumLevel; ++i)
        for (const auto& pair : m_pLayers[i])
            pair.second->PriorityUpdate(fTimeDelta);
}

void CObject_Manager::Update(const _float& fTimeDelta)
{
    for (_uint i = 0; i < m_iNumLevel; ++i)
        for (const auto& pair : m_pLayers[i])
            pair.second->Update(fTimeDelta);
}

void CObject_Manager::LateUpdate(const _float& fTimeDelta)
{
    for (_uint i = 0; i < m_iNumLevel; ++i)
        for (const auto& pair : m_pLayers[i])
            pair.second->Late_Update(fTimeDelta);
}

void CObject_Manager::Clear(_uint iLevelIdx)
{
    if (iLevelIdx >= m_iNumLevel)
        return;

    for (auto& pair : m_pLayers[iLevelIdx])
    {
        if (nullptr != pair.second)
            SafeRelease(pair.second);
    }

    m_pLayers[iLevelIdx].clear();
}

CGameObject* CObject_Manager::GetGameObjectInLayer(_uint iPrototypeLevelIdx, _uint iLevelIdx, const _wstring& strLayerTag, const string& strObjectTag)
{
    if (iLevelIdx >= m_iNumLevel)
        return nullptr;

    CLayer* pLayer = FindLayer(iLevelIdx, strLayerTag);
    if (nullptr == pLayer)
        return nullptr;

    CGameObject* pGameObject = pLayer->GetGameObject(strObjectTag);
    if (nullptr == pGameObject)
        return nullptr;

    return pGameObject;
}

CGameObject* CObject_Manager::GetGameObjectInLayerBack(_uint iPrototypeLevelIdx, _uint iLevelIdx, const _wstring& strLayerTag)
{
    if (iLevelIdx >= m_iNumLevel)
        return nullptr;

    CLayer* pLayer = FindLayer(iLevelIdx, strLayerTag);
    if (nullptr == pLayer)
        return nullptr;

    CGameObject* pGameObject = pLayer->GetGameObjectBack();

    if (nullptr == pGameObject)
        return nullptr;
    else 
        return pGameObject;
}

CComponent* CObject_Manager::GetComponent(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iObjectIndex)
{
    auto pLayer = FindLayer(iLevelIndex, strLayerTag);
    
    if (nullptr == pLayer)
        return nullptr;

    return pLayer->GetComponent(strComponentTag, iObjectIndex);
}

CComponent* CObject_Manager::GetComponent(_uint iLevelIndex, const _wstring& strLayerTag, _uint iPartObjectIndex, const _wstring& strPartComponentTag, _uint iObjectIndex)
{
    // 컨테이너 오브젝트에 해당하는 객체가 포함되어 있는 레이어를 찾고 찾는 컨테이너 오브젝트의 파트 오브젝트의 컴포넌트를 리턴한다.
    auto pLayer = FindLayer(iLevelIndex, strLayerTag);

    if (nullptr == pLayer)
        return nullptr;

    return pLayer->GetComponent(iPartObjectIndex, strPartComponentTag, iObjectIndex);
}

HRESULT CObject_Manager::SpawnObjectAtPickingPos(_uint iPrototypeLevelIdx, const _wstring& strPrototypeTag, _uint iLevelIdx, const _wstring& strLayerTag, _float3 vSpawnPos, string strName, void* pArg)
{
    if (iLevelIdx >= m_iNumLevel)
        return E_FAIL;

    CGameObject::GAMEOBJECT_DESC Desc;
    Desc.strObjectTag = strName;
    Desc.bUseSpawnPos = true;
    Desc.vSpawnPos = vSpawnPos;

    // 실제 게임 오브젝트를 추가할 때 프로토 매니져로 부터 클론한 객체를 Layer에 담아준다.
    CGameObject* pGameObj = dynamic_cast<CGameObject*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_GAMEOBJ, iPrototypeLevelIdx, strPrototypeTag, &Desc));

    if (nullptr == pGameObj)
        return E_FAIL;

    CLayer* pLayer = FindLayer(iLevelIdx, strLayerTag);

    if (nullptr == pLayer)
    {
        pLayer = CLayer::Create();

        pLayer->AddGameObject(pGameObj);

        // Layer를 넘겨주어야 한다. (GameObject가 아니라)
        m_pLayers[iLevelIdx].emplace(strLayerTag, pLayer);
    }
    // Layer가 있는 경우 해당 레이어 바로 오브젝트 추가 해준다.
    else 
        pLayer->AddGameObject(pGameObj);

    return S_OK;
}

HRESULT CObject_Manager::SpawnObject(_uint iPrototypeLevelIdx, const _wstring& strPrototypeTag, _uint iLevelIdx, const _wstring& strLayerTag, void* pArg)
{
    if (iLevelIdx >= m_iNumLevel)
        return E_FAIL;

    // 실제 게임 오브젝트를 추가할 때 프로토 매니져로 부터 클론한 객체를 Layer에 담아준다.
    CGameObject* pGameObj = dynamic_cast<CGameObject*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_GAMEOBJ, iPrototypeLevelIdx, strPrototypeTag, pArg));

    if (nullptr == pGameObj)
        return E_FAIL;

    CLayer* pLayer = FindLayer(iLevelIdx, strLayerTag);

    if (nullptr == pLayer)
    {
        pLayer = CLayer::Create();

        pLayer->AddGameObject(pGameObj);

        // Layer를 넘겨주어야 한다. (GameObject가 아니라)
        m_pLayers[iLevelIdx].emplace(strLayerTag, pLayer);
    }
    // Layer가 있는 경우 해당 레이어 바로 오브젝트 추가 해준다.
    else
        pLayer->AddGameObject(pGameObj);

    return S_OK;
}

CLayer* CObject_Manager::FindLayer(_uint iLevelIdx, const _wstring& strLayerTag)
{
    if (iLevelIdx >= m_iNumLevel)
        return nullptr;

    // 유쌤 현재 키값이 wstring인데 멀티 스레드 || 멀티 프로세스 일 때 find가 제대로 동작 안할 수도 있다는데 
    // 무슨 말인지 질문하기
    auto iter = m_pLayers[iLevelIdx].find(strLayerTag);
    if (iter == m_pLayers[iLevelIdx].end())
        return nullptr;

    return iter->second;
}

CGameObject* CObject_Manager::GetPickedGameObject(_uint iLevelID, _fvector vRayOrigin, _fvector vRayDir, _float& fDist, _float4& vPickingPos)
{
    for (auto& pair : m_pLayers[iLevelID])
    {
        if (CGameObject* pGameObject = pair.second->GetPickedGameObject(vRayOrigin, vRayDir, fDist, vPickingPos))
        {
            return pGameObject;
        }
    }

    return nullptr;
}

_bool CObject_Manager::RemoveObject(_uint iLevelID, CGameObject* pRemoveObject)
{
    for (auto Pair : m_pLayers[iLevelID])
    {
        if (Pair.second->RemoveFromLayer(pRemoveObject))
            return true;
    }

    return false;
}

CObject_Manager* CObject_Manager::Create(_uint iNumLevels)
{
    CObject_Manager* pInst = new CObject_Manager{};

    if (FAILED(pInst->Initialize(iNumLevels)))
    {
        MSG_BOX("Failed to Created : CObject_Manager");
        SafeRelease(pInst);
    }

    return pInst;
}

void CObject_Manager::Free()
{
    __super::Free();

    for (_uint i = 0; i < m_iNumLevel; ++i)
    {
        // Layer Release 작업 진행한다.
        for (auto& pair : m_pLayers[i])
            SafeRelease(pair.second);

         m_pLayers[i].clear();
    }
    SafeDeleteArray(m_pLayers);

    SafeRelease(m_pGameInst);
}