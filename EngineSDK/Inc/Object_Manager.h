#pragma once
#include "Base.h"

/* 사본 객체들을 보관한다. */

BEGIN(Engine)

class CLayer;
class CGameInstance;
class CGameObject;
class CComponent;
typedef map<const _wstring, CLayer*> LAYERS;


class ENGINE_DLL CObject_Manager : public CBase
{
public:
	CObject_Manager();
	virtual ~CObject_Manager() override = default;

public:
	HRESULT							Initialize(_uint iNumLevels);
	HRESULT							AddGameObjectToLayer(_uint iPrototypeLevelIdx, const _wstring& strPrototypeTag, _uint iLevelIdx, const _wstring& strLayerTag, void* pArg = nullptr);
	void							PriorityUpdate(const _float& fTimeDelta);
	void							Update(const _float& fTimeDelta);
	void							LateUpdate(const _float& fTimeDelta);
	void							Clear(_uint iLevelIdx);


public:
	CGameObject*					GetGameObjectInLayer(_uint iPrototypeLevelIdx, _uint iLevelIdx, const _wstring& strLayerTag, const string& strObjectTag);
	CGameObject*					GetGameObjectInLayerBack(_uint iPrototypeLevelIdx, _uint iLevelIdx, const _wstring& strLayerTag);
	CComponent*						GetComponent(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iObjectIndex = 0);
	CComponent*						GetComponent(_uint iLevelInde, const _wstring& strLayerTag, _uint iPartObjectIndex, const _wstring& strPartComponentTag, _uint iObjectIndex = 0);
	CGameObject*					GetPickedGameObject(_uint iLevelID, _fvector vRayOrigin, _fvector vRayDir, _float& fDist, _float4& vPickingPos);

public:
	HRESULT							SpawnObjectAtPickingPos(_uint iPrototypeLevelIdx, const _wstring& strPrototypeTag, _uint iLevelIdx, const _wstring& strLayerTag, _float3 vSpawnPos, string strName, void* pArg = nullptr);
	HRESULT							SpawnObject(_uint iPrototypeLevelIdx, const _wstring& strPrototypeTag, _uint iLevelIdx, const _wstring& strLayerTag, void* pArg = nullptr);

public:
	CLayer*							FindLayer(_uint iLevelIdx, const _wstring& strLayerTag);

public:

	_bool							RemoveObject(_uint iLevelID, CGameObject* pRemoveObject);

private:
	_uint							m_iNumLevel = 0;
	map<const _wstring, CLayer*>*	m_pLayers = nullptr;
	CGameInstance*					m_pGameInst = nullptr;

public:
	static CObject_Manager*			Create(_uint iNumLevels);
	virtual void					Free() override;

};

END