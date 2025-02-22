#pragma once
#include "Client_Defines.h"
#include "UIPanel.h"

BEGIN(Client)

class CUIPlayerInfoBG final : public CUIPanel
{
private:
	CUIPlayerInfoBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPlayerInfoBG(const CUIPlayerInfoBG& Prototype);
	virtual ~CUIPlayerInfoBG() override = default;

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
	static CUIPlayerInfoBG* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUIPlayerInfoBG* Clone(void* pArg);
	virtual void			Free() override;
};

END