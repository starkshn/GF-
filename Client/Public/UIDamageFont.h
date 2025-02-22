#pragma once
#include "Client_Defines.h"
#include "UIElement.h"

BEGIN(Client)

class CUIDamageFont final : public CUIElement
{
public:
	enum UI_DAMAGE_FONT_COLOR { UI_DAMAGE_FONT_DEFAULT, UI_DAMAGE_FONT_YELLOW,  };

	typedef struct ui_damage_font_desc : public UI_DEFAULT_INFO
	{
		UI_DAMAGE_FONT_COLOR eUIDamageFontColor;

		_float fStartScale;
		_float fEndSacle;
		_float fScalingSP;

	} UI_DAMAGE_FONT_DESC;

private:
	CUIDamageFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIDamageFont(const CUIDamageFont& Prototype);
	virtual ~CUIDamageFont() override = default;

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

private:
	UI_DAMAGE_FONT_COLOR	m_eUIDamageColor = UI_DAMAGE_FONT_DEFAULT;
	_float					m_fStartScale = 0.f;
	_float					m_fEndScale = 0.f;
	_float					m_fScalingSP = 0.f;

	_float					m_fAcc = 0.f;

private:
	CTexture*				m_pTextureComs[12];

public:
	static CUIDamageFont*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUIDamageFont*	Clone(void* pArg);
	virtual void			Free() override;

};

END