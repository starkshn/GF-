#include "GameInstance.h"
#include "Level_Loading.h"
#include "Loader.h"
#include "Level_Tool.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{

}

HRESULT CLevel_Loading::Initialize(const LEVELID eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, m_eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Update(const _float& fTimeDelta)
{
	if (true == m_pLoader->IsFinished() && GetKeyState(VK_SPACE) & 0x8000)
	{
		CLevel* pNewLevel = { nullptr };

		switch (m_eNextLevelID)
		{
		case LEVEL_TOOL:
			pNewLevel = CLevel_Tool::Create(m_pDevice, m_pContext);
			break;
		}

		if (nullptr == pNewLevel)
			return;

		if (FAILED(m_pGameInstance->OpenLevel(m_eNextLevelID, pNewLevel)))
			return;
	}
}

HRESULT CLevel_Loading::Render()
{
#ifdef _DEBUG
	m_pLoader->Show_Debug();
#endif

	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID)
{
	CLevel_Loading* pInst = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInst->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		Safe_Release(pInst);
	}

	return pInst;
}

void CLevel_Loading::Free()
{
	__super::Free();

	// CLevel_Loading이 들고 있기 때문에 release반드시 해주어야 한다. 안하면 당연히 leak 발생한다.
	Safe_Release(m_pLoader);
}
