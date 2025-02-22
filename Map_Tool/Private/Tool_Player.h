#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"
#include "Model.h"

BEGIN(Engine)

class CShader;
class CColBoundingBox;
class CCollider;
class CNavigation;

END

BEGIN(Tool)

class CTool_Player final : public CGameObject
{
private:
	CTool_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTool_Player(const CTool_Player& Prototype);
	virtual ~CTool_Player() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	HRESULT					Ready_Components();
	HRESULT					Bind_ShaderResources();

public:
	_uint					Get_NumModels() { return m_iNumModels; }
	void					Increase_ModelNum() { if (m_iCurrentModelIndex + 1 < m_iNumModels) ++m_iCurrentModelIndex; }
	void					Decrease_ModelNum() {  if (m_iCurrentModelIndex - 1 >= 0) --m_iCurrentModelIndex; }
	CModel*					Get_CurrentModel() { return m_pModelComs[m_iCurrentModelIndex]; }
	

private:
	CShader*				m_pShaderCom = nullptr;
	vector<CModel*>			m_pModelComs;
	CCollider*				m_pColliderCom = nullptr;
	CColBoundingBox*		m_pBoundingBox = nullptr;
	CNavigation*			m_pNavCom = nullptr;

private:
	_int					m_iCurrentModelIndex = 0;
	_int					m_iNumModels = 8;

public:
	static CTool_Player*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END