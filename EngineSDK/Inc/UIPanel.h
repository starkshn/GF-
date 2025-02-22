#pragma once
#include "Engine_Defines.h"
#include "UIParent.h"
#include "UIPanel.h"
#include "UIElement.h"

BEGIN(Engine)

class ENGINE_DLL CUIPanel : public CUIParent
{
protected:
	CUIPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPanel(const CUIPanel& Prototype);
	virtual ~CUIPanel() override = default;

public:
	virtual HRESULT				InitializePrototype() override;
	virtual HRESULT				Initialize(void* pArg) override;

public:
	HRESULT						AddChild(CUIElement* pUI, string strName, _float fSX, _float fSY);
	CUIElement*					GetChildUI(string strName);

public:
	map<string, CUIElement*>&	GetChildUIContainer() { return m_ChildUI; }

public:
	void						SetEleWorldPoses(vector<_vector> vWorldPoses);
	void						SetTypeMiniMap() { m_bMiniMap = true; }
	void						SetBossPos(_fvector vBossPos, _bool bInRange);
	void						SetDoorPos(_fvector vBossPos, _bool bInRange);

protected:
	map<string, CUIElement*>	m_ChildUI;

	
private:
	vector<_vector>				m_WorldPoses;
	_bool						m_bMiniMap = false;

public:
	virtual	CUIParent*			Clone(void* pArg) abstract;
	virtual	void				Free() override;
};

END