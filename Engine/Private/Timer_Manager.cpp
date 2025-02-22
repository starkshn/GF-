#include "..\Public\Timer_Manager.h"
#include "Timer.h"

CTimer_Manager::CTimer_Manager(void)
{

}

_float CTimer_Manager::GetTimeDelta(const _wstring& strTimerTag)
{
	CTimer*		pInstance = FindTimer(strTimerTag);
	if (nullptr == pInstance)
		return 0.f;

	return pInstance->Get_TimeDelta();
}



HRESULT CTimer_Manager::AddTimer(const _wstring& strTimerTag)
{
	if (nullptr != FindTimer(strTimerTag))
		return E_FAIL;

	CTimer* pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	m_mapTimer.emplace(strTimerTag, pTimer);

	return S_OK;
}

void CTimer_Manager::UpdateTimeDelta(const _wstring& strTimerTag)
{
	CTimer*		pInstance = FindTimer(strTimerTag);
	if (nullptr == pInstance)
		return;

	pInstance->Update_Timer();
}


CTimer* CTimer_Manager::FindTimer(const _wstring& strTimerTag)
{
	auto	iter = m_mapTimer.find(strTimerTag);
	
	if (iter == m_mapTimer.end())
		return nullptr;

	return iter->second;
}

CTimer_Manager * CTimer_Manager::Create()
{
	return new CTimer_Manager();	
}

void CTimer_Manager::Free(void)
{
	__super::Free();

	for (auto& Pair : m_mapTimer)
		SafeRelease(Pair.second);
	m_mapTimer.clear();
}
