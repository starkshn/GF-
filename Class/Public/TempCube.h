#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"


BEGIN(Engine)

class CTexture;
class CColBoundingBox;

END

BEGIN(Tool)


class CTempCube final : public CGameObject
{
private:
	CTempCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTempCube(const CTempCube& Prototype);
	virtual ~CTempCube() override = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	HRESULT					Ready_Components();

private:
	CColBoundingBox*			m_pBoundingBox = nullptr;
	CTexture*					m_pTextureCom = nullptr;

public:
	static CTempCube*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;

};

END