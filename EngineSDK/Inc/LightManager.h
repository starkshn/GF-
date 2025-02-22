#pragma once
#include "Base.h"

BEGIN(Engine)

class CLight;
class CShader;
class CVIBufferRect;


class CLightManager final : public CBase
{
private:
	CLightManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLightManager() override = default;

public:
	const LIGHT_DESC* GetLightDesc(_uint iIdx) const;

public:
	HRESULT					Initialize();
	HRESULT					AddLight(const LIGHT_DESC& LightDesc);
	HRESULT					Render(CShader* pShader, CVIBufferRect* pVIBuffer);

public:
	_bool					IsActiveLight(const string& strLightName);
	void					ActiveLight(const string& strLightName);
	void					DeActiveLight(const string& strLightName);

	void					UpdateLightPos(const string& strLightName, _fvector vWorldPos);
	void					UpdatePointLightRange(const string& strLightName, _float fRange);

private:
	CLight*					FindLight(const string& strLightName);
	
public:
	void					Clear(_uint iLevelID);

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	list<CLight*>			m_Lights;

public:
	static CLightManager*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void			Free() override;
};

END