#pragma once
#include "GameObject.h"
#include "Client_Defines.h"


BEGIN(Engine)

class CShader;
class CTexture;
class CVIBuffer_Terrain;
class CNavigation;

END


BEGIN(Client)

class CTerrain final : public CGameObject
{
public:
	enum TEXTURETYPE { TYPE_DIFFUSE, TYPE_MASK, TYPE_BRUSH, TYPE_END };

private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& Prototype);
	virtual ~CTerrain() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom[TYPE_END] = { nullptr, nullptr, nullptr };
	CVIBuffer_Terrain*		m_pVIBufferCom = nullptr;
	CNavigation* m_pNavigationCom = nullptr;
	CNavigation* m_pNavigationComS2 = nullptr;
	CNavigation*			m_pNavigationComS3 = nullptr;

private:
	HRESULT					Ready_Components();
	HRESULT					Bind_ShaderResources();

public:
	static	CTerrain*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual	CGameObject*	Clone(void* pArg) override;
	virtual	void			Free() override;
};

END