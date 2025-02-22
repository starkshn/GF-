#pragma once
#include "Base.h"
#include "GameInstance.h"
#include "UIObject.h"
#include "UIParent.h"
#include "UIPanel.h"

BEGIN(Engine)

class ENGINE_DLL CUIManager : public CBase
{
public:
	CUIManager();
	virtual ~CUIManager() override = default;

public:
	HRESULT						Initialize();
	void						PriorityUpdate(const _float& fTimeDelta);
	void						Update(const _float& fTimeDelta);
	void						LateUpdate(const _float& fTimeDelta);

public:
	// Panel 추가할 때 사용
	HRESULT						AddPanelUI(string strName, CUIPanel* pPanel, UI_SHOW_TYPE eShowType);
	CUIPanel*					GetPanelUI(string strName);

	// Panel의 자식으로 추가할 때 사용, UI 생성해서 추가한다.
	HRESULT						AddChildToPanel(string strPanelName, string strAddUIName, CUIElement* pUI, _float fSX, _float fSY);
	CUIElement*					GetChildUIFromPanel(string strPanelName, string strAddUIName);

	HRESULT						AddElementUI(string strName, CUIElement* pUI);
	CUIElement*					GetElementUI(string strName);

public:
	void						CheckDeadUI();


public:
	void						AddPlayerUI(CUIParent* pPlayerUI);
	void						RenderOffPlayerUI();
	void						RenderOnPlayerUI();

	
public:
	void						AddElementVertical(string strName, CUIElement* pUI);
	CUIElement*					 GetElementVertical();


public:
	// 0 : Default, 1 : Yellow
	void						SetDamageFont(_int iDamage, _int iColor, _float fSX, _float fSY);


public:
	void						AllOff();

private:
	CGameInstance*				m_pGameInst = nullptr;
	map<string, CUIPanel*>		m_Panels[(_int)UI_SHOW_TYPE::UI_END];
	map<string, CUIElement*>	m_Elements;


	map<string, CUIElement*>	m_ElementsVertical;


private:
	vector<CUIParent*>			m_PlayerUIs;


public:
	static CUIManager*			Create();
	virtual void				Free() override;
};

END