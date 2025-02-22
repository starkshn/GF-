#pragma once
#include "Base.h"

BEGIN(Engine)

class CLight;


class CLight_Manager final : public CBase
{
private:
	CLight_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLight_Manager() override = default;

public:
	const LIGHT_DESC*		GetLightDesc(_uint iIdx) const;
	
public:
	HRESULT					Initialize();
	HRESULT					Add_Light(const LIGHT_DESC& LightDesc);

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	list<CLight*>			m_Lights;

public:
	static CLight_Manager*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void			Free() override;
};

END