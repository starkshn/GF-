#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_TempWater final : public CVIBuffer
{
private:
	CVIBuffer_TempWater(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_TempWater(const CVIBuffer_TempWater& Prototype);
	virtual ~CVIBuffer_TempWater() override = default;

public:
	virtual HRESULT					InitializePrototype() override;
	virtual HRESULT					Initialize(void* pArg) override;

public:
	HRESULT							ResetVtxCnt(_int iCntX, _int iCntZ);
	void							UpdateYPos(_float fDT, _float fSP);

private:
	_uint							m_iCntX, m_iCntZ;
	_bool							m_bUp = true;

public:
	static CVIBuffer_TempWater*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent*				Clone(void* pArg) override;
	virtual void					Free() override;
};

END