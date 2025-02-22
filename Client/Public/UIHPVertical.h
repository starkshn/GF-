#pragma once
#include "Client_Defines.h"
#include "UIElement.h"

BEGIN(Client)

class CUIHPVertical final : public CUIElement
{
private:
	CUIHPVertical(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIHPVertical(const CUIHPVertical& Prototype);
	virtual ~CUIHPVertical() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;


private:
	const _float			m_fScalingTimeMax = 0.2f;
	_float					m_fAcc = 0.f;
	_bool					m_bOnce = 1;
	_float					m_fAccReal = 0;

private:
	HRESULT					BindShaderResources();
	HRESULT					ReadyComponents(void* pArg);

public:
	static CUIHPVertical*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUIHPVertical*	Clone(void* pArg);
	virtual void			Free() override;
};

END