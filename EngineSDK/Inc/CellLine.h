#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CVIBufferLine;

// 디버그용 Draw 컴포넌트 객체

class ENGINE_DLL CCellLine final : public CGameObject
{
private:
	CCellLine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCellLine(const CCellLine& Prototype);
	virtual ~CCellLine() = default;

public:
	virtual HRESULT		InitializePrototype();
	virtual HRESULT		Initialize(void* pArg);

public:
	// Call-By-Value로 월드 행렬을 매틱 받아온다.
	virtual void		Update(_fmatrix vWorld);

	void				UpdateLine(const _fvector& vStart, const _fvector& vEnd);

	void				SetStartEndLinePos(_fvector vStart, _fvector vEnd);

#ifdef _DEBUG

	virtual HRESULT		Render();

#endif

private:
	CShader*			m_pShader = nullptr;
	CVIBufferLine*		m_pVIBuffer = nullptr;

	_float3				m_vStart;
	_float3				m_vEnd;

private:
	static _float4x4	m_WorldMatrix;

public:
	static CCellLine*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void		Free() override;
};

END