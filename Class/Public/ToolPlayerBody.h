#pragma once

#include "PartObject.h"
#include "Model.h"
BEGIN(Engine)

class CShader;
class CCollider;

END

BEGIN(Tool)

class CToolPlayerBody final : public CPartObject
{
public:
	typedef struct tool_player_body_desc : public PARTOBJECT_DESC
	{

		_uint* pState;

	} TOOL_PLAYER_BODY_DESC;

private:
	CToolPlayerBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CToolPlayerBody(const CToolPlayerBody& Prototype);
	virtual ~CToolPlayerBody() = default;



public:
	virtual HRESULT				InitializePrototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				PriorityUpdate(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				LateUpdate(_float fTimeDelta) override;
	virtual HRESULT				Render() override;
	virtual HRESULT				RenderShadow() override;

public:
	const _float4x4*			GetBoneCombiedMatrix(const _char* pBoneName) const { return m_pModelCom->GetBoneCombinedMatrix(pBoneName); }

private:
	CShader*					m_pShaderCom = nullptr;
	CModel*						m_pModelCom = nullptr ;
	CCollider*					m_pColliderCom = nullptr;

private:
	_uint*						m_pState = { nullptr };

private:
	HRESULT						ReadyComponents();
	HRESULT						BindShaderResources();

public:
	static CToolPlayerBody*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END