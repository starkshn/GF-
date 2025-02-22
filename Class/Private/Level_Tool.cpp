#include "Level_Tool.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "TempWater.h"
#include "Transform.h"
#include "Tool_Terrain.h"
#include "Layer.h"
#include "TempCube.h"
#include "TestModel.h"
#include "GameObject.h"

// stages
#include "Tool_Stage_01.h"
#include "Tool_Boss_Stage.h"

// 바닥 충돌체
#include "ColFloor.h"
#include "ColBoundingBox.h"

// 플레이어
#include "Tool_Player.h"

// 애니매이션
#include "Animation.h"

#include "GlobalFuncs.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include <iostream>

#include <DirectXCollision.h>
#include <DirectXMath.h>
#include "DebugDraw.h"

#include <codecvt>
#include <sstream>

#include "Converter.h"

#include "ParticleExplosion.h"
#include "Height.h"
#include "Shadow.h"

CLevel_Tool::CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
	// ReadyGaraMaskTexture();
}

HRESULT CLevel_Tool::Initialize()
{
	// World에 필수 부품들
	{
		if (FAILED(ReadyLights()))
			return E_FAIL;

		if (FAILED(ReadyHeightCamera()))
			return E_FAIL;

		if (FAILED(ReadyShadowCamera()))
			return E_FAIL;
		
		if (FAILED(ReadyLayerMainCamera(TEXT("Layer_Camera"))))
			return E_FAIL;
	}

	// 바닥(베이스) 충돌체
	{
		if (FAILED(ReadyLayerColFloor(TEXT("Layer_ColFloor"))))
			return E_FAIL;
	}

	// 스테이지
	{
		if (FAILED(ReadyLayerStages(TEXT("Layer_Stages"))))
			return E_FAIL;
	}

	// Terrain 
	{
		if (FAILED(ReadyLayerTerrain(TEXT("Layer_Terrain"))))
			return E_FAIL;
	}

	// 오브젝트 
	{
		if (FAILED(ReadyLayerTestModel(TEXT("Layer_TestModel")))) return E_FAIL;
		if (FAILED(ReadyLayerToolPlayer(TEXT("Layer_Tool_Player")))) return E_FAIL;
		// if (FAILED(ReadyLayerDefaultMonster01(TEXT("Layer_DefaultMonster01")))) return E_FAIL;
	}

	// CellLine 디버그용
	if (FAILED(ReadyLayerCellLine(TEXT("Layer_CellLine")))) 
		return E_FAIL;

	if (FAILED(ReadyLayerParticles(TEXT("Layer_Particle"))))
		return E_FAIL;

	if (FAILED(ReadyLayerEffect(TEXT("Layer_Effect"))))
		return E_FAIL;

	if (FAILED(ReadyLayerSkyBox(TEXT("Layer_SkyBox"))))
		return E_FAIL;

	
	return S_OK;
}

void CLevel_Tool::Update(const _float& fTimeDelta)
{
	
}


HRESULT CLevel_Tool::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("Tool 레벨 입니다."));
#endif

	
	return S_OK;
}


HRESULT CLevel_Tool::ReadyLights()
{
	LIGHT_DESC			LightDesc{};

	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(0.7f, 0.7f, 0.7f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->AddLight(LightDesc)))
		return E_FAIL;

	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4(10.f, 3.f, 10.f, 0.f);
	LightDesc.fRange = 10.f;
	LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(m_pGameInstance->AddLight(LightDesc)))
		return E_FAIL;

	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4(20.f, 3.f, 10.f, 0.f);
	LightDesc.fRange = 10.f;
	LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.5f, 0.2f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(m_pGameInstance->AddLight(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tool::ReadyHeightCamera()
{
	_matrix CamWorldMatrix = XMMatrixIdentity();
	CamWorldMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	CamWorldMatrix.r[1] = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	CamWorldMatrix.r[2] = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	CamWorldMatrix.r[3] = XMVectorSet(65.f, 20.f, 65.f, 1.f);

	m_pGameInstance->SetUpHeightTransformMatrix(CHeight::D3DTS_VIEW, XMMatrixInverse(nullptr, CamWorldMatrix));
	m_pGameInstance->SetUpHeightTransformMatrix(CHeight::D3DTS_PROJ, XMMatrixOrthographicLH(200.f, 200.f, 0.f, 100.f));

	return S_OK;
}

HRESULT CLevel_Tool::ReadyShadowCamera()
{
	m_pGameInstance->SetUpShadowTransformMatrix(CShadow::D3DTS_VIEW, XMMatrixLookAtLH(XMVectorSet(-30.f, 50.f, 20.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	m_pGameInstance->SetUpShadowTransformMatrix(CShadow::D3DTS_PROJ, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), static_cast<_float>(g_iWinSizeX) / g_iWinSizeY, 0.1f, 1000.f));

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Test_Camera(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->AddGameObjectToLayer(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_Camera"), _uint(LEVEL_TOOL), strLayerTag, nullptr))) 
		return E_FAIL;
	return S_OK;
}


HRESULT CLevel_Tool::ReadyLayerMainCamera(const _wstring& strLayerTag)
{
	CTool_Main_Camera::TOOL_MAIN_CAM_DESC Desc{};

	Desc.fSpeedPerSec = 30.f;
	Desc.fRotSpeedPerSec = XMConvertToRadians(180.f);
	Desc.fMouseSensor = 0.1f;
	Desc.fFovY = XMConvertToRadians(60.f);
	Desc.fAspectRatio = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.vEye = _float3(0.1f, 2.f, -7.f);
	Desc.vAt = _float3(0.f, 0.f, 0.f);


	if (FAILED(m_pGameInstance->AddGameObjectToLayer(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_Camera"), _uint(LEVEL_TOOL), strLayerTag, &Desc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CLevel_Tool::ReadyLayerTerrain(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->AddGameObjectToLayer(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_Terrain"), _uint(LEVEL_TOOL), strLayerTag, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tool::ReadyLayerColFloor(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->AddGameObjectToLayer(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_ColFloor"), _uint(LEVEL_TOOL), strLayerTag, nullptr)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tool::ReadyLayerTestModel(const _wstring& strLayerTag)
{
	for (size_t i = 0; i < 20; i++)
		if (FAILED(m_pGameInstance->AddGameObjectToLayer(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_TestModel"), _uint(LEVEL_TOOL), strLayerTag, nullptr))) return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tool::ReadyLayerDefaultMonster01(const _wstring& strLayerTag)
{
	//CGameObject::GAMEOBJECT_DESC pDesc;
	//pDesc.strObjectTag = "MONSTER";
	//pDesc.vSpawnPos = _float3{ 0.f, 0.f, -1.f };

	//if (FAILED(m_pGameInstance->AddGameObjectToLayer(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_Monster01"), _uint(LEVEL_TOOL), strLayerTag, (void*)&pDesc))) return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tool::ReadyLayerStages(const _wstring& strLayerTag)
{
	/*if (FAILED(m_pGameInstance->AddGameObjectToLayer(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_Stage01"), _uint(LEVEL_TOOL), strLayerTag, nullptr)))
	return E_FAIL;*/

	// if (FAILED(m_pGameInstance->AddGameObjectToLayer(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_Boss_Stage"), _uint(LEVEL_TOOL), strLayerTag, nullptr)))
	// 	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tool::ReadyLayerToolPlayer(const _wstring& strLayerTag)
{
	CGameObject::GAMEOBJECT_DESC pDesc;
	pDesc.strObjectTag = "PLAYER";
	pDesc.vSpawnPos = _float3{0.f, 0.f, 0.f};
	
	if (FAILED(m_pGameInstance->AddGameObjectToLayer(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_Tool_Player"), _uint(LEVEL_TOOL), strLayerTag, (void*)&pDesc))) return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tool::ReadyLayerCellLine(const _wstring& strLayerTag)
{


	return S_OK;
}

HRESULT CLevel_Tool::ReadyLayerParticles(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_TOOL, TEXT("Prototype_GameObject_Particle_Snow"), LEVEL_TOOL, strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_TOOL, TEXT("Prototype_GameObject_Particle_Explosion"), LEVEL_TOOL, strLayerTag)))
		return E_FAIL;

	// static_cast<CParticleExplosion*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_TOOL, LEVEL_TOOL, TEXT("Layer_Pariticle")));

	return S_OK;
}

HRESULT CLevel_Tool::ReadyLayerEffect(const _wstring& strLayerTag)
{
	/*for (_int i = 0; i < 40; ++i)
	{
		if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_TOOL, TEXT("Prototype_GameObject_Effect"), LEVEL_TOOL, strLayerTag)))
			return E_FAIL;
	}*/

	return S_OK;
}

HRESULT CLevel_Tool::ReadyLayerSkyBox(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->AddGameObjectToLayer(LEVEL_TOOL, TEXT("Prototype_GameObject_SkyBox"), LEVEL_TOOL, strLayerTag)))
		return E_FAIL;

	return S_OK;
}



CLevel_Tool* CLevel_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Tool* pInst = new CLevel_Tool(pDevice, pContext);

	if (FAILED(pInst->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Tool");
		SafeRelease(pInst);
	}

	return pInst;
}

void CLevel_Tool::Free()
{
	__super::Free();
}
