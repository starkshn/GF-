#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class CVIBufferCell final : public CVIBuffer
{
private:
	CVIBufferCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBufferCell(const CVIBufferCell& Prototype);
	virtual ~CVIBufferCell() = default;

public:
	virtual HRESULT			InitializePrototype(const _float3* pPoints);
	virtual HRESULT			Initialize(void* pArg) override;

public:
	static CVIBufferCell*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

END