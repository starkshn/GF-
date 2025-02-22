#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"


BEGIN(Engine)

class CShader;
class CModel;
class CColBoundingBox;

END

BEGIN(Tool)

class CMonster02 final : public CGameObject
{
private:
	CMonster02(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster02(const CMonster02& Prototype);
	virtual ~CMonster02() override = default;

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

private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CColBoundingBox*		m_pBoundingBox = nullptr;

public:
	static CMonster02*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END