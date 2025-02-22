#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
private:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader(const CShader& Prototype);
	virtual ~CShader() override = default;

public:
	virtual HRESULT				InitializePrototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);	 
	virtual HRESULT				Initialize(void* pArg) override;

public:
	HRESULT						Begin(_uint iPassIdx);
	HRESULT						BindRawValue(const _char* pConstantName, const void* pData, _uint iLenth); // 상수 이름, pData(아무 데이터나 받겠다), 데이터의 길이 (241080)
	HRESULT						BindMatrix(const _char* pConstantName, const _float4x4* pMatrix);
	HRESULT						BindMatrices(const _char* pConstantName, const _float4x4* pMatrix, _uint iNumMatrices);
	HRESULT						BindSRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV);
	HRESULT						BindSRVs(const _char* pConstantName, ID3D11ShaderResourceView** ppSRV, _uint NumSRVs);

	


public:
// Temp
	HRESULT						Bind_Val(const _char* pConstantName, const _float fVal);
	HRESULT						Bind_Val_Test(const _char* pConstantName, _fvector vDir);

private:
	ID3DX11Effect*				m_pEffect = nullptr;
	_uint						m_iNumLayouts = 0;				// pass의 개수와 같다 생각해도 된다.
	vector<ID3D11InputLayout*>	m_InputLayouts;

public:
	static CShader*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual CComponent*			Clone(void* pArg) override;
	virtual void				Free() override;
};

END