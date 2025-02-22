#include "Prototype_Manager.h"
#include "GameObject.h"
#include "Component.h"

CPrototype_Manager::CPrototype_Manager()
{
	
}

HRESULT CPrototype_Manager::Initialize(_uint iNumLevels)
{
	if (nullptr != m_pPrototypes)
		return E_FAIL;

	// iNumLevels 클라의 레벨 개수 만큼 동적할당 하겠다.
	m_pPrototypes = new PROTOTYPE[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CPrototype_Manager::AddPrototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype)
{
	if (iLevelIndex >= m_iNumLevels || nullptr != FindPrototype(iLevelIndex, strPrototypeTag))
		return E_FAIL;

	// 왜 insert를 안 쓰고 emplace를 썻나?
	// - https://www.notion.so/std-map-fdb0babe92d6482ea21e03d02a0fdefe?pvs=4 
	auto ret = m_pPrototypes[iLevelIndex].emplace(strPrototypeTag, pPrototype);
	
	return S_OK;
}

CBase* CPrototype_Manager::ClonePrototype(Engine::PROTOTYPE eType, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	CBase* pProtoType = FindPrototype(iLevelIndex, strPrototypeTag);
	if (!pProtoType) 
		return nullptr;

	CBase* pCopyObj = nullptr;
	
	switch (eType)
	{
	case Engine::PROTOTYPE::PROTO_GAMEOBJ:
		pCopyObj = dynamic_cast<CGameObject*>(pProtoType)->Clone(pArg);
		break;
	case Engine::PROTOTYPE::PROTO_COMPONENT:
		pCopyObj = dynamic_cast<CComponent*>(pProtoType)->Clone(pArg);
		break;
	default:
		break;
	}

	if (nullptr == pCopyObj)
		return nullptr;

	return pCopyObj;
}

void CPrototype_Manager::Clear(_uint iLevelIdx)
{
	if (iLevelIdx >= m_iNumLevels)
		return;
	
	for (auto& pair : m_pPrototypes[iLevelIdx])
		SafeRelease(pair.second);

	m_pPrototypes[iLevelIdx].clear();
}

CBase* CPrototype_Manager::FindPrototype(_uint iLevelIndex, const _wstring& strPrototypeTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto iter = m_pPrototypes[iLevelIndex].find(strPrototypeTag);
	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

CPrototype_Manager* CPrototype_Manager::Create(_uint iNumLevels)
{
	CPrototype_Manager* pInst = new CPrototype_Manager();

	if (FAILED(pInst->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CPrototype_Manager");
		SafeRelease(pInst);
	}

	return pInst;
}

void CPrototype_Manager::Free()
{
	__super::Free();

	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& pair : m_pPrototypes[i])
			SafeRelease(pair.second);

		m_pPrototypes[i].clear();
	}

	SafeDeleteArray(m_pPrototypes);
}
