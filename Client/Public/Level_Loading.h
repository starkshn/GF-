#pragma once
#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLoader;

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Loading() override = default;

public:
	virtual HRESULT			Initialize(const LEVELID eNextLevelID);
	virtual void			Update(const _float& fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	LEVELID					m_eNextLevelID = LEVEL_END;
	CLoader*				m_pLoader = nullptr;

public:
	static CLevel_Loading*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID);
	virtual void			Free() override;
};

END