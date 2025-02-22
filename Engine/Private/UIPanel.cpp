#include "UIPanel.h"

CUIPanel::CUIPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	CUIParent(pDevice, pContext)
{
}

CUIPanel::CUIPanel(const CUIPanel& Prototype)
	:
	CUIParent(Prototype)
{
}

HRESULT CUIPanel::InitializePrototype()
{
	return S_OK;
}

HRESULT CUIPanel::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIPanel::AddChild(CUIElement* pUI, string strName, _float fSX, _float fSY)
{
	if (m_eShowType != pUI->GetShowType())
	{
		MSG_BOX("CUIPanel::AddChild Error! Mismatch ShowType!");
		return E_FAIL;
	}

	if (!pUI)
	{
		MSG_BOX("CUIPanel::AddChild Error! pUI is nullptr!");
		return E_FAIL;
	}

	if (GetChildUI(strName))
	{
		MSG_BOX("CUIPanel::AddChild Error! pUI is exist!");
		return E_FAIL;
	}



	CTransform* pCom = static_cast<CTransform*>(pUI->GetComponent(TEXT("Com_Transform")));
	if (!pCom)
		return E_FAIL;

	// 부모 월드 행렬
	_matrix PanelWorldMatrix = m_pTransformCom->GetWorldMatrix();

	_float fPSX = GetSX();
	_float fPSY = GetSY();

	_float fCSX = fPSX + fSX;
	_float fCSY = fPSY + fSY;

	_uint iNumViewPorts = 1;
	D3D11_VIEWPORT ViewportDesc = {};
	m_pContext->RSGetViewports(&iNumViewPorts, &ViewportDesc);

	pCom->SetTransformState(CTransform::STATE_POS, XMVectorSet(fCSX - ViewportDesc.Width * 0.5f, -fCSY + ViewportDesc.Height * 0.5f, 0.f, 1.f));

	m_ChildUI.insert({ strName, pUI });
	SafeAddRef(pUI);

	return S_OK;
}

CUIElement* CUIPanel::GetChildUI(string strName)
{
	auto iter = m_ChildUI.find(strName);
	if (iter != m_ChildUI.end())
		return iter->second;
	return nullptr;
}

void CUIPanel::SetEleWorldPoses(vector<_vector> vWorldPoses)
{
	_int iCSize = m_ChildUI.size();

	if (vWorldPoses.size() == 0)
	{
		for (_int i = 0; i < iCSize; ++i)
		{
			auto iter = m_ChildUI.begin();
			advance(iter, i);

			if (iter->second->GetObjectTag() == "UIMiniMapPlayerPos" || iter->second->GetObjectTag() == "UIMiniMapBossPos" || iter->second->GetObjectTag() == "UIMiniMapDoor")
				continue;

			iter->second->DeActiveRender();
		}
		return;
	}

	static _int iMaxSize = 0;

	m_WorldPoses = vWorldPoses;
	iMaxSize = m_WorldPoses.size();
	_int iIdx = 0;
	
	for (_int i = 0; i < iCSize; ++i)
	{
		auto iter = m_ChildUI.begin();
		advance(iter, i);

		if (iter->second->GetObjectTag() == "UIMiniMapPlayerPos" || iter->second->GetObjectTag() == "UIMiniMapBossPos" || iter->second->GetObjectTag() == "UIMiniMapDoor")
			continue;

		if (iIdx < iMaxSize)
		{
			static_cast<CTransform*>((iter->second)->GetComponent(TEXT("Com_Transform")))->SetTransformState(CTransform::STATE_POS, m_WorldPoses[iIdx++]);
			iter->second->ActiveRender();
		}
		else
		{
			iter->second->DeActiveRender();
		}
	}

	m_WorldPoses.clear();
}

void CUIPanel::SetBossPos(_fvector vBossPos, _bool bInRange)
{
	CUIParent* pBossMini = nullptr;

	for (auto iter = m_ChildUI.begin(); iter != m_ChildUI.end(); ++iter)
	{
		if (iter->second->GetObjectTag() == "UIMiniMapBossPos")
		{
			pBossMini = iter->second;
			break;
		}
	}

	if (pBossMini)
	{
		if (!bInRange)
		{
			pBossMini->DeActiveRender();

		}
		else
		{
			CTransform* pCom = static_cast<CTransform*>(pBossMini->GetComponent(TEXT("Com_Transform")));
			if (pCom)
			{
				pBossMini->ActiveRender();
				pCom->SetTransformState(CTransform::STATE_POS, vBossPos);
			}
		}
	}

}

void CUIPanel::SetDoorPos(_fvector vBossPos, _bool bInRange)
{
	CUIParent* pDoor = nullptr;

	for (auto iter = m_ChildUI.begin(); iter != m_ChildUI.end(); ++iter)
	{
		if (iter->second->GetObjectTag() == "UIMiniMapDoor")
		{
			pDoor = iter->second;
			break;
		}
	}

	if (pDoor)
	{
		if (!bInRange)
		{
			pDoor->DeActiveRender();
		}
		else
		{
			CTransform* pCom = static_cast<CTransform*>(pDoor->GetComponent(TEXT("Com_Transform")));
			if (pCom)
			{
				pDoor->ActiveRender();
				pCom->SetTransformState(CTransform::STATE_POS, vBossPos);
			}
		}
	}
}

void CUIPanel::Free()
{
	__super::Free();

	for_each(m_ChildUI.begin(), m_ChildUI.end(), [](auto& Pair) 
		{
			if (Pair.second) 
				SafeRelease(Pair.second);
		});

	m_ChildUI.clear();
}
