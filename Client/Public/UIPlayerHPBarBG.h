#pragma once
#include "Client_Defines.h"
#include "UIElement.h"

BEGIN(Client)

class CUIPlayerHPBarBG : public CUIElement
{
private:
	CUIPlayerHPBarBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPlayerHPBarBG(const CUIPlayerHPBarBG& Prototype);
	virtual ~CUIPlayerHPBarBG() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	HRESULT					BindShaderResources();
	HRESULT					ReadyComponents(void* pArg);

public:
	static CUIPlayerHPBarBG* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUIPlayerHPBarBG* Clone(void* pArg);
	virtual void			Free() override;
};

END