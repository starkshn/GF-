#pragma once

#include "PartObject.h"
#include "Model.h"

BEGIN(Engine)

class CShader;
class CModel;
class CCollider;

END

BEGIN(Tool)

class CToolPlayerWeapon final : public CPartObject
{
public:
	typedef struct tool_player_weapon_desc : CPartObject::PARTOBJECT_DESC
	{
		_uint* pState;
		const _float4x4* pSocketMatrix;
	}	TOOL_PLAYER_WEAPON_DESC;

private:
	CToolPlayerWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CToolPlayerWeapon(const CToolPlayerWeapon& Prototype);
	virtual ~CToolPlayerWeapon() = default;
	
public:
	virtual HRESULT					InitializePrototype() override;
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void					PriorityUpdate(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					LateUpdate(_float fTimeDelta) override;
	virtual HRESULT					Render() override;
	virtual HRESULT					RenderShadow() override;


private:
	CShader*						m_pShaderCom = nullptr;
	CModel*							m_pModelCom = nullptr;
	CCollider*						m_pColliderCom = nullptr;

private:
	_uint*							m_pState = { nullptr };
	const _float4x4*				m_pSocketMatrix = { nullptr };

private:
	HRESULT							ReadyComponents();
	HRESULT							BindShaderResources();

public:
	static CToolPlayerWeapon*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override;
};

END