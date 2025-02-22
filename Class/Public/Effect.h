#pragma once

#include "Tool_Defines.h"
#include "AlphaObject.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CVIBufferRect;

END


BEGIN(Tool)

class CEffect final : public CAlphaObject
{
private:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect(const CEffect& Prototype);
	virtual ~CEffect() = default;

public:
	virtual HRESULT				InitializePrototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				PriorityUpdate(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				LateUpdate(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

private:
	CShader*					m_pShaderCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CVIBufferRect*				m_pVIBufferCom = nullptr;

private:
	_float						m_fFrame = 0.f;

private:
	HRESULT						ReadyComponents();
	HRESULT						BindShaderResources();

public:
	static CEffect*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END