#pragma once
#include "Engine_Defines.h"
#include "UIObject.h"
#include "VIBufferRect.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
#include "GameObject.h"

BEGIN(Engine)

class CGameInstance;

class ENGINE_DLL CUIParent abstract : public CUIObject
{
public:
	typedef struct ui_parent_desc : public CUIObject::UI_OBJECT_DESC
	{
		UI_SHOW_TYPE eShowType;
		UI_TYPE eUIType;
		_uint iDepth;
		CGameObject* pUIOwner;

		CUIParent* pPerspectiveUIOwner;


	} UI_DEFAULT_INFO;

protected:
	CUIParent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIParent(const CUIParent& Prototype);
	virtual ~CUIParent() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

private:
	HRESULT					Init(UI_SHOW_TYPE eShowType, UI_TYPE eType, UI_DEFAULT_INFO Desc);
	HRESULT					InitializeOrthoMatrix(UI_DEFAULT_INFO Desc);
	HRESULT					InitializePerpectiveMatrix(UI_DEFAULT_INFO Desc);

public:
	void					UpdatePerspectiveMatrix();
	_vector					CalWorldPosFromOwnerScreenPos(_float2 vOwnerUIScreenPos);
	_vector					CalWorldPosFromScreenPos(_float fSX, _float fSY);

	_vector					CalScreenPosFromWorldPos(_fvector vWorldPos);

public:
	UI_SHOW_TYPE			GetShowType() { return m_eShowType; }
	UI_TYPE					GetUIType() { return m_eUIType; }

	_float					GetSX() { return m_fSX; }
	_float					GetSY() { return m_fSY; }

	_float					GetSizeX() { return m_fSSizeX; }
	_float					GetSizeY() { return m_fSSizeY; }

public:
	_bool					IsActiveRender() { return m_bRenderAvtive; }
	void					DeActiveRender() { m_bRenderAvtive = false; }
	void					ActiveRender() { m_bRenderAvtive = true; }

public:
	void					SetUIOwner(CGameObject* pOwner) 
	{ 
		if (pOwner)
		{
			m_pUIOwner = pOwner;
			// SafeAddRef(m_pUIOwner);
		}
	}
	CGameObject*			GetUIOwner() { return m_pUIOwner; }

	CUIParent*				GetPerspectiveUIOwner() 
	{ 
		if (m_eShowType == UI_SHOW_TYPE::UI_PERSPECTIVE)
			return m_pPerpectiveUIOwner;
		else
			return nullptr;
	}

public:
	_bool					GetPerpectiveUIOwnerIsInFrustum();

public:
	_float2					GetPerspectiveScreenPos()
	{
		if (m_eShowType == UI_SHOW_TYPE::UI_PERSPECTIVE)
			return m_vPerpectiveScreenPos;
		else
			return _float2{0.f, 0.f};
	}

	_bool					CheckPerspectiveCondition();


public:
	_bool					CanRender() { return m_bDoRender; }
	void					SetDoNotRender() { m_bDoRender = false; }
	void					SetDoRender() { m_bDoRender = true; }


public:

public:
	void					SetDamageFontNum(_int iNum) { m_iDamageFontNum = iNum; }
	_int					GetDamageFontNum() { return m_iDamageFontNum; }
	void					SetDamageFontColor(_int iColor) { m_iDamageFontColor = iColor; }
	_int					GetDamageFontColor() { return m_iDamageFontColor; }

	
public:
	// 병신임 UI는 z가 0이라 플레이어 위치와 내적할 수 없다.
	_bool					IsInCamera(_uint iTargetLevel);

protected:
	UI_SHOW_TYPE			m_eShowType = UI_SHOW_TYPE::UI_END;
	UI_TYPE					m_eUIType = UI_TYPE::UI_END;
	_bool					m_bRenderAvtive = true;

protected:
	CGameObject*			m_pUIOwner = nullptr;
	CUIParent*				m_pPerpectiveUIOwner = nullptr;

private:
	_float2					m_vPerpectiveScreenPos = {};

private:
	_bool					m_bDoRender = false;

private:
	_int					m_iDamageFontNum = 0;
	_int					m_iDamageFontColor = 0;

protected:
	CVIBufferRect*			m_pVIBufferCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

public:
	virtual	CUIParent*		Clone(void* pArg) abstract;
	virtual	void			Free() override;
	
};

END