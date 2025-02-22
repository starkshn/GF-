#pragma once
#include "Transform.h"

BEGIN(Engine)

class CGameInstance;

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct GameObjDesc : public CTransform::TRANSFORM_DESC
	{
		string	strObjectTag;
		_float3 vSpawnPos;
		_bool	bUseSpawnPos = false;
		_uint	iLevelIndex;

	} GAMEOBJECT_DESC;

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& ProtoType);
	virtual ~CGameObject() override = default;

public:
	virtual HRESULT						InitializePrototype();				// 원형이 호출할 함수
	virtual HRESULT						Initialize(void* pArg);				// 사본이 호출 할 함수
	virtual void						PriorityUpdate(_float fTimeDelta);
	virtual void						Update(_float fTimeDelta);
	virtual void						LateUpdate(_float fTimeDelta);
	virtual HRESULT						Render();
	
	// 필요한 애들만 override해준다.
	virtual HRESULT						RenderHeight() { return S_OK; }
	virtual HRESULT						RenderShadow() { return S_OK; }

public:
	void								SetActive(_bool bActive)
	{ 
		m_bActive = bActive; 
	}

	_bool								GetActive() { return m_bActive; }


public:
	HRESULT								AddComponent(_uint iPrototypeLevelIdx, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);
	virtual CComponent*					GetComponent(const _wstring& strCompTag);

public:
	virtual void						CalNotAccuarteHitPosAndNormal(_fvector vDstPos, _fvector vNotAccuarteFromPos, _fvector vFromDir, _float3& vOutPos, _float3& vOutNormal) {};

public:
	string								GetObjectTag() { return m_strObjectTag; }

public:
	virtual void						CalHitPosAndNormal(_fvector vPlayerPos, _fvector vPlayerToDir) {};

public:
	void								SetRemoveFromLayerNextTick() { m_bRemoveFromLayer = true; }
	_bool								GetRemoveFromLayerNextTick() { return m_bRemoveFromLayer; }

public:
	_bool								IsInFrustum() { return m_bIsInFrustum; }
	void								SetInFrustum() { m_bIsInFrustum = true; }
	void								SetOutFrustum() { m_bIsInFrustum = false; }


public:
	_bool								IsObjInCam(_uint iGlobal);

private:
	_bool								m_bActive = false;
	
protected:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
	CGameInstance*						m_pGameInst = nullptr;
	CTransform*							m_pTransformCom = { nullptr };

protected:
	map<const _wstring, CComponent*>	m_Components;
	string								m_strObjectTag = "NONE";
	_float3								m_vSpawnPos = {};
	_uint								m_iLevelIndex = -1;
	HIT_RESULT							m_HitRet = {};


private:
	_bool								m_bRemoveFromLayer = false;



private:
	_bool								m_bIsInFrustum = false;
	

public:
	virtual CGameObject*				Clone(void* pArg) abstract;
	virtual void						Free() override;
};

END