#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBufferRect final : public CVIBuffer
{
private:
	CVIBufferRect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBufferRect(const CVIBufferRect& Prototype);
	virtual ~CVIBufferRect() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

public:
	static CVIBufferRect*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

END