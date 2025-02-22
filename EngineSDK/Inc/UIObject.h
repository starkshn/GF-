#pragma once
#include "GameObject.h"

// 클라에서 만든 UI용 객체들의 부모가 되는 클래스이다.
// 직교 투영을 위한 데이터와 기능을 가지고 있다.

BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
public:
	typedef struct ui_desc : CGameObject::GAMEOBJECT_DESC
	{
		_float fSX, fSY, fSSizeX, fSSizeY;
	} UI_OBJECT_DESC;

protected:
	CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIObject(const CUIObject& Prototype);
	virtual ~CUIObject() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

public:
	_uint					GetDepth() { return m_iDepth; }
	void					SetDepth(_uint iDepth) { m_iDepth = iDepth; }

protected:
	_float					m_fSX, m_fSY, m_fSSizeX, m_fSSizeY;
	_float4x4				m_ViewMatrix{}, m_ProjMatrix{};
	_uint					m_iDepth = 0;

public:
	virtual	CGameObject*	Clone(void* pArg) abstract;
	virtual	void			Free() override;
};

END