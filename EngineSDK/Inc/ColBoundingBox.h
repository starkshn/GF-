#pragma once
#include "Component.h"


BEGIN(Engine)

class CShader;
class CVIBuffer_ColCube;
class CTransform;

class ENGINE_DLL CColBoundingBox : public CComponent
{
private:
	CColBoundingBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CColBoundingBox(const CColBoundingBox& Prototype);
	virtual ~CColBoundingBox() override = default;


public:
	virtual HRESULT						InitializePrototype(CBase* pShader, CBase* pVIBuffer);
	virtual HRESULT						Initialize(void* pArg) override;

public:
	void								Update_Bouding_Box(CTransform* pTransform);
	HRESULT								Render_Bounding_Box(CTransform* pTransform, _matrix viewMat, _matrix projMat);

public:
	_bool								Check_Picking(_fvector vRayOrigin, _fvector vRayDir, _float& fDist, _float4& vPickingPos);
	_bool								IsUsePicking() { return m_bUsePicking; }
	void								SetUsePicking(_bool bUse) { m_bUsePicking = bUse; }

	void								SetCurrentPick(_bool bPicked) { m_bCurPicked = bPicked; }

private:
	CShader*							m_pShaderCom = nullptr;
	CVIBuffer_ColCube*					m_pVIBufferCom = nullptr;
	ID3D11RasterizerState*				m_pDefaultRasterizerState = nullptr;

private:
	DirectX::BoundingBox				m_BoundingBox;

private:
	_bool								m_bUsePicking = true;
	_bool								m_bCurPicked = false;


public:
	static CColBoundingBox*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBase* pShader, CBase* pVIBuffer);
	virtual CComponent*					Clone(void* pArg) override;
	virtual void						Free() override;
};

END