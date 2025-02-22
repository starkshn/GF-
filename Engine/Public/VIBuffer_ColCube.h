#pragma once
#include "VIBuffer.h"


BEGIN(Engine)

class ENGINE_DLL CVIBuffer_ColCube final : public CVIBuffer
{
private:
	CVIBuffer_ColCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_ColCube(const CVIBuffer_ColCube& Prototype);
	virtual ~CVIBuffer_ColCube() override = default;

public:
	virtual HRESULT						InitializePrototype() override;
	virtual HRESULT						Initialize(void* pArg) override;

public:
	static			CVIBuffer_ColCube*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual			CComponent*			Clone(void* pArg) override;
	virtual			void				Free() override;
};

END