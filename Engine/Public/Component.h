#pragma once
#include "Base.h"

BEGIN(Engine)

class CGameInstance;
class CGameObject;

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent(const CComponent& Prototype);
	virtual ~CComponent() override = default;

public:
	virtual	HRESULT				InitializePrototype();
	virtual	HRESULT				Initialize(void* pArg);
	
#ifdef _DEBUG

public:
	virtual	HRESULT				Render() { return S_OK; }

#endif

public:
	void						SetComOwner(CGameObject* pOwner) { m_pOwner = pOwner; }
	CGameObject*				GetComOwner() 
	{
		return m_pOwner;
	}

protected:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	CGameInstance*				m_pGameInst = nullptr;
	_bool						m_bIsCloned = false;
	CGameObject*				m_pOwner = nullptr;
	
public:
	virtual CComponent*			Clone(void* pArg) abstract;
	virtual void				Free() override;
};

END