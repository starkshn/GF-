#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class CQuadTree;

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
private:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype);
	virtual ~CVIBuffer_Terrain() override = default;

public:
	virtual HRESULT						InitializePrototype(const _tchar* pHeightMapFilePath);
	virtual HRESULT						Initialize(void* pArg) override;

	void								Culling(_fmatrix WorldMatrix);


public:
	_uint								GetVerticesX() { return m_iNumVerticesX; }
	_uint								GetVerticesZ() { return m_iNumVerticesZ; }

	const vector<VTX_NORTEX>&			GetVerticesVector();

private:
	_uint								m_iNumVerticesX = 0;
	_uint								m_iNumVerticesZ = 0;

	vector<VTX_NORTEX>					m_vecVertices;

	CQuadTree*							m_pQuadTree = nullptr;

public:
	static			CVIBuffer_Terrain*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath);
	virtual			CComponent*			Clone(void* pArg) override;
	virtual			void				Free() override;
};

END