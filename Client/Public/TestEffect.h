#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CModel;
class CTexture;

END


BEGIN(Client)


class CTestEffect final : public CGameObject
{
private:
	CTestEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestEffect(const CTestEffect& Prototype);
	virtual ~CTestEffect() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	HRESULT					ReadyComponents();
	HRESULT					BindShaderResources();

private:
	_float						m_fMaxLifeTime = 2.f;
	_float						m_fCurLifeTime = 0.f;
	_float						m_fAccLifeTime = 0.f;

private:
	CShader* m_pShaderCom = nullptr;
	CModel* m_pModelCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CTexture* m_pTextureColorCom = nullptr;

public:
	static CTestEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void			Free() override;
};

END