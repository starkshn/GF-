#pragma once
#include "GameObject.h"
#include "Model.h"

BEGIN(Engine)

class ENGINE_DLL CContainerObject abstract : public CGameObject
{
public:
	typedef struct container_descs : public GAMEOBJECT_DESC
	{
		_uint	iNumPartObjects;

	} CONTAINER_OBJECT_DESC;

protected:
	CContainerObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CContainerObject(const CContainerObject& Prototype);
	virtual ~CContainerObject() = default;

public:
	virtual HRESULT				InitializePrototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				PriorityUpdate(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				LateUpdate(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	virtual CComponent*			GetComponent(const _wstring& strCompTag) override;
	virtual CComponent*			GetComponent(_uint iPartObjectIndex, const _wstring& strPartComponentTag);

	/* 이 객체를 구성하기위한 부품(몸, 헤드, 무기, 이펙트, 사운드) 객체들 */
protected:
	vector<CGameObject*>		m_PartObjects;

public:
	virtual CGameObject*		Clone(void* pArg) abstract;
	virtual void				Free() override;
	
};

END