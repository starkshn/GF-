#pragma once
#include "Client_Defines.h"
#include "UIElement.h"
#include "UIBG.h"

BEGIN(Client)

class CUIHPBar : public CUIElement
{
public:
	enum HP_TYPE { HP_PLAYER, HP_MONSTER, HP_TPYE_END};
	enum HP_COLOR { HP_BLUE, HP_GREEN, HP_RED, HP_COLOR_END};

public:
	typedef struct hp_desc : public UI_DEFAULT_INFO
	{
		HP_TYPE  eHPType;
		HP_COLOR eHPBarColor;
		_float	fMaxHPBarVal;

	} UI_HP_BAR_DESC;


private:
	CUIHPBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIHPBar(const CUIHPBar& Prototype);
	virtual ~CUIHPBar() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	_float					GetHPVal() { return m_fHPVal;  }
	void					SetHPVal(_int iVal);


	void					SetHPValPlayer(_int iVal);

private:
	HP_TYPE					m_eHPType = HP_TYPE::HP_PLAYER;
	HP_COLOR				m_eHPBarColor = HP_COLOR::HP_BLUE;

private:
	_float					m_fMaxHPBarVal = -1;
	_float					m_fHPVal = 0;
	_float					m_fCurHP = -1;
	_float					m_fPrevHP = -1;
	_float					m_fDummy = 0.f;
	_float					m_fCurUpdateRatio = 0;
	_bool					m_bUpdateHP = false;

private:
	_float					m_fExpansionTime = 0.f;
	_bool					m_bOnce = 1;

protected:
	CTexture*				m_pHPBarColor = nullptr;

	CUIElement* m_pVertical = nullptr;

	_float m_fHPAcc = 0.f;
	_bool m_bHPOnce = 1;
	// _bool m_bHPOnce2 = 1;

protected:
	virtual HRESULT			BindShaderResources();
	virtual HRESULT			ReadyComponents(void* pArg);

public:
	static CUIHPBar*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUIHPBar*		Clone(void* pArg);
	virtual void			Free() override;
};

END