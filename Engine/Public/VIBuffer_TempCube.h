#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_TempCube final : public CVIBuffer
{
private:
	CVIBuffer_TempCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_TempCube(const CVIBuffer_TempCube& Prototype);
	virtual ~CVIBuffer_TempCube() override = default;

public:
	virtual HRESULT						InitializePrototype();
	virtual HRESULT						Initialize(void* pArg) override;

public:
	vector<VTX_NORTEX>&					GetVerticesVector();


private:
	vector<VTX_NORTEX>					m_vecVertices;

public:
	static			CVIBuffer_TempCube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual			CComponent*			Clone(void* pArg) override;
	virtual			void				Free() override;
};

END