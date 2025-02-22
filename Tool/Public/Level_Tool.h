#pragma once
#include "Tool_Defines.h"
#include "Level.h"

BEGIN(Tool)

class CLevel_Tool final : public CLevel
{
private:
	CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Tool() = default;

public:
	virtual HRESULT			Initialize() override;
	virtual void			Update(const _float& fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	HRESULT					Ready_Layer_Test_Camera(const _wstring& strLayerTag);
	HRESULT					ReadyLayerMainCamera(const _wstring& strLayerTag);

	HRESULT					ReadyLayerTempWater(const _wstring& strLayerTag);
	HRESULT					ReadyLayerTerrain(const _wstring& strLayerTag);

private:
	void					ResetVtxCnt(_int iCntX, _int iCntZ);


private:
	void					Initialize_IMGUI();

public:
	static CLevel_Tool*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void			Free() override;
};

END