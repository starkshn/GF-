#pragma once
#include "Client_Defines.h"
#include "UIElement.h"

BEGIN(Client)

class CUICrossHair final : public CUIElement
{
private:
	CUICrossHair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUICrossHair(const CUICrossHair& Prototype);
	virtual ~CUICrossHair() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	void					StartScaling() { m_bStart = true; }

private:
	_bool					m_bStart = false;

private:
	HRESULT					BindShaderResources();
	HRESULT					ReadyComponents(void* pArg);

public:
	static CUICrossHair*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUICrossHair*	Clone(void* pArg);
	virtual void			Free() override;
};

END