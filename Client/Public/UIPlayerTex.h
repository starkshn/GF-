#pragma once
#include "Client_Defines.h"
#include "UIElement.h"

BEGIN(Client)

class CUIPlayerTex final : public CUIElement
{
private:
	CUIPlayerTex(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPlayerTex(const CUIPlayerTex& Prototype);
	virtual ~CUIPlayerTex() override = default;

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
	static CUIPlayerTex*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUIPlayerTex*	Clone(void* pArg);
	virtual void			Free() override;
};

END