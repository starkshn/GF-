#pragma once
#include "Engine_Defines.h"
#include "UIParent.h"

BEGIN(Engine)

class ENGINE_DLL CUIElement : public CUIParent
{



protected:
	CUIElement(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIElement(const CUIElement& Prototype);
	virtual ~CUIElement() override = default;

public:
	virtual HRESULT				InitializePrototype() override;
	virtual HRESULT				Initialize(void* pArg) override;

	

public:
	virtual	CUIElement*			Clone(void* pArg) abstract;
	virtual	void				Free() override;
};

END