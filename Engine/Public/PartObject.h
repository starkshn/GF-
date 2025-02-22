#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CPartObject abstract : public CGameObject
{
public:
	typedef struct partobject_desc : public GAMEOBJECT_DESC
	{
		const _float4x4* pParentMatrix;			// 부모의 행렬을 전달받기 위해 가지고 있어준다.

	} PARTOBJECT_DESC;
	
protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& Prototype);
	virtual ~CPartObject() = default;

public:
	virtual HRESULT		InitializePrototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		PriorityUpdate(_float fTimeDelta) override;
	virtual void		Update(_float fTimeDelta) override;
	virtual void		LateUpdate(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

	_matrix				GetSocketWorldMatrix() { return XMLoadFloat4x4(&m_WorldMatrix); }


protected:
	const _float4x4*	m_pParentMatrix = {};
	_float4x4			m_WorldMatrix = {};

public:
	virtual void		Free() override;
};

END