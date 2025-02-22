#include "UIManager.h"
#include "GameInstance.h"
#include "Layer.h"

CUIManager::CUIManager()
	:
	m_pGameInst(CGameInstance::GetInstance())
{
	SafeAddRef(m_pGameInst);
}

HRESULT CUIManager::Initialize()
{
	return S_OK;
}

void CUIManager::PriorityUpdate(const _float& fTimeDelta)
{
}

void CUIManager::Update(const _float& fTimeDelta)
{
	CheckDeadUI();
}

void CUIManager::LateUpdate(const _float& fTimeDelta)
{
}

HRESULT CUIManager::AddPanelUI(string strName, CUIPanel* pPanel, UI_SHOW_TYPE eShowType)
{
	if (!pPanel)
		return E_FAIL;

	CUIParent* pPenel = GetPanelUI(strName);
	if (pPenel)
	{
		MSG_BOX("CUIManager::AddPanelUI Error! Exist Panel UI");
		return E_FAIL;
	}

	m_Panels[(_int)eShowType].insert({strName, pPanel});
	SafeAddRef(pPanel);
	return S_OK;
}

CUIPanel* CUIManager::GetPanelUI(string strName)
{
	for (_int i = 0; i < (_int)UI_SHOW_TYPE::UI_END; ++i)
	{
		auto iter = m_Panels[i].find(strName);
		if (iter != m_Panels[i].end())
			return iter->second;
	}
	return nullptr;
}

HRESULT CUIManager::AddChildToPanel(string strPanelName, string strAddUIName, CUIElement* pUI, _float fSX, _float fSY)
{
	CUIPanel* pPanel = nullptr;
	if (pPanel = GetPanelUI(strPanelName))
	{
		if (!pPanel)
		{
			MSG_BOX("CUIManager::AddChildToPanel Error! pPanel is nullptr");
			return E_FAIL;
		}
		pPanel->AddChild(pUI, strAddUIName, fSX, fSY);
	}
	return S_OK;
}

CUIElement* CUIManager::GetChildUIFromPanel(string strPanelName, string strAddUIName)
{
	CUIPanel* pPanel = nullptr;
	if (pPanel = GetPanelUI(strPanelName))
	{
		if (!pPanel)
		{
			MSG_BOX("CUIManager::GetChildUIFromPanel Error! pPanel is nullptr");
			return nullptr;
		}

		return pPanel->GetChildUI(strAddUIName);
	}
	return nullptr;
}

HRESULT CUIManager::AddElementUI(string strName, CUIElement* pUI)
{
	if (GetElementUI(strName))
	{
		MSG_BOX("CUIManager::AddElementUI Error! pUI is Exist");
		return E_FAIL;
	}

	m_Elements.insert({strName, pUI});
	SafeAddRef(pUI);
	return S_OK;
}

CUIElement* CUIManager::GetElementUI(string strName)
{
	auto iter = m_Elements.find(strName);
	if (iter != m_Elements.end())
		return iter->second;
	return nullptr;
}

void CUIManager::CheckDeadUI()
{
	for (_int i = 0; i < (_int)UI_SHOW_TYPE::UI_END; ++i)
	{
		for (auto iter = m_Panels[i].begin(); iter != m_Panels[i].end(); )
		{
			if (CUIParent* pUI = static_cast<CUIParent*>(iter->second))
			{
				if (pUI->GetRemoveFromLayerNextTick())
				{
					_int iCurLevel = m_pGameInst->GetCurrentLevelIndex();

					CUIPanel* pCurRemovePanel = static_cast<CUIPanel*>(pUI);
					map<string, CUIElement*>& ChildContainer = pCurRemovePanel->GetChildUIContainer();

					for (auto citer = ChildContainer.begin(); citer != ChildContainer.end(); )
					{
						if (CUIElement* pChildUI = static_cast<CUIElement*>(citer->second))
						{
							// m_Panels[i].erase(iter->first);
							SafeRelease(pChildUI);
							m_pGameInst->RemoveObject(iCurLevel, pChildUI);
						}

						citer = ChildContainer.erase(citer); // 반복자
					}

					SafeRelease(pUI);
					m_pGameInst->RemoveObject(iCurLevel, pUI);

					iter = m_Panels[i].erase(iter);
				}
				else
				{
					++iter;
				}
			}
			else
			{
				++iter;
			}
		}

	}
}

void CUIManager::AddPlayerUI(CUIParent* pPlayerUI)
{
	m_PlayerUIs.push_back(pPlayerUI);
}

void CUIManager::RenderOffPlayerUI()
{
	for (_int i = 0; i < m_PlayerUIs.size(); ++i)
		m_PlayerUIs[i]->DeActiveRender();
}

void CUIManager::RenderOnPlayerUI()
{
	for (_int i = 0; i < m_PlayerUIs.size(); ++i)
		m_PlayerUIs[i]->ActiveRender();
}

void CUIManager::AddElementVertical(string strName, CUIElement* pUI)
{
	m_ElementsVertical.insert({ strName, pUI });
	// SafeAddRef(pUI);
}

CUIElement* CUIManager::GetElementVertical()
{
	for (auto iter = m_ElementsVertical.begin(); iter != m_ElementsVertical.end(); ++iter)
	{
		if ((iter->second)->IsActiveRender())
			continue;
		else
			return iter->second;
	}
	return nullptr;
}

void CUIManager::SetDamageFont(_int iDamage, _int iColor, _float fSX, _float fSY)
{
	_int iNum = iDamage;
	_int iNums = 0;

	vector<_int> Values;

	// 999
	while (iNum != 0)
	{
		iNum /= 10;
		++iNums;
	}

	_int iTmp = iDamage;

	// 999 / 100 => 9
	// 99 / 10 => 9
	// 9 / 1 => 9
	for (_int i = iNums; i > 0; --i)
	{
		Values.push_back(iTmp / (_int)pow(10, i - 1));
		iTmp %= (_int)pow(10, i - 1);
	}

	vector<CUIElement*> Damages;
	for (auto iter = m_Elements.begin(); iter != m_Elements.end(); ++iter)
	{
		if (iter->second->GetUIType() == UI_TYPE::UI_DAMAGE_FONT && !iter->second->IsActiveRender())
			if (iter->second)
				Damages.push_back(iter->second);

		if (Damages.size() == iNums)
			break;
	}

	// 여기서 전달받은 fSX, fSY와 데미지 크기에 따라 fOffset크기를 잡고 스크린 좌표 설정해야한다.
	// Cal SX, SY
	_float fOffSet = 25.f;
	_float fStartSX = fSX - fOffSet * (iNums / 2);

	for (_int i = 0; i < Damages.size(); ++i)
	{
		_vector vWorldPos = Damages[i]->CalWorldPosFromScreenPos(fStartSX + fOffSet * i, fSY);
		CTransform* pCom = static_cast<CTransform*>(Damages[i]->GetComponent(TEXT("Com_Transform")));
		pCom->SetTransformState(CTransform::STATE_POS, vWorldPos);
		Damages[i]->SetDamageFontNum(Values[i]);
		Damages[i]->SetDamageFontColor(iColor);
		Damages[i]->ActiveRender();
	}
}

void CUIManager::AllOff()
{
	
}


CUIManager* CUIManager::Create()
{
	CUIManager* pInst = new CUIManager{};

	if (FAILED(pInst->Initialize()))
	{
		MSG_BOX("Failed to Created : CUIManager");
		SafeRelease(pInst);
	}

	return pInst;
}

void CUIManager::Free()
{
	__super::Free();	
	SafeRelease(m_pGameInst);

	/*for (_int i = 0; i < (_int)UI_TYPE::UI_END; ++i)
	{
		for_each(m_Panels[i].begin(), m_Panels[i].end(), [](auto& Pair) 
			{
				if (Pair.second)
					SafeRelease(Pair.second);
			});
		
		m_Panels[i].clear();
	}*/

	/*for (auto iter = m_ElementsVertical.begin(); iter != m_ElementsVertical.end(); ++iter)
	{
		SafeRelease(*iter);
	}*/
}
