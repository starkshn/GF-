#pragma once

#include "Base.h"

BEGIN(Engine)

class CTimer_Manager final : public CBase 
{
private:
	CTimer_Manager(void);
	virtual ~CTimer_Manager(void) = default;

public:
	_float			GetTimeDelta(const _wstring& strTimerTag);
	
public:
	HRESULT			AddTimer(const _wstring& strTimerTag);
	void			UpdateTimeDelta(const _wstring& strTimerTag);


private:
	map<const _wstring, class CTimer*>			m_mapTimer;

private:
	class CTimer*		FindTimer(const _wstring& strTimerTag);


public:
	static CTimer_Manager* Create();
	virtual void Free(void) override;
};

END
