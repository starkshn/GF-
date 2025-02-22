#pragma once
#include "Base.h"

BEGIN(Engine)

class CGameInstance;

class CHeight final : public CBase
{
public:
	// ºä, Á÷±³
	enum D3D_TRANSFORM_STATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END,};
	
private:
	CHeight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CHeight() = default;

public:
	const _float4x4*		GetHeightTransform(D3D_TRANSFORM_STATE eState) { return &m_TransformMatrices[eState]; }
	void					SetUpTransformMatrix(D3D_TRANSFORM_STATE eState, _fmatrix TransformMatrix)  { XMStoreFloat4x4(&m_TransformMatrices[eState], TransformMatrix); }

public:
	HRESULT					Initialize();
	void					Update();

public:
	_float					ComputeHeight(_fvector vWorldPos);

private:
	_float4x4				m_TransformMatrices[D3DTS_END];
	_uint					m_iWidth{}, m_iHeight{};
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	ID3D11Texture2D*		m_pTexture2D = nullptr;
	CGameInstance*			m_pGameInst = nullptr;
	_float*					m_pHeights = nullptr;

public:
	static CHeight*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void			Free() override;
};

END