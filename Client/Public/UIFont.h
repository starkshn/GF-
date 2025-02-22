#pragma once
#include "Client_Defines.h"
#include "UIElement.h"

BEGIN(Client)

class CUIFont final : public CUIElement
{
public:
	enum FONT_COLOR { FONT_DEFAULT, FONT_BLUE, FONT_GREEN};

public:
	typedef struct font_desc : public UI_DEFAULT_INFO
	{
		FONT_COLOR eFontColor;
		_float fFontScalingSP;

	} UI_FONT_DESC;


private:
	CUIFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIFont(const CUIFont& Prototype);
	virtual ~CUIFont() override = default;

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

public:
	_uint					GetNum() { return m_iNum; }
	void					SetNum(_uint iNum) { m_iNum = iNum; }

public:
	void					StartIncreaseScaling();
	void					StartDecreaseScaling();
	void					StopScaling();

private:
	void					IncreaseScaling(_float fTimeDelta);
	void					DecreaseScaling(_float fTimeDelta);

private:
	_uint					m_iNum = 0;
	FONT_COLOR				m_eFontColor = FONT_COLOR::FONT_DEFAULT;

private:
	_float					m_fFontScalingSP = 0.f;
	_float					m_fMaxSizeX = 0.f;
	_float					m_fMaxSizeY = 0.f;
	_float					m_fMinSizeX = 0.f;
	_float					m_fMinSizeY = 0.f;
	_float					m_fAccSizeX = 0.f;
	_float					m_fAccSizeY = 0.f;
	_bool					m_bStartIn = false;
	_bool					m_bStartDe = false;

private:
	CTexture*				m_pTextureComs[12];


public:
	static CUIFont*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUIFont*		Clone(void* pArg);
	virtual void			Free() override;
};

END