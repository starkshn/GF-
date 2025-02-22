#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CModel;
class CNavigation;

END

BEGIN(Client)

class CBossStage final : public CGameObject
{
private:
	CBossStage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossStage(const CBossStage& Prototype);
	virtual ~CBossStage() override = default;

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
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavMesh = nullptr;

public:
	static CBossStage*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END