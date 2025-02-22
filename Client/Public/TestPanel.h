#pragma once
#include "Client_Defines.h"
#include "UIPanel.h"

BEGIN(Client)

class CTestPanel final : public CUIPanel
{
private:
	CTestPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestPanel(const CTestPanel& Prototype);
	virtual ~CTestPanel() override = default;

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
	static CTestPanel*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CTestPanel*		Clone(void* pArg);
	virtual void			Free() override;
	
};

END