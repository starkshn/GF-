#pragma once

#include "Client_Defines.h"
#include "AlphaObject.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CVIBufferRect;
class CModel;

END


BEGIN(Client)

class CEffect abstract : public CAlphaObject
{
public:
	typedef struct effect_desc : public CGameObject::GAMEOBJECT_DESC
	{
		_bool bUseFrameEffect;
		_float fMaxLifeTime;
	} EFFECT_DESC;

protected:
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

public:
	_bool						IsActive() { return m_bActive; }
	void						Active() { m_bActive = true; }
	void						DeActive() { m_bActive = false; }
	void						ForceDeActive();

	void						Move() { m_bMove = true; }
	void						StopMove() { m_bMove = false; }
	void						SetMoveDir(_fvector vDir) { m_vMoveDir = vDir; }

public:
	void						SetOwner(CGameObject* pOwner) { m_pOwner = pOwner; }
	CGameObject*				GetOwner() { return m_pOwner; }

protected:
	void						BillBoard();

protected:
	_float						m_fFrame = 0.f;
	_bool						m_bUseFrameEffect = false;
	
	_float						m_fMaxLifeTime = 0.f;
	_float						m_fCurLifeTime = 0.f;
	_float						m_fAccLifeTime = 0.f;
	_bool						m_bActive = false;

protected:
	_bool						m_bMove = false;
	_vector						m_vMoveDir;

	CGameObject*				m_pOwner = nullptr;

protected:
	_float	m_fMaxScale = 16.f;
	_float	m_fAccScale = 0.f;
	_float	m_fSP = 50.f;

protected:
	CShader*					m_pShaderCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CVIBufferRect*				m_pVIBufferCom = nullptr;
	CModel*						m_pModelCom = nullptr;

	CTexture*					m_pTextureDiffuseCom = nullptr;
	CTexture*					m_pTextureNoiseCom = nullptr;



private:
	virtual HRESULT				ReadyComponents() abstract;
	virtual HRESULT				BindShaderResources() abstract;

public:
	virtual void				Free() override;
};

END