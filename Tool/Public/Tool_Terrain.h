#pragma once
#include "GameObject.h"
#include "Tool_Defines.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CVIBuffer_Terrain;

END

BEGIN(Tool)

class CTool_Terrain final : public CGameObject
{
private:
	CTool_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTool_Terrain(const CTool_Terrain& Prototype);
	virtual ~CTool_Terrain() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CVIBuffer_Terrain*		m_pVIBufferCom = nullptr;

private:
	HRESULT					ReadyComponents();
	HRESULT					BindShaderResources();

public:
	_bool					Picking(_int iScreenX, _int iScreenY, _float4& vPickPos, _float& fDistance);

public:
	static	CTool_Terrain*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual	CGameObject*	Clone(void* pArg) override;
	virtual	void			Free() override;
};

END