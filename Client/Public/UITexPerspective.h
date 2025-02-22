#pragma once
#include "Client_Defines.h"
#include "UIElement.h"

BEGIN(Client)

class CUITexPerspective final : public CUIElement
{
public:
	enum UI_TEX_PERSPECTIVE_TYPE { UI_TEX_P_DOOR, UI_TEX_P_END, };

public:
	typedef struct ui_tex_p_desc : UI_DEFAULT_INFO
	{
		UI_TEX_PERSPECTIVE_TYPE eUITexPType;	
	} UI_TEX_P_DESC;

private:
	CUITexPerspective(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUITexPerspective(const CUITexPerspective& Prototype);
	virtual ~CUITexPerspective() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	void					SetTargetObj(CGameObject* pTargetObj) { m_pTargetObj = pTargetObj; }
	CGameObject*			GetTargetObj() { return m_pTargetObj; }

private:
	UI_TEX_PERSPECTIVE_TYPE	m_eUITexPType = UI_TEX_P_END;
	CGameObject*			m_pTargetObj = nullptr;

private:
	HRESULT					BindShaderResources();
	HRESULT					ReadyComponents(void* pArg);

public:
	static CUITexPerspective* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUITexPerspective*	Clone(void* pArg);
	virtual void			Free() override;
};

END