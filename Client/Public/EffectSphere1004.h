#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)

class CShader;
class CModel;
class CTexture;

END

BEGIN(Client)

class CEffectSphere1004 final : public CEffect
{
private:
	CEffectSphere1004(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffectSphere1004(const CEffectSphere1004& Prototype);
	virtual ~CEffectSphere1004() = default;

public:
	virtual HRESULT				InitializePrototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				PriorityUpdate(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				LateUpdate(_float fTimeDelta) override;
	virtual HRESULT				Render() override;


private:
	CTexture* m_pNoiseTex = nullptr;
	CTexture* m_pDiffuseTex = nullptr;

private:
	virtual HRESULT				ReadyComponents() override;
	virtual HRESULT				BindShaderResources() override;

public:
	static CEffectSphere1004* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void				Free() override;
};

END