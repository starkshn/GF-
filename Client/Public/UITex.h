#pragma once
#include "Client_Defines.h"
#include "UIElement.h"

BEGIN(Client)

class CUITex final : public CUIElement
{
public:
	enum UI_TEX_TYPE 
	{ 
		PLAYER_GUN_KEY_NUMBER_1, PLAYER_GUN_KEY_NUMBER_2, PLAYER_GUN_KEY_NUMBER_3, 
		PLAYER_GUN_TEX,
		PLAYER_GUN_BULLET, PLAYER_SKILL_DASH_BG, PLAYER_SKILL_DASH,
		TEX_KEY_SHIFT, TEX_KEY_E, 
		MONSTER_HP_BG, 
		MINI_MAP_PLAYER_POS, MINI_MAP_MONSTER_POS, MINI_MAP_BOSS, MINI_DOOR,
		BOSS_ICON, BOSS_HP_BG,
		PLAYER_SKILL_MID,

		PLAYER_SKILL_DASH_EFFECT, PLAYER_DAMAGED_EFFECT,

		PLAYER_SKILL_MID_ANIM,

		PLAYER_KEY_SHIFT, PLAYER_KEY_E, PLAYER_KEY_Q, PLAYER_KEY_F,

		VICTORY,

		TEX_END
	};

	enum UI_TEX_EFFECT_TYPE
	{
		UI_TEX_EFFECT_DEFAULT, UI_TEX_EFFECT_ALPHA, 
	};

public:
	typedef struct ui_tex_desc : public UI_DEFAULT_INFO
	{
		UI_TEX_TYPE eUITexType;
		UI_TEX_EFFECT_TYPE eUITexEffectType = UI_TEX_EFFECT_TYPE::UI_TEX_EFFECT_DEFAULT;

		_int iAnimRow = 0;
		_int iAnimCol = 0;
		_bool bUseAnim = false;
		_float fAnimMaxTime = 0.f;

	} UI_TEX_DESC;

	struct UI_TEX_SCALE_DESC
	{
		_float fScalingMaxSizeX;
		_float fScalingMaxSizeY;
		_float fScalingMinSizeX;
		_float fScalingMinSizeY;

		_bool bHold = false;
		_float fHoldTime = 0.f;
	};

private:
	CUITex(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUITex(const CUITex& Prototype);
	virtual ~CUITex() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	void					SetGunTexIdx(_int iIdx) { m_iGunTexIdx = iIdx; }

public:
	void					SetUseTexCoordFlex() { m_bUseTexCoordFlex = true; }
	void					SetTexCoordX(_float fTexCoordX) { m_fTexCoordX = fTexCoordX; }

public:
	void					StartAlphaEffect() 
	{
		if (m_eUITexEffectType == UI_TEX_EFFECT_ALPHA)
			m_bStartAlphaEffect = true;
	}

	void					StopAnim() { m_bStartAnim = false; }
	void					StartAnim() { m_bStartAnim = true; }

public:
	_bool					GetStartScaling() { return m_bStartScaling; }
	void					SetScalingDesc(UI_TEX_SCALE_DESC Desc);
	void					StartScaling();

private:
	UI_TEX_TYPE				m_eUITexType = UI_TEX_TYPE::TEX_END;
	UI_TEX_EFFECT_TYPE		m_eUITexEffectType = UI_TEX_EFFECT_TYPE::UI_TEX_EFFECT_DEFAULT;

private:
	vector<CTexture*>		m_pTextureGunsCom = { nullptr, nullptr, nullptr, nullptr };
	_int					m_iGunTexIdx = 0;

private:
	_bool					m_bUseTexCoordFlex = false;
	_float					m_fTexCoordX = 0.f;

private:
	_bool					m_bStartAlphaEffect = false;
	const _float			m_fMaxAlphaTime = 0.5f;
	_float					m_fAccAlphaTime = 0.f;

private:
	_bool					m_bUseAnimUI = false;
	_bool					m_bStartAnim = true;
	_int					m_iAnimRow = 0;
	_int					m_iAnimCol = 0;
	_float					m_fAnimMaxTime = 0.f;
	_float					m_fAccAnimTime = 0.f;

private:
	_bool					m_bUseScaling = false;
	_bool					m_bStartScaling = false;
	_float					m_fScalingSP = 0.f;
	_float					m_fScalingMaxSizeX = 0.f;
	_float					m_fScalingMaxSizeY = 0.f;
	_float					m_fScalingMinSizeX = 0.f;
	_float					m_fScalingMinSizeY = 0.f;
	_float					m_fCurScale = 0.f;

	_bool					m_bHold = false;
	_float					m_fHoldTime = 0.f;

private:
	HRESULT					BindShaderResources();
	HRESULT					ReadyComponents(void* pArg);

public:
	static CUITex*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUITex*			Clone(void* pArg);
	virtual void			Free() override;
};

END