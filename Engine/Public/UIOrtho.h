#pragma once
#include "Engine_Defines.h"
#include "UIParent.h"

BEGIN(Engine)

class ENGINE_DLL CUIOrtho abstract : public CUIParent
{
protected:
	CUIOrtho(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIOrtho(const CUIOrtho& Prototype);
	virtual ~CUIOrtho() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

public:
	virtual	CUIOrtho*		Clone(void* pArg) abstract;
	virtual	void			Free() override;
};

END