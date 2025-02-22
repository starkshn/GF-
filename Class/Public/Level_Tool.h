#pragma once
#include "Tool_Defines.h"
#include "Level.h"
#include "Tool_Main_Camera.h"
#include "ColBoundingBox.h"
#include "Model.h"
#include <filesystem>
#include "tinyxml2.h"
#include "Bone.h"
#include "GameObject.h"
#include "CellLine.h"

BEGIN(Tool)

class CTempWater;
class CBase;
class CTool_Terrain;
class CColFloor;

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
	HRESULT					ReadyLights();
	HRESULT					ReadyHeightCamera();
	HRESULT					ReadyShadowCamera();
	HRESULT					Ready_Layer_Test_Camera(const _wstring& strLayerTag);
	HRESULT					ReadyLayerMainCamera(const _wstring& strLayerTag);
	HRESULT					ReadyLayerTempWater(const _wstring& strLayerTag);
	HRESULT					ReadyLayerTerrain(const _wstring& strLayerTag);
	HRESULT					ReadyLayerColFloor(const _wstring& strLayerTag);
	HRESULT					ReadyLayerStages(const _wstring& strLayerTag);
	HRESULT					ReadyLayerTestModel(const _wstring& strLayerTag);
	HRESULT					ReadyLayerDefaultMonster01(const _wstring& strLayerTag);
	HRESULT					ReadyLayerToolPlayer(const _wstring& strLayerTag);
	HRESULT					ReadyLayerCellLine(const _wstring& strLayerTag);
	HRESULT					ReadyLayerParticles(const _wstring& strLayerTag);
	HRESULT					ReadyLayerEffect(const _wstring& strLayerTag);
	HRESULT					ReadyLayerSkyBox(const _wstring& strLayerTag);

public:
	static CLevel_Tool*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void			Free() override;
};

END