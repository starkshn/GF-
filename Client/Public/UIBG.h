#pragma once
#include "Client_Defines.h"
#include "UIPanel.h"

BEGIN(Client)

class CUIBG final : public CUIPanel
{
public:
	enum UI_BG_TYPE { PLAYER_GUN_INFO, PLAYER_SKILL, PLAYER_SKILL_MID, MOSTER_HP, MINI_MAP, BOSS_HP, BG_END};

public:
	typedef struct ui_bg_desc : public CUIParent::UI_DEFAULT_INFO
	{
		UI_BG_TYPE eBGType;
	} UI_BG_DESC;

private:
	CUIBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIBG(const CUIBG& Prototype);
	virtual ~CUIBG() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	UI_BG_TYPE				m_eBGType = UI_BG_TYPE::BG_END;

	_bool					m_bDotRender = true;

private:
	HRESULT					BindShaderResources();
	HRESULT					ReadyComponents(void* pArg);

public:
	static CUIBG*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUIBG*			Clone(void* pArg);
	virtual void			Free() override;
};

END