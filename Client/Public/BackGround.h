#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)

class CShader;
class CVIBufferRect;
class CTexture;

END


BEGIN(Client)

class CBackGround final : public CUIObject
{
public:
	typedef struct BackGroundDesc : public CUIObject::UI_OBJECT_DESC
	{
		
	} BACKGROUND_DESC;

private:
	CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBackGround(const CBackGround& Prototype);
	virtual ~CBackGround() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	HRESULT					Ready_Components();
	HRESULT					Bind_ShaderResources();	// <- Render에서 하나하나 바인딩 하는거 보기 흉하니까 따로 빼두자 240930(정)

private:
	CShader*				m_pShaderCom = nullptr;
	CVIBufferRect*			m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;


public:
	static CBackGround*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END