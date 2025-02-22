#pragma once
#include "Component.h"

BEGIN(Engine)

class CShader;

class ENGINE_DLL CTexture final : public CComponent
{
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& Prototype);
	virtual ~CTexture() override = default;

public:
	virtual HRESULT						InitializePrototype(const _tchar* pTextureFilePath, _uint iNumTexture);
	virtual HRESULT						Initialize(void* pArg) override;
	
public:
	HRESULT								BindShaderResource(CShader* pShader, const _char* pConstantName, _uint iSRVIdx = 0);
	HRESULT								BindShaderResources(CShader* pShader, const _char* pConstantName);

private:
	_uint								m_iNumSRVs = 0;
	vector<ID3D11ShaderResourceView*>	m_SRVs;

public:
	static CTexture*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures = 1);
	virtual CComponent*					Clone(void* pArg) override;
	virtual void						Free() override;
};

END