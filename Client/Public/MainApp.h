#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT					Initialize();
	void					Update(_float fTimeDelta);
	HRESULT					Render();

private:
	HRESULT					SetUpStartLevel(const LEVELID eLevelID);

private:
	HRESULT					ReadyLoadUI();

private:
	HRESULT					ReadyGara();

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

#ifdef _DEBUG
private:
	_float						m_fTimeAcc = {};
	_tchar						m_szFPS[MAX_PATH] = {};
	_uint						m_iNumDraw = {};

#endif

	CGameObject*				m_pBack = nullptr;

public:
	static CMainApp*		Create();
	virtual void			Free() override;
};

END