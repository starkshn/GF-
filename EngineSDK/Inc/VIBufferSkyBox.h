#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBufferSkyBox final : public CVIBuffer
{
private:
	CVIBufferSkyBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBufferSkyBox(const CVIBufferSkyBox& Prototype);
	virtual ~CVIBufferSkyBox() = default;

public:
	virtual HRESULT				InitializePrototype();
	virtual HRESULT				Initialize(void* pArg);

public:
	static CVIBufferSkyBox*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent*			Clone(void* pArg) override;
	virtual void				Free() override;
	
};

END