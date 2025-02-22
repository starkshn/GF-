#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CAlphaObject abstract : public CGameObject
{
protected:
	CAlphaObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAlphaObject(const CAlphaObject& Prototype);
	virtual ~CAlphaObject() = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

protected:
	void					ComputeViewZ();

public:
	_float					GetViewZ() const { return m_fViewZ; }

protected:
	_float					m_fViewZ = 0.f;

public:
	virtual CGameObject*	Clone(void* pArg) = 0;
	virtual void			Free() override;
};

END