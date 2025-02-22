#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CModel;

END

BEGIN(Client)

class CStage01 final : public CGameObject
{
private:
	CStage01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStage01(const CStage01& Prototype);
	virtual ~CStage01() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	HRESULT					ReadyComponents();
	HRESULT					BindShaderResources();

private:
	CShader* m_pShaderCom = nullptr;
	CModel* m_pModelCom = nullptr;

public:
	static CStage01*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END