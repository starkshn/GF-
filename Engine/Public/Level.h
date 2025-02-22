#pragma once
#include "Base.h"
#include "GameInstance.h"


BEGIN(Engine)


class ENGINE_DLL CLevel : public CBase
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT			Initialize();
	virtual void			Update(const _float& fTimeDelta);
	virtual HRESULT			Render();


public:
	HRESULT					ReadyGaraMaskTexture();

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };

public:
	virtual void			Free() override;
};

END