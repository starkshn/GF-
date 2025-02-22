#pragma once

#include "Tool_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CGameInstance;

END

BEGIN(Tool)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT					Initialize();
	void					Update(const _float& fTimeDelta);
	HRESULT					Render();

private:
	HRESULT					SetUp_StartLevel(const LEVELID eLevelID);

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

public:
	static CMainApp*		Create();
	virtual void			Free() override;
};

END