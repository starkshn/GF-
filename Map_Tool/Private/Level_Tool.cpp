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

#include "SpawnTrigger.h"
#include "Spawner.h"

static _float g_fRotationY = 0.0f; // Y축 회전 각도


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
		if (FAILED(ReadyLayerDefaultMonster01(TEXT("Layer_DefaultMonster01")))) return E_FAIL;
	}


	// 공부용
	{
		// if (FAILED(Ready_Layer_TempWater(TEXT("Layer_TempWater")))) return E_FAIL;
		// if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain")))) return E_FAIL;
	}

	// CellLine 디버그용
	{
		if (FAILED(ReadyLayerCellLine(TEXT("Layer_CellLine")))) 
			return E_FAIL;
	}

	if (FAILED(ReadyLayerParticles(TEXT("Layer_Particle"))))
		return E_FAIL;

	m_pColFloor = dynamic_cast<CColFloor*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_TOOL, LEVEL_TOOL, TEXT("Layer_ColFloor")));
	m_pColFloorTransform = dynamic_cast<CTransform*>(m_pColFloor->GetComponent(TEXT("Com_Transform")));
	m_pColFloorBoundingBox = dynamic_cast<CColBoundingBox*>(m_pColFloor->GetComponent(TEXT("Com_BoundingBox")));
	
	if (nullptr == m_pColFloor || nullptr == m_pColFloorTransform)
		return E_FAIL;

	SafeAddRef(m_pColFloor);
	SafeAddRef(m_pColFloorBoundingBox);
	m_pColFloorBoundingBox->SetUsePicking(true);
	
	Initialize_IMGUI();

	// 콘솔 창 할당
	// AllocConsole();
	// 
	// // 콘솔 출력을 위한 스트림 연결
	// FILE* stream;
	// freopen_s(&stream, "CONOUT$", "w", stdout);  // 표준 출력을 콘솔로 리디렉션
	// freopen_s(&stream, "CONOUT$", "w", stderr);  // 표준 에러 출력을 콘솔로 리디렉션


	// 몬스터들 뼈 구조 XML 파싱
	{
		CConverter::GetInstance()->ExportAssetBonesToXML("../Bin/resources/Models/GF_Monsters/Monster01/Monster01.fbx", "../Bin/resources/Models/GF_Monsters/Monster01/Monster01.xml", CModel::TYPE_ANIM);
		CConverter::GetInstance()->ExportAssetBonesToXML("../Bin/resources/Models/GF_Monsters/Monster02/Monster02.fbx", "../Bin/resources/Models/GF_Monsters/Monster02/Monster02.xml", CModel::TYPE_ANIM);
		m_strMonster01 = ReadBoneHierachyFromXML("../Bin/resources/Models/GF_Monsters/Monster01/Monster01.xml", m_RootBoneMonster01);
		m_strMonster02 = ReadBoneHierachyFromXML("../Bin/resources/Models/GF_Monsters/Monster02/Monster02.xml", m_RootBoneMonster02);
	}


	return S_OK;
}

void CLevel_Tool::Update(const _float& fTimeDelta)
{
	float mouseX, mouseY;
	mouseX = ImGui::GetIO().MousePos.x;
	mouseY = ImGui::GetIO().MousePos.y;

	if (!ImGui::GetIO().WantCaptureMouse)
	{
		// 피킹을 사용중이고 RB 버튼이 눌렸을 경우
		if (m_bUsePicking)
		{
			// 마우스 스크린 좌표
			POINT ptMousePos;
			GetCursorPos(&ptMousePos);
			ScreenToClient(g_hWnd, &ptMousePos);

			_float4					vPickedPos{ 0.f, 0.f, 0.f, 0.f };
			_float					fDist = 0.f;

			if (m_bPickNavMesh)
			{
				// 마우스 스크린 좌표
				POINT ptMousePos;
				GetCursorPos(&ptMousePos);
				ScreenToClient(g_hWnd, &ptMousePos);

				_float4					vPickedPos{ 0.f, 0.f, 0.f, 0.f };
				_float					fDist = 0.f;

				if (m_bPickNavMesh)
					PickCellLines();
				else
				{
					if (m_pGameInstance->MouseDown(MOUSE_KEY::RB))
						Picking(m_ePickingType, m_eSpawnType, ptMousePos.x, ptMousePos.y, vPickedPos, fDist);
				}
			}
			else
			{
				if (m_pGameInstance->MouseDown(MOUSE_KEY::RB))
					Picking(m_ePickingType, m_eSpawnType, ptMousePos.x, ptMousePos.y, vPickedPos, fDist);
			}
		}
		
	}

	// Get Picking Object Pos
	{
		if (nullptr != m_pCurrent_Picked_Object && nullptr != m_pCurrent_Picked_Object_Transform)
		{
			XMStoreFloat4(&m_vCurrent_Picked_Object_Pos, m_pCurrent_Picked_Object_Transform->GetTransformState(CTransform::STATE_POS));
		}
	}

	if (m_bFollowCameraPos)
	{
		m_pCurrent_Picked_Object_Transform->SetTransformState(CTransform::STATE_POS, XMLoadFloat4(m_pGameInstance->GetCameraPosition()));
		m_pCurrent_Picked_Object_Transform->SetTransformState(CTransform::STATE_LOOK, XMLoadFloat4(m_pGameInstance->GetCameraState(Engine::CAMERA_STATE::LOOK)));
		m_pCurrent_Picked_Object_Transform->SetTransformState(CTransform::STATE_UP, XMLoadFloat4(m_pGameInstance->GetCameraState(Engine::CAMERA_STATE::UP)));
		m_pCurrent_Picked_Object_Transform->SetTransformState(CTransform::STATE_RIGHT, XMLoadFloat4(m_pGameInstance->GetCameraState(Engine::CAMERA_STATE::RIGHT)));
	}


	for (_int i = 0; i < m_CellLines.size(); ++i)
	{
		m_CellLines[i]->Update(XMMatrixIdentity());
		m_pGameInstance->AddRenderObject(CRenderer::RG_PRIORITY, m_CellLines[i]);
	}
}


HRESULT CLevel_Tool::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("Tool 레벨 입니다."));
#endif

	Render_IMGUI();

	RenderCellLine();

	return S_OK;
}

HRESULT CLevel_Tool::ReadyLayerTempWater(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->AddGameObjectToLayer(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_TempWater"), _uint(LEVEL_TOOL), strLayerTag, nullptr))) 
		return E_FAIL;

	m_pObj = m_pGameInstance->GetGameObjctInLayerBack(LEVEL_TOOL, LEVEL_TOOL, TEXT("Layer_TempWater"));

	if (m_pObj != nullptr)
	{
		m_pTempWater = dynamic_cast<CTempWater*>(m_pObj);
	}

	return S_OK;
}

string CLevel_Tool::ReadBoneHierachyFromXML(const _char* pFilePath, BONE& pRootBone)
{	
	// 상대 경로를 절대 경로로 변환
	std::filesystem::path relativePath(pFilePath);
	std::filesystem::path absolutePath = std::filesystem::absolute(relativePath);

	std::filesystem::path pFilePathNow = pFilePath;

	// 1. 파일 이름만 추출: "1201_Right_Bones.xml"
	std::string fileName = pFilePathNow.filename().string();

	// 2. 확장자 제거: "1201_Right_Bones"
	std::string stem = pFilePathNow.stem().string();
	
	std::string result;
	// 3. 언더바 위치 찾기
	size_t pos = stem.find('_');
	if (pos != std::string::npos) 
	{
		// 언더바 앞부분만 추출: "1201_Right"
		result = stem.substr(0, pos);
		std::cout << "Parsed string: " << result << std::endl;
	}
	else 
	{
		std::cout << "No underscore found." << std::endl;
	}


	tinyxml2::XMLDocument pDoc;
	if (pDoc.LoadFile(absolutePath.string().c_str()) != tinyxml2::XML_SUCCESS)
		return "";

	string rootBoneName = "Bone" + result;

	tinyxml2::XMLElement* pRoot = pDoc.FirstChildElement("Bones");
	if (!pRoot)
		return "";

	tinyxml2::XMLElement* pRootBoneEle = pRoot->FirstChildElement("Bone");
	if (!pRootBoneEle)
		return "";

	ParseBoneElement(pRootBoneEle, pRootBone);

	return rootBoneName;
}

void CLevel_Tool::ParseBoneElement(tinyxml2::XMLElement* pElement, BONE& parentBone)
{
	const _char* pBoneName = pElement->Attribute("Name");
	if (pBoneName)
	{
		parentBone.strName = pBoneName;
	}

	tinyxml2::XMLElement* pChildElement = pElement->FirstChildElement("Bone");
	while (pChildElement)
	{
		BONE ChildBone;

		ParseBoneElement(pChildElement, ChildBone);

		parentBone.Child.push_back(ChildBone);

		pChildElement = pChildElement->NextSiblingElement("Bone");
	}
}

HRESULT CLevel_Tool::ReadyLights()
{
	LIGHT_DESC	LightDesc{};

	// Terrain Light Desc
	{
		ZeroMemory(&LightDesc, sizeof LightDesc);

		LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
		LightDesc.vDirection = _float4{ -1.f, -1.f, -1.f, 0.f };
		LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
		LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
		LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

		if (FAILED(m_pGameInstance->AddLight(LightDesc)))
			return E_FAIL;
	}
	
	// TempWater Light Desc
	{
		ZeroMemory(&LightDesc, sizeof LightDesc);

		LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
		LightDesc.vDirection = _float4{ 0.f, -1.f, 0.f, 0.f };
		LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
		LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
		LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

		if (FAILED(m_pGameInstance->AddLight(LightDesc)))
			return E_FAIL;
	}


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
	// if (FAILED(m_pGameInstance->AddGameObjectToLayer(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_Terrain"), _uint(LEVEL_TOOL), strLayerTag, nullptr)))
	// 	return E_FAIL;
	// 
	// m_pObj = m_pGameInstance->GetGameObjctInLayerBack(LEVEL_TOOL, TEXT("Prototype_GameObject_Terrain"), LEVEL_TOOL, TEXT("Layer_Terrain"));
	// 
	// if (m_pObj != nullptr)
	// {
	// 	m_pTerrain = dynamic_cast<CTool_Terrain*>(m_pObj);
	// }

	return S_OK;
}

HRESULT CLevel_Tool::ReadyLayerColFloor(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->AddGameObjectToLayer(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_ColFloor"), _uint(LEVEL_TOOL), strLayerTag, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tool::ReadyLayerTestModel(const _wstring& strLayerTag)
{
	// for (size_t i = 0; i < 20; i++)
	// 	if (FAILED(m_pGameInstance->AddGameObjectToLayer(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_TestModel"), _uint(LEVEL_TOOL), strLayerTag, nullptr))) return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tool::ReadyLayerDefaultMonster01(const _wstring& strLayerTag)
{
	//CGameObject::GAMEOBJECT_DESC pDesc;
	//pDesc.strObjectTag = "MONSTER";
	//pDesc.vSpawnPos = _float3{ 0.f, 0.f, -1.f };

	//if (FAILED(m_pGameInstance->AddGameObjectToLayer(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_Monster01"), _uint(LEVEL_TOOL), strLayerTag, (void*)&pDesc))) return E_FAIL;

	//if (FAILED(m_pGameInstance->AddGameObjectToLayer(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_Boss"), _uint(LEVEL_TOOL), strLayerTag, nullptr))) return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tool::ReadyLayerStages(const _wstring& strLayerTag)
{
	// 스테이지 LEVEL_TOOL에 추가
	{
		if (FAILED(m_pGameInstance->AddGameObjectToLayer(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_Stage01"), _uint(LEVEL_TOOL), strLayerTag, nullptr)))
			return E_FAIL;

		CTool_Stage_01* pStage01 = static_cast<CTool_Stage_01*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_TOOL, LEVEL_TOOL, strLayerTag));
		if (nullptr != pStage01)
		{
			m_pStage01 = pStage01;
		}
	}
	
	// 보스 스테이지 LEVEL_TOOL에 추가
	{
		 // if (FAILED(m_pGameInstance->AddGameObjectToLayer(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_Boss_Stage"), _uint(LEVEL_TOOL), strLayerTag, nullptr))) return E_FAIL;
	}


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

	return S_OK;
}

void CLevel_Tool::ResetVtxCnt(_int iCntX, _int iCntZ)
{
	cout << iCntX << ' ' << iCntZ << endl;

	if (nullptr == m_pTempWater)
	{
		m_pObj = m_pGameInstance->GetGameObjctInLayerBack(LEVEL_TOOL, LEVEL_TOOL, TEXT("Layer_TempWater"));

		if (m_pObj != nullptr)
		{
			m_pTempWater = dynamic_cast<CTempWater*>(m_pObj);
			m_pTempWater->Reset_Vtx_Cnt(m_iTempWater_CntZ, m_iTempWater_CntX);
		}
	}
	else
	{
		m_pTempWater->Reset_Vtx_Cnt(m_iTempWater_CntZ, m_iTempWater_CntX);
	}
}

void CLevel_Tool::Initialize_IMGUI()
{
	// ImGui 컨텍스트 생성
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& IO = ImGui::GetIO();
	(void)IO; // 사용하지 않는 변수 경고 방지

	// ImGui 설정
	IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;			// 키보드 내비게이션 활성화
	IO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;			// 게임패드 내비게이션 활성화
	IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;				 // 도킹 기능 활성화
	IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;				 // 멀티 뷰포트 활성화

	// ImGui 스타일 설정 (옵션)
	ImGui::StyleColorsDark(); // 기본 어두운 테마
	// ImGui::StyleColorsClassic(); // 클래식 테마
	// ImGui::StyleColorsLight(); // 밝은 테마

	// ImGui Win32 백엔드 초기화
	ImGui_ImplWin32_Init(g_hWnd);

	// ImGui DirectX 11 백엔드 초기화
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);
}

void CLevel_Tool::Render_IMGUI()
{
	// ImGui 프레임 시작
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	
	Render_Camera_Pos();
	Render_TempWater();


	if (nullptr != m_pPlayer)
	{
		if (m_RootBone1201.Child.empty() && m_RootBone1205.Child.empty() && m_RootBone1003.Child.empty() && m_RootBone1004.Child.empty() && m_RootBone1401.Child.empty())
		{
			m_str1201 = ReadBoneHierachyFromXML("../Bin/resources/Models/GF_Player/1201_Right/1201_Right_Bones.xml", m_RootBone1201);
			m_str1205 = ReadBoneHierachyFromXML("../Bin/resources/Models/GF_Player/1205_Right/1205_Right_Bones.xml", m_RootBone1205);
			m_str1004 = ReadBoneHierachyFromXML("../Bin/resources/Models/GF_Player/1004_Right/1004_Right_Bones.xml", m_RootBone1004);
			m_str1003 = ReadBoneHierachyFromXML("../Bin/resources/Models/GF_Player/1003_Right/1003_Right_Bones.xml", m_RootBone1003);
			m_str1401 = ReadBoneHierachyFromXML("../Bin/resources/Models/GF_Player/1401_Right/1401_Right_Bones.xml", m_RootBone1401);
			m_str1008 = ReadBoneHierachyFromXML("../Bin/resources/Models/GF_Player/1008_Right/1008_Right_Bones.xml", m_RootBone1008);
			m_str1503 = ReadBoneHierachyFromXML("../Bin/resources/Models/GF_Player/1503_Right/1503_Right_Bones.xml", m_RootBone1503);
			m_str1508 = ReadBoneHierachyFromXML("../Bin/resources/Models/GF_Player/1508_Right/1508_Right_Bones.xml", m_RootBone1508);





		}
		else
			ShowBoneHierarchy();
	}

	RenderAnimTool();

	RenderPlayerAnimInfo();
	Render_Picking_Info();


	RenderPickedObjectAnimInfo();


	RenderMonsterBonesFromXML();


	RenderSpawnTriggerInfo();


	RenderInterMoveInfo();


	// ImGui 프레임 완료
	ImGui::Render();


	// ImGui 그리기
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
}

void CLevel_Tool::Render_Camera_Pos()
{
	m_pObj = m_pGameInstance->GetGameObjctInLayerBack(LEVEL_TOOL, LEVEL_TOOL, TEXT("Layer_Camera"));
	if (m_pObj)
	{
		m_pCam = static_cast<CTool_Main_Camera*>(m_pObj);

		if (m_pCam)
		{
			ImGui::Begin("Camera");
			_vector vPos = m_pCam->Get_Pos();

			_float fX = *((_float*)&vPos + 0);
			_float fY = *((_float*)&vPos + 1);
			_float fZ = *((_float*)&vPos + 2);
			_float fW = *((_float*)&vPos + 3);

			ImGui::Text("Values: %.2f, %.2f, %.2f, %.2f", fX, fY, fZ, fW);

			ImGui::End();
		}
	}
}

void CLevel_Tool::Render_TempWater()
{
	// ImGui UI 생성 (예시)
	{
		ImGui::Begin("Temp Water");
		ImGui::Text("Input Cnt X, Y");

		ImGui::InputInt("Input Cnt Z ", &m_iTempWater_CntZ);
		ImGui::InputInt("Input Cnt X ", &m_iTempWater_CntX);

		if (ImGui::Button("Adjust"))
			ResetVtxCnt(m_iTempWater_CntZ, m_iTempWater_CntX); // 버튼이 클릭되면 MyFunction 호출


		ImGui::Text("Input TW Light Dir");
		ImGui::InputFloat("DIR X", &m_fTempWater_LightX);
		ImGui::InputFloat("DIR Y", &m_fTempWater_LightY);
		ImGui::InputFloat("DIR Z", &m_fTempWater_LightZ);

		if (ImGui::Button("Adjust TW Light Dir"))
		{
			m_vTempWater_LightDir = { m_fTempWater_LightX, m_fTempWater_LightY, m_fTempWater_LightZ };
			m_pTempWater->Set_Light_Direction(m_vTempWater_LightDir);
		}


		ImGui::Text("Diffuse Light");
		ImGui::InputFloat("Diffuse R", &m_fTempWater_Light_DiffuseR);
		ImGui::InputFloat("Diffuse G", &m_fTempWater_Light_DiffuseG);
		ImGui::InputFloat("Diffuse B", &m_fTempWater_Light_DiffuseB);

		if (ImGui::Button("Adjust Diffuse Light"))
		{
			m_pTempWater->Set_Light_Diffuse({ m_fTempWater_Light_DiffuseR, m_fTempWater_Light_DiffuseG, m_fTempWater_Light_DiffuseB });
		}

		ImGui::Text("Ambient Light");
		ImGui::InputFloat("Ambient R", &m_fTempWater_Light_AmbientR);
		ImGui::InputFloat("Ambient G", &m_fTempWater_Light_AmbientG);
		ImGui::InputFloat("Ambient B", &m_fTempWater_Light_AmbientB);

		if (ImGui::Button("Adjust Ambient"))
		{
			m_pTempWater->Set_Light_Ambient({ m_fTempWater_Light_AmbientR, m_fTempWater_Light_AmbientG, m_fTempWater_Light_AmbientB });
		}

		ImGui::Text("Specular Light");
		ImGui::InputFloat("Specular R", &m_fTempWater_Light_SpecularR);
		ImGui::InputFloat("Specular G", &m_fTempWater_Light_SpecularG);
		ImGui::InputFloat("Specular B", &m_fTempWater_Light_SpecularB);

		if (ImGui::Button("Adjust Specular"))
		{
			m_pTempWater->Set_Light_Specular({ m_fTempWater_Light_AmbientR, m_fTempWater_Light_AmbientG, m_fTempWater_Light_AmbientB });
		}


		ImGui::Text("Specular P");
		ImGui::InputFloat("POW : ", &m_fP);

		if (ImGui::Button("Adjust P"))
		{
			if (m_pTempWater != nullptr)
			{
				m_pTempWater->Set_Light_Specular_P(m_fP);
			}
		}

		ImGui::End();
	}

}

void CLevel_Tool::Render_Picking_Info()
{
	ImGui::Begin("Picking Info");

	// 피킹 여부
	if (ImGui::CollapsingHeader("Picking"))
	{
		if (ImGui::BeginTable("split", 2))
		{
			ImGui::TableNextColumn(); ImGui::Checkbox("No Picking", &m_bNotPicking);
			ImGui::TableNextColumn(); ImGui::Checkbox("Use Picking", &m_bUsePicking);
			ImGui::EndTable(); // 테이블 종료
		}
	}

	// 어디에 피킹하여 스폰 할 것인지
	if (ImGui::CollapsingHeader("Pick on Where"))
	{
		if (ImGui::BeginTable("split", 3))
		{
			ImGui::TableNextColumn(); ImGui::Checkbox("Floor", &m_bPickOnFloor);
			ImGui::TableNextColumn(); ImGui::Checkbox("Terrain", &m_bPickOnTerrain);
			ImGui::TableNextColumn(); ImGui::Checkbox("GameObject", &m_bPickOnGameObject);
			ImGui::EndTable(); // 테이블 종료
		}
	}

	// 스폰을 한다면 무슨 오브젝트를 스폰 할 것인지
	if (ImGui::CollapsingHeader("Spawn Type Or NavMesh"))
	{
		if (ImGui::BeginTable("split", 3))
		{
			ImGui::TableNextColumn(); ImGui::Checkbox("Nav Mesh", &m_bPickNavMesh);
			ImGui::TableNextColumn(); ImGui::Checkbox("Col Cube", &m_bSpawnColCube);
			ImGui::TableNextColumn(); ImGui::Checkbox("AABB", &m_bSpawnAABB);
			ImGui::TableNextColumn(); ImGui::Checkbox("SpawnTrigger", &m_bSpawn_SpawnTrigger);
			ImGui::TableNextColumn(); ImGui::Checkbox("Spawner", &m_bSpawn_Spawner);
			ImGui::TableNextColumn(); ImGui::Checkbox("InterMove", &m_bSpawnInterMove);
			ImGui::TableNextColumn(); ImGui::Checkbox("InterMoveEndPos", &m_bSpawnInterMoveEndPos);


			ImGui::TableNextColumn(); ImGui::Checkbox("Monster01", &m_bSpawnMonster01);
			ImGui::TableNextColumn(); ImGui::Checkbox("Monster02", &m_bSpawnMonster02);
			ImGui::TableNextColumn(); ImGui::Checkbox("Monster03", &m_bSpawnMonster03);
			ImGui::TableNextColumn(); ImGui::Checkbox("Monster04", &m_bSpawnMonster04);
			ImGui::TableNextColumn(); ImGui::Checkbox("Monster05", &m_bSpawnMonster05);
			ImGui::TableNextColumn(); ImGui::Checkbox("Monster06", &m_bSpawnMonster06);
			ImGui::TableNextColumn(); ImGui::Checkbox("Monster07", &m_bSpawnMonster07);

			// 새로 추가 된거
			ImGui::TableNextColumn(); ImGui::Checkbox("Monster08", &m_bSpawnMonster08);
			ImGui::TableNextColumn(); ImGui::Checkbox("Monster09", &m_bSpawnMonster09);

			ImGui::EndTable(); // 테이블 종료
		}
	}

	if (ImGui::Button("Reset Picking Info"))
	{
		Reset_Picking_Info();
	}

	// 스폰을 한다면 무슨 오브젝트를 스폰 할 것인지
	if (ImGui::CollapsingHeader("Nav Mesh"))
	{

		if (m_bPickNavMesh)
		{
			if (ImGui::Button("Export Current NavMesh"))
			{
				ExportNavMesh();
			}
		}

		if (ImGui::Button("Reset Cell Line"))
		{
			ResetCellLine();
		}

		ImGui::Text("Floor Scale X : %.2f", &m_fFloorScaleX);
		ImGui::InputFloat("Floor Scale X Input", &m_fFloorScaleX);
		ImGui::SliderFloat("Floor Scale X Slider", &m_fFloorScaleX, 1.f, 200.f);

		ImGui::Text("Floor Scale Z : %.2f", &m_fFloorScaleZ);
		ImGui::InputFloat("Floor Scale Z Input", &m_fFloorScaleZ);
		ImGui::SliderFloat("Floor Scale Z Slider", &m_fFloorScaleZ, 1.f, 200.f);


		ImGui::Text("Floor POS X : %.2f", &m_fFloorX);
		ImGui::InputFloat("Floor POS X Input", &m_fFloorX);
		ImGui::SliderFloat("Floor POS X Slider", &m_fFloorX, -150.f, 150.f);

		ImGui::Text("Floor POS Y : %.2f", &m_fFloorY);
		ImGui::InputFloat("Floor POS Y Input", &m_fFloorY);
		ImGui::SliderFloat("Floor POS Y Slider", &m_fFloorY, -5.f, 10.f);

		ImGui::Text("Floor POS Z : %.2f", &m_fFloorZ);
		ImGui::InputFloat("Floor POS Z Input", &m_fFloorZ);
		ImGui::SliderFloat("Floor POS Z Slider", &m_fFloorZ, -150.f, 150.f);


		m_pColFloorTransform->Scaling(m_fFloorScaleX, 0.5f, m_fFloorScaleZ);

		_vector vPos = m_pColFloorTransform->GetTransformState(CTransform::STATE_POS);
		vPos.m128_f32[0] = m_fFloorX;
		vPos.m128_f32[1] = m_fFloorY;
		vPos.m128_f32[2] = m_fFloorZ;
		m_pColFloorTransform->SetTransformState(CTransform::STATE_POS, vPos);

	}


	if (ImGui::CollapsingHeader("AABB"))
	{
		if (ImGui::Button("Save AABB Info"))
		{
			SaveAABBInfo();
		}
	}


	if (nullptr != m_pPlayer)
	{
		if (ImGui::Button("Attach Player To Camera"))
		{
			AttachPlayerToCamera();
		}

		ImGui::SameLine();

		if (ImGui::Button("Detach Player From Camera"))
		{
			DetachPlayerFromCamera();
		}
	}


	if (nullptr != m_pCurrent_Picked_Object && nullptr != m_pCurrent_Picked_Object_Transform)
	{
		ImGui::Begin("Picking Object Info");

		ImGui::Text("Cur Pos : %.2f, %.2f, %.2f", m_vCurrent_Picked_Object_Pos.x, m_vCurrent_Picked_Object_Pos.y, m_vCurrent_Picked_Object_Pos.z);

		if (ImGui::SliderFloat3("Moving XYZ", &m_vCurrent_Picked_Object_Pos.x, -300.0f, 300.0f) || ImGui::InputFloat3("Moving XYZ (Input)", &m_vCurrent_Picked_Object_Pos.x, "%.2f"))
		{
			m_vCurrent_Picked_Object_Move_Pos.w = 0.f;

			_vector vOriginPos = XMLoadFloat4(&m_vCurrent_Picked_Object_Pos);
			m_pCurrent_Picked_Object_Transform->SetTransformState(CTransform::STATE_POS, vOriginPos);
		}


		_vector vUp = m_pCurrent_Picked_Object_Transform->GetTransformState(CTransform::STATE_UP);
		m_pCurrent_Picked_Object_Transform->Rotation(vUp, m_fCurrent_Picked_Object_Rot);


		// Scaling 슬라이더 및 입력 필드
		if (ImGui::SliderFloat3("Scaling XYZ (Slider)", &m_vCurrent_Picked_Object_Scale.x, 1.f, 100.f) || ImGui::InputFloat3("Scaling XYZ (Input)", &m_vCurrent_Picked_Object_Scale.x, "%.2f"))
		{
			m_pCurrent_Picked_Object_Transform->Scaling(m_vCurrent_Picked_Object_Scale.x, m_vCurrent_Picked_Object_Scale.y, m_vCurrent_Picked_Object_Scale.z);
		}



		// Y축 회전 슬라이더 및 입력 필드
		if (ImGui::SliderFloat("Rotation Y (Slider)", &g_fRotationY, -XM_PI, XM_PI, "%.2f rad") ||
			ImGui::InputFloat("Rotation Y (Input)", &g_fRotationY, 0.01f, 0.1f, "%.2f rad"))
		{
			_vector vAxisY = XMVectorSet(0.f, 1.f, 0.f, 0.f); // Y축 벡터
			m_pCurrent_Picked_Object_Transform->Rotation(vAxisY, g_fRotationY);
		}

		if (ImGui::Button("Reset Rot Y"))
		{
			g_fRotationY = 0.f;
		}


		// ImGui::SliderFloat3("Scaling XYZ", &m_vCurrent_Picked_Object_Scale.x, 1.f, 100.f);
		// m_pCurrent_Picked_Object_Transform->Scaling(m_vCurrent_Picked_Object_Scale.x, m_vCurrent_Picked_Object_Scale.y, m_vCurrent_Picked_Object_Scale.z);


		ImGui::SliderFloat("Angle", &m_fCurrent_Picked_Object_Rot, 0.0f, XM_PI, "%.5f deg");
		ImGui::Text("Current Angle: %.5f degrees", m_fCurrent_Picked_Object_Rot);

		ImGui::End();
	}


	if (nullptr != m_pCurrent_Picked_Object)
	{
		if (ImGui::Button("Remove"))
		{
			if (m_pGameInstance->RemoveObject(LEVEL_TOOL, m_pCurrent_Picked_Object))
			{
				SafeRelease(m_pCurrent_Picked_Object);
				SafeRelease(m_pCurrent_Picked_Object_Transform);

				m_pCurrent_Picked_Object = nullptr;
				m_pCurrent_Picked_Object_Transform = nullptr;
			}
		}
	}


	if (m_bSelectedSpawner)
	{
		// Enum을 string으로 변환
		static const _char* pMonsterTypes[] = { "Monster01 Sword Default", "Monster02 Red ", "Monster03 : Arrow", "Monster04 : Yellow ", "Monster05 : horse ", "Monster06 : Chang Default "};

		// 회전 타입 선택

		SPAWN_MONSTER_TYPE eCurSpawnMonsterType = SPAWN_MONSTER_TYPE::MONSTER01;

		static _int iMonsterType = static_cast<_int>(eCurSpawnMonsterType);
		if (ImGui::Combo("Rotation Type", &iMonsterType, pMonsterTypes, IM_ARRAYSIZE(pMonsterTypes)))
		{
			eCurSpawnMonsterType = static_cast<SPAWN_MONSTER_TYPE>(iMonsterType);

			if (nullptr != m_pCurrent_Picked_Object)
			{
				if (false == strcmp("Spawner", m_pCurrent_Picked_Object->GetObjectTag().c_str()))
				{
					static_cast<CSpawner*>(m_pCurrent_Picked_Object)->SetSpawnMonsterType(eCurSpawnMonsterType);
				}
			}


			_int a = 10;
		}

		if (ImGui::Button("Save Spawner Spawn Monster Type"))
		{
			
			m_bSelectedSpawner = false;
		}
	}




	ImGui::End();

}

void CLevel_Tool::Reset_Picking_Info()
{
	m_bNotPicking = true;
	m_bUsePicking = false;

	m_bPickOnFloor = false;
	m_bPickOnTerrain = false;
	m_bPickOnGameObject = false;

	m_bSpawnMonster01 = false;

	m_bSpawnColCube = false;
	m_bSpawnAABB = false;
	m_bSpawn_SpawnTrigger = false;
	m_bSpawn_Spawner = false;
	m_bSpawnInterMove = false;
	m_bSpawnInterMoveEndPos = false;
}

void CLevel_Tool::RenderPlayerAnimInfo()
{
	ImGui::Begin("Player Anim Info");

	if (nullptr != m_pPlayer)
	{
		if (ImGui::CollapsingHeader("Player Model Toggle"))
		{
			CTool_Player* pPlayer = static_cast<CTool_Player*>(m_pPlayer);

			if (ImGui::Button("Next Model"))
			{
				m_pPlayerModel->ResetCurrentAnimation();
				pPlayer->Increase_ModelNum();
				m_pPlayerModel = static_cast<CModel*>(pPlayer->Get_CurrentModel());
			}

			ImGui::SameLine();

			if (ImGui::Button("Prev Model"))
			{
				pPlayer->Decrease_ModelNum();
				m_pPlayerModel->ResetCurrentAnimation();
				m_pPlayerModel = static_cast<CModel*>(pPlayer->Get_CurrentModel());
			}
		}

		if (ImGui::CollapsingHeader("Player Anim Toggle"))
		{
			ImGui::Text("NumAnimations: %d ", m_iNumModelAnimations);
			ImGui::Text("Current Animation Index: %d ", m_pPlayerModel->GetCurrentAnimationIndex());

			if (ImGui::Button("Play Current Anim"))
			{
				// 현재 애니매이션 재생 중이라면 replay when playing
				if (m_pPlayerModel->IsAnimationPlaying())
				{
					m_pPlayerModel->SetAnimationPlayType(CAnimation::PLAY_TYPE::REPLAY_WHEN_PLAY);
				}
				// 현재 애니매이션 재생중이 아니라면 그대로 ONCE 재생
				else
				{
					m_pPlayerModel->ResetCurrentAnimation();
					m_pPlayerModel->SetAnimationPlayType(CAnimation::PLAY_TYPE::ONCE);
				}
			}

			if (ImGui::Button("Play Next Animation"))
			{
				// m_pPlayerModel->ResetCurrentAnimation();
				_int iFrom = m_pPlayerModel->GetCurrentAnimationIndex();
				_int iTo = m_pPlayerModel->GetNextValidAnimationIndex();
				m_pPlayerModel->SetInterpolationAnimationIndex(iFrom, iTo);

				// m_pPlayerModel->SetCurrentAnimationIndex(m_pPlayerModel->GetNextValidAnimationIndex());

			}
			ImGui::SameLine();

			if (ImGui::Button("Play Prev Animation"))
			{
				// m_pPlayerModel->ResetCurrentAnimation();
				_int iFrom = m_pPlayerModel->GetCurrentAnimationIndex();
				_int iTo = m_pPlayerModel->GetPrevValidAnimationIndex();
				m_pPlayerModel->SetInterpolationAnimationIndex(iFrom, iTo);

				// m_pPlayerModel->ResetCurrentAnimation();
				// m_pPlayerModel->SetCurrentAnimationIndex(m_pPlayerModel->GetPrevValidAnimationIndex());
			}
		}

		const vector<CBone*>& vecBones = m_pPlayerModel->GetBoneVector();
	}

	ImGui::End();
}

void CLevel_Tool::ShowBoneHierarchy()
{

	ImGui::Begin("Bone Hierarchy");

	RenderBones(m_RootBone1201, m_str1201);
	RenderBones(m_RootBone1205, m_str1205);
	RenderBones(m_RootBone1003, m_str1003);
	RenderBones(m_RootBone1004, m_str1004);
	RenderBones(m_RootBone1401, m_str1401);
	RenderBones(m_RootBone1008, m_str1008);
	RenderBones(m_RootBone1503, m_str1503);
	RenderBones(m_RootBone1508, m_str1508);
	
	ImGui::End();
	
}

void CLevel_Tool::RenderBones(const BONE& bone, string strRootBoneName)
{
	string RootBoneName = bone.strName + strRootBoneName;

	// 현재 뼈대가 선택되었는지 확인
	_bool isSelected = m_SelectedBonesName.find(bone.strName) != m_SelectedBonesName.end();

	if (ImGui::TreeNode(RootBoneName.c_str()))
	{
		// 체크박스를 트리 노드와 같은 줄에 렌더링
		ImGui::SameLine();
		if (ImGui::Checkbox(("##" + RootBoneName).c_str(), &isSelected))
		{
			if (isSelected)
			{
				m_SelectedBonesName.insert(bone.strName);  // 선택된 뼈대 추가
				CBone* pBone = m_pPlayerModel->GetBone(bone.strName.c_str());
				m_SelectedBones.insert({bone.strName, pBone });
			}
			else
			{
				auto iter = find_if(m_SelectedBones.begin(), m_SelectedBones.end(), [&bone](const std::pair<const string, Engine::CBone*>& pair)
					{
						if (false == strcmp(bone.strName.c_str(), pair.first.c_str()))
							return true;
						return false;
					}
				);

				if (m_SelectedBones.end() != iter)
					m_SelectedBones.erase((*iter).first);

				m_SelectedBonesName.erase(bone.strName);   // 선택된 뼈대 제거
			}
		}

		for (const auto& childBone : bone.Child)
		{
			RenderBones(childBone, "");
		}

		ImGui::TreePop();
	}
}

void CLevel_Tool::RenderAnimTool()
{
	ImGui::Begin("Anim Tool");

	if (0 != m_SelectedBonesName.size())
	{
		string strModelName = m_pPlayerModel->GetModelName();
		ImGui::Text("Current Model Name : %s", strModelName.c_str());

		ImGui::Text("Select Bone Count : %d", m_SelectedBonesName.size());

		for (string Name : m_SelectedBonesName)
		{
			ImGui::Text("Select Bone Name : %s", Name.c_str());
		}

		// 현재 모델의 애니매이션 재생을 막는다. TransfmationMatrix를 수정할 준비를 해준다.
		string strLockBTNName = "Lock Current Mode : " + strModelName;
		if (ImGui::Button(strLockBTNName.c_str()))
		{
			if (!m_bLockAnim)
			{
				m_bLockAnim = true;
				m_pPlayerModel->SetLockAnim();

				// 선택된 뼈 만큼 루프를 돈다. -> 각 뼈에 대한 초기 TransformationMatrix의 주소 TransformationMatrix값, Right, Up, Look 값을 저장한다.
				for (auto& Pair : m_SelectedBones)
				{
					if (nullptr != Pair.second)
					{
						// TransformationMatrix의 주소를 담아둔다.
						_float4x4* pBoneLockTransformMatrix = Pair.second->GetTransformationMatrix();
						m_SelectedBonesTransfomationMatrix.insert({ Pair.first, pBoneLockTransformMatrix });

						// 회전 시킬 기준이 되는 Right, Up, Look을 담아둔다.
						{
							_float3 vRIGHT, vUP, vLOOK, vPOS;

							memcpy(&vRIGHT, &pBoneLockTransformMatrix->m[0], sizeof(_float3));
							memcpy(&vUP, &pBoneLockTransformMatrix->m[1], sizeof(_float3));
							memcpy(&vLOOK, &pBoneLockTransformMatrix->m[2], sizeof(_float3));
							memcpy(&vPOS, &pBoneLockTransformMatrix->m[3], sizeof(_float3));

							XMStoreFloat3(&vRIGHT, XMVector3Normalize(XMLoadFloat3(&vRIGHT)));
							XMStoreFloat3(&vUP, XMVector3Normalize(XMLoadFloat3(&vUP)));
							XMStoreFloat3(&vLOOK, XMVector3Normalize(XMLoadFloat3(&vLOOK)));

							RUL rul{};
							rul.vRight = vRIGHT;
							rul.vUp = vUP;
							rul.vLook = vLOOK;
							rul.vPos = vPOS;
							m_SelectedBonesRUL.insert({ Pair.first, rul });

							// TransformationMatrixOrigin을 담아둔다.
							m_SelectedBonesTransfomationMatrixOrigin.insert({ Pair.first, *Pair.second->GetTransformationMatrix() });
						}

						vector<_float> vRotXYZ = { 0.f, 0.f, 0.f };
						vector<_float> vPosXYZ = { 0.f, 0.f, 0.f };
						m_SelectedBonesRadian.insert({ Pair.first, vRotXYZ });
					}
				}
			}
		}

		// 현재 모델의 애니매이션 재생을 다시 할 수 있도록 Lock을 푼다. TransfmationMatrix를 수정하지 않겠다.
		string strReleaseLockBTNName = "Release Lock Current Model : " + strModelName;
		if (ImGui::Button(strReleaseLockBTNName.c_str()))
		{
			if (m_bLockAnim)
			{
				m_bLockAnim = false;
				m_pPlayerModel->ReleaseLockAnim();

				m_SelectedBonesName.clear();
				m_SelectedBones.clear();
				m_SelectedBonesRUL.clear();
				m_SelectedBonesTransfomationMatrix.clear();
				m_SelectedBonesTransfomationMatrixOrigin.clear();
				m_SelectedBonesRadian.clear();

				m_bStartKeyframe = false;
			}
		}

		m_bStartKeyframe;

		if (m_bLockAnim)
		{
			// 키프레임 기록 시작
			if (ImGui::Button("Start Record KeyFrame"))
				m_bStartKeyframe = true;

			ImGui::InputFloat("Current Animation Speed(Default : 1.0f) : ", &m_fAnimSP, 0.f, 5.f);
			ImGui::InputFloat("Current Keyframe Position : ", &m_fKeyframePosition, 0.f, 2.f);

			if (ImGui::Button("Insert KeyFrame"))
			{
				// TODO
				// 이곳에서 현재 상태의 모든 뼈들의 TransformationMatrix의 상태를 저장해준다.
				// 1. m_SelectedBonesTransfomationMatrix를 가져와 스케일, rotation, position을 저장한다.
				// 2. 회전의 경우 right, up, look을 가져와 쿼터니언으로 만들어 KEYFRAME구조체에 담에 m_SelectedBonesKeyframes 에 insert해준다.
				for (auto& Pair : m_SelectedBones)
				{
					if (nullptr != Pair.second && m_SelectedBonesTransfomationMatrix.end() != m_SelectedBonesTransfomationMatrix.find(Pair.first))
					{
						auto iter = m_SelectedBonesKeyframes.find(Pair.first);

						// 존재 한다면
						if (m_SelectedBonesKeyframes.end() != iter)
						{
							auto iterMatrix = m_SelectedBonesTransfomationMatrix.find(Pair.first);
							if (iterMatrix != m_SelectedBonesTransfomationMatrix.end())
							{
								_float4x4* pMat = (*iterMatrix).second;
								_matrix mat = XMLoadFloat4x4(pMat);
								
								_float3 vRight, vUp, vLook, vPos;
								SetRightUpLookPosFromTransformationMatrix(vRight, vUp, vLook, vPos, mat);

								_float4 vQuaternion;
								CreateRotationQuaternion(vRight, vUp, vLook, vQuaternion);

								KEYFRAME CurrentKeyframe = {};
								CurrentKeyframe.vPosition = vPos;
								CurrentKeyframe.vRotation = vQuaternion;
								CurrentKeyframe.vScale = _float3{ 1.f, 1.f, 1.f };

								// 위에서 IMGUI로 입력받은 m_fKeyframePosition를 vKeyFramePosition에 넣어준다.
								CurrentKeyframe.vKeyFramePosition = m_fKeyframePosition;

								m_SelectedBonesKeyframes.at(Pair.first).push_back(CurrentKeyframe);
							}
						}
						// 존재 하지 않는다면 삽입 후 벡터에 추가한다.
						else
						{
							auto iterMatrix = m_SelectedBonesTransfomationMatrix.find(Pair.first);
							if (iterMatrix != m_SelectedBonesTransfomationMatrix.end())
							{
								_float4x4* pMat = (*iterMatrix).second;
								_matrix mat = XMLoadFloat4x4(pMat);

								_float3 vRight, vUp, vLook, vPos;
								SetRightUpLookPosFromTransformationMatrix(vRight, vUp, vLook, vPos, mat);

								_float4 vQuaternion;
								CreateRotationQuaternion(vRight, vUp, vLook, vQuaternion);

								KEYFRAME CurrentKeyframe = {};
								CurrentKeyframe.vPosition = vPos;
								CurrentKeyframe.vRotation = vQuaternion;
								CurrentKeyframe.vScale = _float3{ 1.f, 1.f, 1.f };
								CurrentKeyframe.vKeyFramePosition = m_fKeyframePosition;

								m_SelectedBonesKeyframes.insert({ Pair.first, vector<keyframe>()});
								m_SelectedBonesKeyframes.at(Pair.first).push_back(CurrentKeyframe);
							}
						}
					}
				}
			}

			ImGui::SameLine();

			// 키프레임 기록 종료
			if (ImGui::Button("End Record KeyFrame"))			
				m_bStartKeyframe = false;
			
			if (ImGui::RadioButton("IDLE ", m_eSaveAnimNameType == 0))
				m_eSaveAnimNameType = ANIM_IDLE;	
			if (ImGui::RadioButton("WALK", m_eSaveAnimNameType == 1))
				m_eSaveAnimNameType = ANIM_WALK;
			if (ImGui::RadioButton("PUT", m_eSaveAnimNameType == 2))
				m_eSaveAnimNameType = ANIM_PUT;
			if (ImGui::RadioButton("DRAW", m_eSaveAnimNameType == 3))
				m_eSaveAnimNameType = ANIM_DRAW;
			if (ImGui::RadioButton("JUMP", m_eSaveAnimNameType == 4))
				m_eSaveAnimNameType = ANIM_JUMP;
			if (ImGui::RadioButton("HOME", m_eSaveAnimNameType == 5))
				m_eSaveAnimNameType = ANIM_HOME;

			if (ImGui::Button("Save KeyFrame To .Anim"))
			{

				if (ANIM_END == m_eSaveAnimNameType)
				{
					MSG_BOX("애니매이션 타입 설정해라...");
				}
				else
				{
					if (FAILED(OpenKeyFrameDialog(m_eSaveAnimNameType)))
						MSG_BOX(".Anim저장 실패");
					else
						MSG_BOX(".Anim저장 굿");
				}
					
			}
		}


		// TransformationMatrix를 수정할 준비가 되었다면...
		if (m_bLockAnim && m_bStartKeyframe)
		{
			for (auto& Pair : m_SelectedBones)
			{
				if (nullptr != Pair.second)
				{
					// TODO 이곳에서 각 뼈의 RUL을 수정 할 수 있도록 해준다
					string strSliderNameX = "Rot X " + Pair.first;
					string strSliderNameY = "Rot Y " + Pair.first;
					string strSliderNameZ = "Rot Z " + Pair.first;

					auto iter = m_SelectedBonesRadian.find(Pair.first);
					ImGui::SliderFloat(strSliderNameX.c_str(), &(*iter).second[0], -XM_PI, XM_PI, "%.3f deg");
					ImGui::SliderFloat(strSliderNameY.c_str(), &(*iter).second[1], -XM_PI, XM_PI, "%.3f deg");
					ImGui::SliderFloat(strSliderNameZ.c_str(), &(*iter).second[2], -XM_PI, XM_PI, "%.3f deg");

					auto RULiter = m_SelectedBonesRUL.find(Pair.first);
					_matrix RotXMatrix = XMMatrixRotationAxis(XMLoadFloat3(&(*RULiter).second.vRight), (*iter).second[0]);
					_matrix RotYMatrix = XMMatrixRotationAxis(XMLoadFloat3(&(*RULiter).second.vUp), (*iter).second[1]);
					_matrix RotZMatrix = XMMatrixRotationAxis(XMLoadFloat3(&(*RULiter).second.vLook), (*iter).second[2]);

					ImGui::InputFloat(("##" + strSliderNameX).c_str(), &(*iter).second[0], 0.0f, 0.0f, "%.3f");
					ImGui::InputFloat(("##" + strSliderNameY).c_str(), &(*iter).second[1], 0.0f, 0.0f, "%.3f");
					ImGui::InputFloat(("##" + strSliderNameZ).c_str(), &(*iter).second[2], 0.0f, 0.0f, "%.3f");

					_matrix RotationMatrix = RotXMatrix * RotYMatrix * RotZMatrix;

					// 중요한 개념이 들어간다.
					// - 원본 행렬 * RotaionMatrix를 곱하는데 원본 행렬의 각 축을 RotaionMatrix랑 곱해 회전 시킨다.

					auto OriginIter = m_SelectedBonesTransfomationMatrixOrigin.find(Pair.first);
					_matrix matRotation = XMLoadFloat4x4(&(*OriginIter).second) * RotationMatrix;

					auto TransformPtrIter = m_SelectedBonesTransfomationMatrix.find(Pair.first);
					XMStoreFloat4((_float4*)(*TransformPtrIter).second->m[0], matRotation.r[0]);
					XMStoreFloat4((_float4*)(*TransformPtrIter).second->m[1], matRotation.r[1]);
					XMStoreFloat4((_float4*)(*TransformPtrIter).second->m[2], matRotation.r[2]);
				}
			}			
		}
		else
		{
			// TODO...
		}
	}

	if (ImGui::RadioButton("Load Type : IDLE ", m_eLoadAnimNameType == 0)) m_eLoadAnimNameType = ANIM_IDLE;
	if (ImGui::RadioButton("Load Type : WALK", m_eLoadAnimNameType == 1)) m_eLoadAnimNameType = ANIM_WALK;
	if (ImGui::RadioButton("Load Type : PUT", m_eLoadAnimNameType == 2)) m_eLoadAnimNameType = ANIM_PUT;
	if (ImGui::RadioButton("Load Type : DRAW", m_eLoadAnimNameType == 3)) m_eLoadAnimNameType = ANIM_DRAW;
	if (ImGui::RadioButton("Load Type : JUMP", m_eLoadAnimNameType == 4)) m_eLoadAnimNameType = ANIM_JUMP;

	if (ImGui::Button("Load KeyFrame From .Anim"))
	{
		if (ANIM_END != m_eLoadAnimNameType)
		{
			LOADANIMTYPE eType;
			switch (m_eLoadAnimNameType)
			{
			case Tool::CLevel_Tool::ANIM_IDLE:
				eType = LOADANIMTYPE::LOADANIM_IDLE;
				break;
			case Tool::CLevel_Tool::ANIM_WALK:
				eType = LOADANIMTYPE::LOADANIM_WALK;
				break;
			case Tool::CLevel_Tool::ANIM_PUT:
				eType = LOADANIMTYPE::LOADANIM_PUT;
				break;
			case Tool::CLevel_Tool::ANIM_DRAW:
				eType = LOADANIMTYPE::LOADANIM_DRAW;
				break;
			case Tool::CLevel_Tool::ANIM_JUMP:
				eType = LOADANIMTYPE::LOADANIM_JUMP;
				break;
			case Tool::CLevel_Tool::ANIM_END:
				break;
			default:
				break;
			}

			m_pPlayerModel->LoadAnimFile(eType, "1201_Right");
		}
	}


	ImGui::End();
}

HRESULT CLevel_Tool::OpenKeyFrameDialog(ANIM_NAME_TYPE eAnimType)
{
	OPENFILENAME ofn;
	WCHAR szFile[256] = { 0 };
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;  // 소유자 창 핸들 (nullptr로 설정하면 기본 창으로 작동)
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"anim Files\0*.anim\0All Files\0*.*\0";
	ofn.lpstrDefExt = L"anim";  // 기본 확장자

	WCHAR szInitialDir[256] = { 0 };
	// 실행 파일의 전체 경로를 얻어와서 현재 디렉터리를 추출
	WCHAR modulePath[256] = { 0 };
	GetModuleFileName(nullptr, modulePath, sizeof(modulePath) / sizeof(WCHAR));

	// 경로에서 파일 이름 부분을 제거하여 디렉터리만 남기기
	WCHAR* lastSlash = wcsrchr(modulePath, L'\\');
	if (lastSlash)
	{
		*lastSlash = L'\0';  // 마지막 슬래시 이후를 제거하여 디렉터리 경로만 남김
		wcscpy_s(szInitialDir, modulePath);  // 초기 디렉터리에 설정
	}

	ofn.lpstrInitialDir = szInitialDir;  // 초기 디렉터리 설정
	ofn.Flags = OFN_OVERWRITEPROMPT;  // 덮어쓰기 확인

	// 파일 저장 다이얼로그 표시
	if (GetSaveFileName(&ofn) == TRUE)
	{
		_char pFilePath[256];
		WCharToChar(szFile, pFilePath);

		std::filesystem::path path(pFilePath);
		// 1. 확장자 제거하고 순수 파일명만 추출
		std::string fileName = path.stem().string(); // 예: "1201_Right_IDLE"

		string strAnimName;

		// 2. 특정 문자열 "IDLE" 추출
		size_t pos = fileName.rfind("IDLE");
		if (pos != std::string::npos)
		{
			strAnimName = fileName.substr(pos); // "IDLE" 추출
		}
		else
		{
			strAnimName = ""; // "IDLE"이 없을 경우 빈 문자열 반환
		}

		return SaveAnimationKeyFrame(pFilePath, strAnimName, eAnimType);
	}

	return S_OK;
}

HRESULT CLevel_Tool::SaveAnimationKeyFrame(const _char* pFilePath, string strSavefileName, ANIM_NAME_TYPE eAnimType)
{
	ofstream OutFile(pFilePath, ios::out | ios_base::binary);
	if (!OutFile.is_open())
		return E_FAIL;

	AI_ANIMATION_INFO AnimInfo = {};
	AnimInfo.fDuration = 1.f;
	AnimInfo.fTickPerSecond = m_fAnimSP;
	AnimInfo.iNumChannels = static_cast<_int>(m_SelectedBones.size());		// 채널 개수 저장

	switch (eAnimType)
	{
	case Tool::CLevel_Tool::ANIM_IDLE:
		strcpy_s(AnimInfo.szAnimName, "IDLE");
		break;
	case Tool::CLevel_Tool::ANIM_WALK:
		strcpy_s(AnimInfo.szAnimName, "WALK");
		break;
	case Tool::CLevel_Tool::ANIM_PUT:
		strcpy_s(AnimInfo.szAnimName, "PUT");
		break;
	case Tool::CLevel_Tool::ANIM_DRAW:
		strcpy_s(AnimInfo.szAnimName, "DRAW");
		break;
	case Tool::CLevel_Tool::ANIM_JUMP:
		strcpy_s(AnimInfo.szAnimName, "JUMP");
		break;
	case Tool::CLevel_Tool::ANIM_HOME:
		strcpy_s(AnimInfo.szAnimName, "HOME");
		break;
	case Tool::CLevel_Tool::ANIM_END:
		return E_FAIL;
		break;
	default:
		break;
	}

	AnimInfo.iLenAnimName = static_cast<_int>(strlen(AnimInfo.szAnimName) + 1); // anim 이름 길이 저장
	OutFile.write((_char*)&AnimInfo, sizeof(AI_ANIMATION_INFO));				// 쓴다.

	const _float fTotalTime = 1.f;
	
	if (m_SelectedBonesName.size() != m_SelectedBones.size())
		return E_FAIL;

	// AnimInfo.szAnimName에 해당하는 채널들의 정보를 쓴다.
	for (auto& val : m_SelectedBonesName)
	{
		AI_CHANNEL_INFO cinfo = {};
		strcpy_s(cinfo.szChannelName, val.c_str());
		cinfo.iBoneIndex = m_pPlayerModel->GetBoneIndex(cinfo.szChannelName);
		cinfo.iLenChannelName = static_cast<_int>(strlen(cinfo.szChannelName) + 1);
		cinfo.iNumKeyFrame;

		auto iter = m_SelectedBonesKeyframes.find(cinfo.szChannelName);
		if (iter != m_SelectedBonesKeyframes.end())
		{
			_int iSize = static_cast<_int>((*iter).second.size());		// 해당 채널의 키프레임 정보를 쓴다.
			cinfo.iNumKeyFrame = iSize;
		}
		else
			return E_FAIL;

		// 쓴다.
		OutFile.write((_char*)&cinfo, sizeof(AI_CHANNEL_INFO));

		const vector<KEYFRAME>& vecKeyframes = (*iter).second;
		for (const auto& keyframe : vecKeyframes)
		{
			OutFile.write((_char*)&keyframe, sizeof(KEYFRAME));
		}
	}	

	OutFile.close();

	m_fAnimSP = 0.f;

	return S_OK;
}

void CLevel_Tool::SetRightUpLookPosFromTransformationMatrix(_float3& vRight, _float3& vUp, _float3& vLook, _float3& vPos, const _fmatrix& TransformationMatrix)
{
	memcpy(&vRight, &TransformationMatrix.r[0], sizeof(_float3));
	memcpy(&vUp, &TransformationMatrix.r[1], sizeof(_float3));
	memcpy(&vLook, &TransformationMatrix.r[2], sizeof(_float3));
	memcpy(&vPos, &TransformationMatrix.r[3], sizeof(_float3));
}

void CLevel_Tool::CreateRotationQuaternion(const _float3& vRight, const _float3& vUp, const _float3& vLook, _float4& vRotation)
{
	// 회전 행렬 생성
	_float4x4 rotationMatrix;
	rotationMatrix._11 = vRight.x; rotationMatrix._12 = vRight.y; rotationMatrix._13 = vRight.z; rotationMatrix._14 = 0.0f;
	rotationMatrix._21 = vUp.x;    rotationMatrix._22 = vUp.y;    rotationMatrix._23 = vUp.z;    rotationMatrix._24 = 0.0f;
	rotationMatrix._31 = vLook.x;  rotationMatrix._32 = vLook.y;  rotationMatrix._33 = vLook.z;  rotationMatrix._34 = 0.0f;
	rotationMatrix._41 = 0.0f;     rotationMatrix._42 = 0.0f;     rotationMatrix._43 = 0.0f;     rotationMatrix._44 = 1.0f;

	// 회전 행렬을 쿼터니언으로 변환
	_matrix xmMatrix = XMLoadFloat4x4(&rotationMatrix);
	_vector quaternion = XMQuaternionRotationMatrix(xmMatrix);
	XMStoreFloat4(&vRotation, quaternion); // 쿼터니언을 _float4 구조체에 저장
}

void CLevel_Tool::RenderPickedObjectAnimInfo()
{
	ImGui::Begin("Anim Info");

	if (0 != m_iNumModelAnimations)
	{
		if (ImGui::CollapsingHeader("Model Toggle"))
		{
			CTool_Player* pPlayer = static_cast<CTool_Player*>(m_pCurrent_Picked_Object);

			// ImGui::Text("Num Models: %d ", pPlayer->Get_NumModels());

			if (ImGui::Button("Next Model"))
			{
				m_pModel->ResetCurrentAnimation();
				pPlayer->Increase_ModelNum();
				m_pModel = static_cast<CModel*>(pPlayer->Get_CurrentModel());
			}

			ImGui::SameLine();

			if (ImGui::Button("Prev Model"))
			{
				pPlayer->Decrease_ModelNum();
				m_pModel->ResetCurrentAnimation();
				m_pModel = static_cast<CModel*>(pPlayer->Get_CurrentModel());
			}
		}

		if (ImGui::CollapsingHeader("Anim Toggle"))
		{
			ImGui::Text("NumAnimations: %d ", m_iNumModelAnimations);


			ImGui::Text("Current Animation Index: %d ", m_pModel->GetCurrentAnimationIndex());


			// 현재 애니매이션 재생
			if (ImGui::Button("Play Current Anim"))
			{
				// 현재 애니매이션 재생 중이라면 replay when playing
				if (m_pModel->IsAnimationPlaying())
				{
					m_pModel->SetAnimationPlayType(CAnimation::PLAY_TYPE::REPLAY_WHEN_PLAY);
				}
				// 현재 애니매이션 재생중이 아니라면 그대로 ONCE 재생
				else
				{
					m_pModel->SetAnimationPlayType(CAnimation::PLAY_TYPE::ONCE);
				}
			}

			if (ImGui::Button("Play Next Animation"))
			{
				_int iFrom = m_pModel->GetCurrentAnimationIndex();
				_int iTo = m_pModel->GetNextValidAnimationIndex();
				m_pModel->SetInterpolationAnimationIndex(iFrom, iTo);
			}
			ImGui::SameLine();

			if (ImGui::Button("Play Prev Animation"))
			{
				_int iFrom = m_pModel->GetCurrentAnimationIndex();
				_int iTo = m_pModel->GetPrevValidAnimationIndex();
				m_pModel->SetInterpolationAnimationIndex(iFrom, iTo);
			}


			/*if (ImGui::Button("Play Next Animation"))
			{
				m_pModel->ResetCurrentAnimation();
				_int iValidIndex = m_pModel->GetNextValidAnimationIndex();
				m_pModel->SetCurrentAnimationIndex(m_pModel->GetNextValidAnimationIndex());
				
			}
			ImGui::SameLine();

			if (ImGui::Button("Play Prev Animation"))
			{
				m_pModel->ResetCurrentAnimation();
				m_pModel->SetCurrentAnimationIndex(m_pModel->GetPrevValidAnimationIndex());
			}*/
		}
	}

	ImGui::End();
}

void CLevel_Tool::RenderSpawnTriggerInfo()
{

	ImGui::Begin("Spawn Trigger Settings");

	// TODO
	if (ImGui::Button("Use Spawn Trigger"))
	{
		m_bUseSpawnTrigger ? m_bUseSpawnTrigger = false : m_bUseSpawnTrigger = true;
	}

	if (m_bUseSpawnTrigger)
	{
		static _bool bUsingPicking = false;

		// Enum을 string으로 변환
		static const _char* rotTypeLabels[] = { "Left", "Back", "Right", "Front" };

		// 전역 또는 클래스 멤버 변수 예제
		static _int iSpawnTriggerNumMonsters = 0;
		static ROT_TYPE eSpawnTriggerMonsterRotType = ROT_NONE;

		// 스폰할 몬스터 수
		ImGui::InputInt("Number of Monsters", &iSpawnTriggerNumMonsters);

		// 회전 타입 선택
		static _int rotType = static_cast<_int>(eSpawnTriggerMonsterRotType);
		if (ImGui::Combo("Rotation Type", &rotType, rotTypeLabels, IM_ARRAYSIZE(rotTypeLabels)))
		{
			eSpawnTriggerMonsterRotType = static_cast<ROT_TYPE>(rotType);
		}

		if (ImGui::Button("Set Spawn Trigger Info") && false == bUsingPicking)
		{
			bUsingPicking = true;

			m_iSpawnTriggerNumMonsters = iSpawnTriggerNumMonsters;
			m_eSpawnTriggerMonsterRotType = eSpawnTriggerMonsterRotType;

			m_iSpawnerPickingCount = m_iSpawnTriggerNumMonsters;

			m_SpawnerMonsterTypes.clear();
			m_SpawnerMonsterTypes.resize(m_iSpawnTriggerNumMonsters);
		}

		if (ImGui::Button("Reset Spawn Trigger Info"))
		{
			bUsingPicking = false;

			iSpawnTriggerNumMonsters = 0;
			eSpawnTriggerMonsterRotType = ROT_NONE;
			m_iSpawnTriggerNumMonsters = 0;
			m_eSpawnTriggerMonsterRotType = ROT_NONE;
		}

		if (bUsingPicking)
		{
			

		}



		if (0 == m_iSpawnerPickingCount)
		{
			if (ImGui::Button("Save SpawnTrigger And Spawners Infos"))
			{
				SaveCurrentSpawnTriggerInfo();
			}
		}
	}

	ImGui::End();
}

void CLevel_Tool::RenderInterMoveInfo()
{
	ImGui::Begin("InterMove Use");


	static _bool bUseInterMove = false;

	// TODO
	if (ImGui::Button("Use InterMove"))
	{
		bUseInterMove ? bUseInterMove = false : bUseInterMove = true;
	}

	if (bUseInterMove)
	{
		static _bool bUsingPicking = false;

		// Enum을 string으로 변환
		static const _char* rotTypeLabels[] = { "Left", "Back", "Right", "Front" };
		static ROT_TYPE eSpawnTriggerMonsterRotType = ROT_NONE;


		// 회전 타입 선택
		static _int rotType = static_cast<_int>(eSpawnTriggerMonsterRotType);
		if (ImGui::Combo("InterMove End Pos Rotation Type", &rotType, rotTypeLabels, IM_ARRAYSIZE(rotTypeLabels)))
		{
			eSpawnTriggerMonsterRotType = static_cast<ROT_TYPE>(rotType);
		}

		if (ImGui::Button("Set InterMove Info") && false == bUsingPicking)
		{
			bUsingPicking = true;
			m_eMoveEndPosRotType = eSpawnTriggerMonsterRotType;
			m_iMoveEndPosPickingCount = 1;
		}

		if (ImGui::Button("Reset InterMove Trigger Info"))
		{
			bUsingPicking = false;

			eSpawnTriggerMonsterRotType = ROT_NONE;
			m_eMoveEndPosRotType = ROT_NONE;
		}

		if (0 == m_iMoveEndPosPickingCount)
		{
			if (ImGui::Button("Save Move End Pos"))
			{
				SaveMoveEndPosInfo();
			}
		}
	}

	ImGui::End();
}

void CLevel_Tool::AttachPlayerToCamera()
{
	m_bFollowCameraPos = true;
}

void CLevel_Tool::DetachPlayerFromCamera()
{
	m_bFollowCameraPos = false;
}

// 스크린 좌표롤 부터 Ray의 시작 위치와 끝 위치를 계산해주는 함수입니다.
void CLevel_Tool::Cal_RayDirAndRayOriginPos(_int iScreenX, _int iScreenY, _vector& vRayDirection, _vector& vRayOriginPos)
{
	// 뷰, 투영(원근) 행렬을 얻어온다.
	_matrix     viewMatrix = m_pGameInstance->GetTransformMatrix(CPipeLine::DTS::D3DTS_VIEW);
	_matrix     projMatrix = m_pGameInstance->GetTransformMatrix(CPipeLine::DTS::D3DTS_PROJ);

	// 마우스 클릭 위치를 NDC로 변환
	_float      mouseNDCX = (_float(iScreenX) / _float(g_iWinSizeX)) * 2.f - 1.f; // -1.0 ~ 1.0
	_float      mouseNDCY = 1.f - (_float(iScreenY) / _float(g_iWinSizeY)) * 2.f; // -1.0 ~ 1.0

	// 뷰, 투영 역행렬 계산
	_matrix     invV = XMMatrixInverse(nullptr, viewMatrix);
	_matrix     invP = XMMatrixInverse(nullptr, projMatrix);

	_matrix     invVP = invP * invV;

	// NDC를 월드 공간으로 변환
	// NDC상에서 가장 가까운 곳과 가장 먼 곳의 좌표를 잡아준다.
	_vector     vNearPoint = XMVectorSet(mouseNDCX, mouseNDCY, 0.f, 1.f);
	_vector     vFarPoint = XMVectorSet(mouseNDCX, mouseNDCY, 1.f, 1.f);

	// 이 두 점을 빼면 Ray가 나온다.
	_vector     vWorldNear = XMVector3TransformCoord(vNearPoint, invVP);
	_vector     vWorldFar = XMVector3TransformCoord(vFarPoint, invVP);

	// 뷰 스페이스 변환 행렬의 역행렬의 3행 성분이 카메라 월드 위치이다.
	_matrix     vInvView = XMMatrixInverse(nullptr, viewMatrix);
	_vector     vRayOrigin = vInvView.r[3];
	XMVectorSetW(vRayOrigin, 1.f);

	_float4 vPos;
	XMStoreFloat4(&vPos, vRayOrigin);

	// Ray의 방향을 구해준다.
	_vector     vRayDir = XMVector3Normalize(XMVectorSubtract(vWorldFar, vWorldNear));

	vRayDirection = vRayDir;
	vRayOriginPos = vRayOrigin;
}

_bool CLevel_Tool::Picking(PICKING_TYPE ePickType, SPAWN_TPYE eSpawnType, _int iScreenX, _int iScreenY, _float4& vPickPos, _float& fDistance)
{
	m_pColFloorBoundingBox->SetUsePicking(true);

	_vector vRayOriginPos, vRayDirection;

	Cal_RayDirAndRayOriginPos(iScreenX, iScreenY, vRayDirection, vRayOriginPos);

	CGameObject* pObj = nullptr;

	// 바닥 피킹을 원하는 경우
	if (m_bPickOnFloor)
	{
		if (m_pColFloorBoundingBox->Check_Picking(vRayOriginPos, vRayDirection, fDistance, vPickPos))
		{
			if (m_bPickNavMesh)
			{
				return true;
			}
			else if (m_bSpawnColCube)
			{
				m_pGameInstance->SpawnObjectAtPickingPos(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_ColCube"), _uint(LEVEL_TOOL), TEXT("Layer_ColCube"), XMFLOAT3{ vPickPos.x, vPickPos.y, vPickPos.z }, "ColCube", nullptr);
			}
			else if (m_bSpawnAABB)
			{
				m_pGameInstance->SpawnObjectAtPickingPos(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_AABB"), _uint(LEVEL_TOOL), TEXT("Layer_AABB"), XMFLOAT3{ vPickPos.x, vPickPos.y, vPickPos.z }, "AABB", nullptr);
			}

			else if (m_bSpawn_SpawnTrigger)
			{
				// TODO
				m_pGameInstance->SpawnObjectAtPickingPos(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_SpawnTrigger"), _uint(LEVEL_TOOL), TEXT("Layer_SpawnTrigger"), _float3{ vPickPos.x, vPickPos.y, vPickPos.z }, "SpawnTrigger", nullptr);
			}

			else if (m_bSpawn_Spawner)
			{
				if (m_iSpawnerPickingCount > 0)
				{
					m_pGameInstance->SpawnObjectAtPickingPos(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_Spawner"), _uint(LEVEL_TOOL), TEXT("Layer_Spawner"), _float3{ vPickPos.x, vPickPos.y, vPickPos.z }, "Spawner", nullptr);
					m_iSpawnerPickingCount--;
				}
			}

			else if (m_bSpawnInterMove)
			{
				m_pGameInstance->SpawnObjectAtPickingPos(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_InterMove"), _uint(LEVEL_TOOL), TEXT("Layer_InterMove"), _float3{ vPickPos.x, vPickPos.y, vPickPos.z }, "InterMove", nullptr);
			}

			else if (m_bSpawnInterMoveEndPos)
			{
				if (m_iMoveEndPosPickingCount > 0)
				{
					m_pGameInstance->SpawnObjectAtPickingPos(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_InterMoveEndPos"), _uint(LEVEL_TOOL), TEXT("Layer_InterMoveEndPos"), _float3{ vPickPos.x, vPickPos.y, vPickPos.z }, "InterMove", nullptr);
					m_iMoveEndPosPickingCount--;
				}
			}

			else if (m_bSpawnMonster01)
			{
				m_pGameInstance->SpawnObjectAtPickingPos(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_Monster01"), _uint(LEVEL_TOOL), TEXT("Layer_Monster01"), XMFLOAT3{ vPickPos.x, vPickPos.y, vPickPos.z }, "Monster01", nullptr);
			}
			else if (m_bSpawnMonster02)
			{
				m_pGameInstance->SpawnObjectAtPickingPos(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_Monster02"), _uint(LEVEL_TOOL), TEXT("Layer_Monster02"), XMFLOAT3{ vPickPos.x, vPickPos.y, vPickPos.z }, "Monster02", nullptr);

			}
			else if (m_bSpawnMonster03)
			{
				m_pGameInstance->SpawnObjectAtPickingPos(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_Monster03"), _uint(LEVEL_TOOL), TEXT("Layer_Monster03"), XMFLOAT3{ vPickPos.x, vPickPos.y, vPickPos.z }, "Monster03", nullptr);

			}
			else if (m_bSpawnMonster04)
			{
				m_pGameInstance->SpawnObjectAtPickingPos(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_Monster04"), _uint(LEVEL_TOOL), TEXT("Layer_Monster04"), XMFLOAT3{ vPickPos.x, vPickPos.y, vPickPos.z }, "Monster04", nullptr);

			}
			else if (m_bSpawnMonster05)
			{
				m_pGameInstance->SpawnObjectAtPickingPos(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_Monster05"), _uint(LEVEL_TOOL), TEXT("Layer_Monster05"), XMFLOAT3{ vPickPos.x, vPickPos.y, vPickPos.z }, "Monster05", nullptr);

			}
			else if (m_bSpawnMonster06)
			{
				m_pGameInstance->SpawnObjectAtPickingPos(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_Monster06"), _uint(LEVEL_TOOL), TEXT("Layer_Monster06"), XMFLOAT3{ vPickPos.x, vPickPos.y, vPickPos.z }, "Monster06", nullptr);

			}
			else if (m_bSpawnMonster07)
			{
				m_pGameInstance->SpawnObjectAtPickingPos(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_Monster07"), _uint(LEVEL_TOOL), TEXT("Layer_Monster07"), XMFLOAT3{ vPickPos.x, vPickPos.y, vPickPos.z }, "Monster07", nullptr);
			}
			else if (m_bSpawnMonster08)
			{
				m_pGameInstance->SpawnObjectAtPickingPos(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_Monster08"), _uint(LEVEL_TOOL), TEXT("Layer_Monster08"), XMFLOAT3{ vPickPos.x, vPickPos.y, vPickPos.z }, "Monster08", nullptr);
			}
			else if (m_bSpawnMonster09)
			{
				m_pGameInstance->SpawnObjectAtPickingPos(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_Monster09"), _uint(LEVEL_TOOL), TEXT("Layer_Monster09"), XMFLOAT3{ vPickPos.x, vPickPos.y, vPickPos.z }, "Monster09", nullptr);
			}
		}
	}
	// 지형 위에 피킹을 원하는 경우
	else if (m_bPickOnTerrain)
	{
		if (m_bSpawnColCube)
		{
			if (m_pTerrain->Check_Picking(vRayOriginPos, vRayDirection, fDistance, vPickPos, &pObj))
			{
				if (nullptr != pObj)
				{
					m_pGameInstance->SpawnObjectAtPickingPos(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_ColCube"), _uint(LEVEL_TOOL), TEXT("Layer_ColCube"), XMFLOAT3{ vPickPos.x, vPickPos.y, vPickPos.z }, nullptr);
				}
			}
		}
		else if (m_bSpawnMonster01)
		{
			if (m_pTerrain->Check_Picking(vRayOriginPos, vRayDirection, fDistance, vPickPos, &pObj))
			{
				if (nullptr != pObj)
				{
					m_pGameInstance->SpawnObjectAtPickingPos(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_DefaultMonster01"), _uint(LEVEL_TOOL), TEXT("Layer_DefaultMonster01"), XMFLOAT3{ vPickPos.x, vPickPos.y, vPickPos.z }, nullptr);
				}
			}
		}
	}
	// 특정 오브젝트 피킹 하고 싶은 경우
	else if (m_bPickOnGameObject)
	{
		m_pColFloorBoundingBox->SetUsePicking(false);

		pObj = m_pGameInstance->GetPickedGameObject(LEVEL_TOOL, vRayOriginPos, vRayDirection, fDistance, vPickPos);

		if (nullptr != pObj)
		{
			string ObjTag = pObj->GetObjectTag();

			if (false == strcmp("AABB", ObjTag.c_str()))
			{
				m_pCurrent_Picked_Object = pObj;
				m_pCurrent_Picked_Object_Transform = static_cast<CTransform*>(m_pCurrent_Picked_Object->GetComponent(TEXT("Com_Transform")));

				XMStoreFloat4(&m_vCurrent_Picked_Object_Pos, m_pCurrent_Picked_Object_Transform->GetTransformState(CTransform::STATE_POS));

				_float3 vScale = m_pCurrent_Picked_Object_Transform->ComputeScaled();
				XMStoreFloat3(&m_vCurrent_Picked_Object_Scale, XMLoadFloat3(&vScale));

				return true;
			}
			// SpawnTrigger라면
			else if (!strcmp("SpawnTrigger", ObjTag.c_str()))
			{
				m_pCurrent_Picked_Object = pObj;
				m_pCurrent_Picked_Object_Transform = static_cast<CTransform*>(m_pCurrent_Picked_Object->GetComponent(TEXT("Com_Transform")));

				XMStoreFloat4(&m_vCurrent_Picked_Object_Pos, m_pCurrent_Picked_Object_Transform->GetTransformState(CTransform::STATE_POS));

				_float3 vScale = m_pCurrent_Picked_Object_Transform->ComputeScaled();
				XMStoreFloat3(&m_vCurrent_Picked_Object_Scale, XMLoadFloat3(&vScale));

				return true;
			}
			// Spawner라면
			else if (false == strcmp("Spawner", ObjTag.c_str()))
			{
				m_pCurrent_Picked_Object = pObj;
				m_pCurrent_Picked_Object_Transform = static_cast<CTransform*>(m_pCurrent_Picked_Object->GetComponent(TEXT("Com_Transform")));

				XMStoreFloat4(&m_vCurrent_Picked_Object_Pos, m_pCurrent_Picked_Object_Transform->GetTransformState(CTransform::STATE_POS));

				_float3 vScale = m_pCurrent_Picked_Object_Transform->ComputeScaled();
				XMStoreFloat3(&m_vCurrent_Picked_Object_Scale, XMLoadFloat3(&vScale));

				m_bSelectedSpawner = true;

				return true;
			}
			// Spawner라면
			else if (false == strcmp("InterMove", ObjTag.c_str()))
			{
				m_pCurrent_Picked_Object = pObj;
				m_pCurrent_Picked_Object_Transform = static_cast<CTransform*>(m_pCurrent_Picked_Object->GetComponent(TEXT("Com_Transform")));

				XMStoreFloat4(&m_vCurrent_Picked_Object_Pos, m_pCurrent_Picked_Object_Transform->GetTransformState(CTransform::STATE_POS));

				_float3 vScale = m_pCurrent_Picked_Object_Transform->ComputeScaled();
				XMStoreFloat3(&m_vCurrent_Picked_Object_Scale, XMLoadFloat3(&vScale));

				m_bSelectedSpawner = true;

				return true;
			}
			// 플레이어 라면
			else if (0 == strcmp(ObjTag.c_str(), "PLAYER"))
			{
				m_pPlayer = pObj;
				m_pPlayerTransform = static_cast<CTransform*>(m_pPlayer->GetComponent(TEXT("Com_Transform")));
				CComponent* pModel = m_pPlayer->GetComponent(TEXT("Com_Model"));
				m_pPlayerModel = static_cast<CModel*>(pModel);
			}

			// 이전에 피킹 된게 있고 현재 피킹 된 오브젝트가 다르다면 색변경을 해제한다.
			if (nullptr != m_pCurrent_Picked_Object && m_pCurrent_Picked_Object != pObj)
			{
				CColBoundingBox* pBB = dynamic_cast<CColBoundingBox*>(m_pCurrent_Picked_Object->GetComponent(TEXT("Com_BoundingBox")));
				pBB->SetCurrentPick(false);

				auto pModel = m_pCurrent_Picked_Object->GetComponent(TEXT("Com_Model"));
				if (nullptr == pModel) m_iNumModelAnimations = 0;
			}
			


			m_pCurrent_Picked_Object = pObj;
			m_pCurrent_Picked_Object_Transform = static_cast<CTransform*>(m_pCurrent_Picked_Object->GetComponent(TEXT("Com_Transform")));

			CComponent* pModel = m_pCurrent_Picked_Object->GetComponent(TEXT("Com_Model"));
			if (nullptr != pModel)
			{
				m_pModel = static_cast<CModel*>(pModel);

				auto eType = m_pModel->GetModelType();
				if (eType == CModel::TYPE::TYPE_ANIM)
				{
					m_iNumModelAnimations = m_pModel->GetNumAnimations();
				}
			}

			XMStoreFloat4(&m_vCurrent_Picked_Object_Pos, m_pCurrent_Picked_Object_Transform->GetTransformState(CTransform::STATE_POS));

			_float3 vScale = m_pCurrent_Picked_Object_Transform->ComputeScaled();
			XMStoreFloat3(&m_vCurrent_Picked_Object_Scale, XMLoadFloat3(&vScale));

			CColBoundingBox* pBB = dynamic_cast<CColBoundingBox*>(m_pCurrent_Picked_Object->GetComponent(TEXT("Com_BoundingBox")));
			pBB->SetCurrentPick(true);

			return true;
		}

		m_pColFloorBoundingBox->SetUsePicking(true);
	}

	m_pColFloorBoundingBox->SetCurrentPick(false);

	return false;
}

void CLevel_Tool::Set_Current_Picking_Object(CGameObject* pObj)
{
	if (nullptr != pObj)
	{
		if (nullptr != m_pCurrent_Picked_Object)
		{
			SafeRelease(m_pCurrent_Picked_Object);
			SafeRelease(m_pCurrent_Picked_Object_Transform);
		}

		m_pCurrent_Picked_Object = static_cast<CGameObject*>(pObj);
		m_pCurrent_Picked_Object_Transform = static_cast<CTransform*>(m_pCurrent_Picked_Object->GetComponent(TEXT("Com_Transform")));

		SafeAddRef(m_pCurrent_Picked_Object);
		SafeAddRef(m_pCurrent_Picked_Object_Transform);

		_vector vPos = m_pCurrent_Picked_Object_Transform->GetTransformState(CTransform::STATE_POS);
		
		_float4 vtmpPos;
		XMStoreFloat4(&vtmpPos, vPos);
		
		m_vCurrent_Picked_Object_Pos.x = vtmpPos.x;
		m_vCurrent_Picked_Object_Pos.y = vtmpPos.y;
		m_vCurrent_Picked_Object_Pos.z = vtmpPos.z;
	}
}

void CLevel_Tool::PickCellLines()
{
	// 마우스 스크린 좌표
	POINT ptMousePos;
	GetCursorPos(&ptMousePos);
	ScreenToClient(g_hWnd, &ptMousePos);

	_float4					vPickedPos{ 0.f, 0.f, 0.f, 0.f };
	_float					fDist = 0.f;

	// 한번도 누른 적이 없을 때
	if (FIRST == (m_iCellClickCount % CL_END))
	{
		if (m_pGameInstance->MouseDown(MOUSE_KEY::RB))
		{
			// LINE_AB 생성
			m_CellLines.push_back(CreateCellLine());

			Picking(m_ePickingType, m_eSpawnType, ptMousePos.x, ptMousePos.y, vPickedPos, fDist);

			// LINE_AB의 점 A에 가까운 점이 있는지 찾는다.
			FindClosetCellPos(vPickedPos);

			// 현재 찍은 점의 좌표를 LINE_AB의 시점으로 넣는다.
			m_CellPositions.push_back(XMLoadFloat4(&vPickedPos));

			// LINE_AB의 시점, 종점을 설정한다.
			m_CellLines.back()->SetStartEndLinePos(m_CellPositions.back(), m_CellPositions.back());

			++m_iCellClickCount;
		}
	}
	else if (SECOND == (m_iCellClickCount % CL_END))
	{
		// 클릭 중이 아니라면
		if (Picking(m_ePickingType, m_eSpawnType, ptMousePos.x, ptMousePos.y, vPickedPos, fDist))
		{
			FindClosetCellPos(vPickedPos);
			m_vCurrentCellLineEndPos = XMLoadFloat4(&vPickedPos);

			// LINE_AB의 시점 종점을 업데이트 한다.
			m_CellLines[m_iCellClickCount - 1]->UpdateLine(m_CellPositions[m_iCellClickCount - 1], m_vCurrentCellLineEndPos);
		}

		if (m_pGameInstance->MouseDown(MOUSE_KEY::RB))
		{
			FindClosetCellPos(vPickedPos);

			m_CellLines.back()->SetStartEndLinePos(m_CellPositions[m_iCellClickCount - 1], XMLoadFloat4(&vPickedPos));
			m_CellPositions.push_back(XMLoadFloat4(&vPickedPos));
			m_CellLines.push_back(CreateCellLine());
			m_CellLines.back()->SetStartEndLinePos(XMLoadFloat4(&vPickedPos), XMLoadFloat4(&vPickedPos));
			++m_iCellClickCount;
		}
	}
	else if (THIRD == (m_iCellClickCount % CL_END))
	{
		if (Picking(m_ePickingType, m_eSpawnType, ptMousePos.x, ptMousePos.y, vPickedPos, fDist))
		{
			FindClosetCellPos(vPickedPos);
			m_vCurrentCellLineEndPos = XMLoadFloat4(&vPickedPos);
			// LINE_AB의 시점 종점을 업데이트 한다.
			m_CellLines[m_iCellClickCount - 1]->UpdateLine(m_CellPositions[m_iCellClickCount - 1], m_vCurrentCellLineEndPos);
		}

		if (m_pGameInstance->MouseDown(MOUSE_KEY::RB))
		{
			FindClosetCellPos(vPickedPos);
			// B점의 Start End 점을 설정해준다.
			m_CellLines.back()->SetStartEndLinePos(m_CellPositions[m_iCellClickCount - 1], XMLoadFloat4(&vPickedPos));

			// C점 생성
			m_CellLines.push_back(CreateCellLine());

			// C점의 시작점을 m_CellPositions에 넣는다.
			m_CellPositions.push_back(XMLoadFloat4(&vPickedPos));

			// C점의 끝점을 찾는다.
			_vector vCPointEndPos = m_CellPositions[m_iCellClickCount - 2];
			_float4 vPointCEndPos;
			XMStoreFloat4(&vPointCEndPos, vCPointEndPos);
			FindClosetCellPos(vPointCEndPos);
			vCPointEndPos = XMLoadFloat4(&vPointCEndPos);

			// C점의 끝점을 찾아서 연결한다.
			m_CellLines.back()->SetStartEndLinePos(m_CellPositions.back(), vCPointEndPos);
			m_vCurrentCellLineEndPos = vCPointEndPos;
			++m_iCellClickCount;

			// true라면 내가 제대로 찍었고
			if (IsLeftHandedCross())
			{
				// 문제 없다.							
			}
			// false라면 제대로 안찍어서 좌표가 변경되었다. <- 알아서 변경해주었기 때문에 큰 문제는 없다.
			else
			{
				// 제대로 안찍어서 보정 되었다.
			}
		}
	}

	if (FIRST == (m_iCellClickCount % CL_END) || SECOND == (m_iCellClickCount % CL_END))
	{
		if (false == m_CellLines.empty())
			m_CellLines[m_iCellClickCount - 1]->UpdateLine(m_CellPositions[m_iCellClickCount - 1], m_vCurrentCellLineEndPos);
	}
}

void CLevel_Tool::RenderCellLine()
{
	for (_int i = 0; i < m_CellLines.size(); ++i)
	{
		m_pGameInstance->AddRenderObject(CRenderer::RG_UI, m_CellLines[i]);
		// m_CellLines[i]->Render();
	}
}

CCellLine* CLevel_Tool::CreateCellLine()
{
	m_pGameInstance->AddGameObjectToLayer(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_CellLine"), _uint(LEVEL_TOOL), TEXT("Layer_CellLine"), nullptr);

	m_pObj = m_pGameInstance->GetGameObjctInLayerBack(LEVEL_TOOL, LEVEL_TOOL, TEXT("Layer_CellLine"));
	if (nullptr != m_pObj)
	{
		return dynamic_cast<CCellLine*>(m_pObj);
	}
	return nullptr;
}

void CLevel_Tool::FindClosetCellPos(_float4& vSrc)
{
	_float4 vDst;

	for (_vector vPos : m_CellPositions)
	{
		XMStoreFloat4(&vDst, vPos);

		if (AlmostEqual(vSrc.x, vDst.x, 0.3f) && AlmostEqual(vSrc.y, vDst.y, 0.3f) && AlmostEqual(vSrc.z, vDst.z, 0.3f))
			vSrc = vDst;
	}
}

_bool CLevel_Tool::IsLeftHandedCross()
{
	_vector vPointA, vPointB, vPointC;
	vPointA = m_CellPositions[m_iCellClickCount - 3];
	vPointB = m_CellPositions[m_iCellClickCount - 2];
	vPointC = m_CellPositions[m_iCellClickCount - 1];

	_float4 vPA, vPB, vPC;
	XMStoreFloat4(&vPA, vPointA);
	XMStoreFloat4(&vPB, vPointB);
	XMStoreFloat4(&vPC, vPointC);

	// (A) . 
	//	   |\
	// (B) .ㅡ. (C)

	_vector vLineAB = vPointB - vPointA; // 점 B를 향하는 벡터
	_vector vLineBC = vPointC - vPointB; // 점 C를 향하는 벡터

	// 정규화를 수행한다.
	vLineAB = XMVector3Normalize(vLineAB);
	vLineBC = XMVector3Normalize(vLineBC);

	// 두 벡터를 외적하여 나온 벡터의 Y성분을 가져온다. 이때 양수라면 왼손좌표계에 맞게 ABC를 찍은 것이고 그렇지 않다면 반시계 방향으로 찍은 것이다.
	// 두 벡터의 외적은 두 벡터에 수직인 새로운 벡터를 생성, 외적의 방향은 오른손 법칙 또는 왼손 법칙에 따라 결정
	// 외적의 Y성분은 삼각형의 방향을 나타낸다. 양수인 경우 시계방향으로 감겨 있고, 음수인 경우 반시계 방향 (잘못된 경우)
	_float fCross = XMVectorGetY(XMVector3Cross(vLineAB, vLineBC));

	// 음수라면 왼손 좌표게에 맞게 좌표를 설정하지 않은 것이다.
	if (fCross <  0.f)
	{
		// B C 두 좌표를 스왑만 해준다.
		swap(m_CellPositions[m_iCellClickCount - 2], m_CellPositions[m_iCellClickCount - 1]);
		return false;
	}	

	return true;
}

HRESULT CLevel_Tool::ExportNavMesh()
{
	ofstream OutFile("../Bin/DataFiles/NavMesh.dat", ios::out | ios_base::binary);
	if (!OutFile.is_open())
		return E_FAIL;

	for (_int i = 0; i < m_CellPositions.size(); ++i)
		OutFile.write((_char*)&m_CellPositions[i], sizeof(_vector));

	OutFile.close();

	return S_OK;
}

void CLevel_Tool::ResetCellLine()
{
	m_CellPositions.clear();

	for (_int i = 0; i < m_CellLines.size(); ++i)
	{
		if (nullptr != m_CellLines[i])
		{
			m_pGameInstance->RemoveObject(LEVEL_TOOL, m_CellLines[i]);
			m_CellLines[i] = nullptr;
		}
	}

	m_CellLines.clear();

	m_iCellClickCount = 0;
}

_vector CLevel_Tool::MeshPicking(_int iScreenX, _int iScreenY, _float4& vPickPos, _float& fDistance)
{
	_vector vRayOriginPos, vRayDirection;
	Cal_RayDirAndRayOriginPos(iScreenX, iScreenY, vRayDirection, vRayOriginPos);

	if (nullptr != m_pStage01)
	{
		_vector vHitPos = m_pStage01->FindHitPos(vRayOriginPos, vRayDirection, fDistance);
		return vHitPos;
	}
}

void CLevel_Tool::RenderMonsterBonesFromXML()
{
	ImGui::Begin("MonsterBone Hierarchy");

	RenderBones(m_RootBoneMonster01, m_strMonster01);
	RenderBones(m_RootBoneMonster02, m_strMonster02);

	ImGui::End();
}

void CLevel_Tool::SaveAABBInfo()
{
	CLayer* pLayer = m_pGameInstance->FindLayer(LEVEL_TOOL, TEXT("Layer_AABB"));
	if (nullptr != pLayer)
	{
		vector<pair<_float3, _float3>> AABBs;
		list<CGameObject*> AABBList = pLayer->GetGameObjectList();
		for (auto pObj : AABBList)
		{
			_vector vPos;
			_vector vEx;
			if (nullptr != pObj->GetComponent(TEXT("Com_Transform")))
			{
				// CTransform* pT = dynamic_cast<CTransform*>(pObj->GetComponent(TEXT("Com_Transform")));
				// vPos = pT->GetTransformState(CTransform::STATE_POS);
			}

			if (nullptr != pObj->GetComponent(TEXT("Com_Collider")))
			{
				CCollider* pCol = dynamic_cast<CCollider*>(pObj->GetComponent(TEXT("Com_Collider")));
				const CBounding* pB = pCol->GetBounding();
				const CBoundingAABB* pAABB = dynamic_cast<const CBoundingAABB*>(pB);
				vEx = XMLoadFloat3(&pAABB->GetBounding()->Extents);
				vPos = XMLoadFloat3(&pAABB->GetBounding()->Center);
			}

			_float3 vAABBPos;
			XMStoreFloat3(&vAABBPos, vPos);

			_float3 vAABBEx;
			XMStoreFloat3(&vAABBEx, vEx);
			AABBs.push_back({ vAABBPos, vAABBEx });
		}
		
		// TODO

		ofstream OutFile("../Bin/DataFiles/AABB.dat", ios::out | ios_base::binary);
		if (!OutFile.is_open())
			return;
		
		_int iSize = (_int)AABBs.size();
		OutFile.write((_char*)&iSize, sizeof(_int));
		for (_int i = 0; i < AABBs.size(); ++i)
		{
			_float3 vPos = AABBs[i].first;
			_float3 vEx = AABBs[i].second;
			OutFile.write((_char*)&vPos, sizeof _float3);
			OutFile.write((_char*)&vEx, sizeof _float3);
		}

		OutFile.close();
	}
}

void CLevel_Tool::SaveCurrentSpawnTriggerInfo()
{
	_int iSpawnerCount = m_iSpawnTriggerNumMonsters;
	_int eRotType = static_cast<_int>(m_eSpawnTriggerMonsterRotType);

	ofstream OutFile("../Bin/DataFiles/SpawnTrigger.dat", ios::out | ios_base::binary);
	if (!OutFile.is_open())
	{
		MSG_BOX("SaveCurrentSpawnTriggerInfo Error!");
		return;
	}

	CSpawnTrigger* pST = static_cast<CSpawnTrigger*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_TOOL, LEVEL_TOOL, TEXT("Layer_SpawnTrigger")));
	if (false == pST->IsUsed())
	{
		pST->SetUse();
	}
	else
	{
		MSG_BOX("SaveCurrentSpawnTriggerInfo CSpawnTrigger Error!");
		return;
	}

	_vector vCenter;
	_vector vExtents;

	if (nullptr != pST->GetComponent(TEXT("Com_Collider")))
	{
		CCollider* pCol = dynamic_cast<CCollider*>(pST->GetComponent(TEXT("Com_Collider")));
		const CBounding* pB = pCol->GetBounding();
		const CBoundingAABB* pAABB = dynamic_cast<const CBoundingAABB*>(pB);
		vExtents = XMLoadFloat3(&pAABB->GetBounding()->Extents);
		vCenter = XMLoadFloat3(&pAABB->GetBounding()->Center);
	}

	// SpawnTrigger 정보를 쓴다.
	OutFile.write((_char*)&iSpawnerCount, sizeof _int);
	OutFile.write((_char*)&eRotType, sizeof _int);
	OutFile.write((_char*)&vCenter, sizeof _vector);
	OutFile.write((_char*)&vExtents, sizeof _vector);


	// Spawner 정보를 쓴다.
	CLayer* pLayer = m_pGameInstance->FindLayer(LEVEL_TOOL, TEXT("Layer_Spawner"));
	if (nullptr != pLayer)
	{
		auto& ObjList = pLayer->GetGameObjectList();

		for (auto& pObj : ObjList)
		{
			CSpawner* pSpawner = static_cast<CSpawner*>(pObj);
			if (false == pSpawner->IsUsed())
			{
				CTransform* pTranCom = static_cast<CTransform*>(pSpawner->GetComponent(TEXT("Com_Transform")));
				_vector vPos = pTranCom->GetTransformState(CTransform::STATE_POS);
				SPAWN_MONSTER_TYPE eType = pSpawner->GetSpawnMonsterType();
				_int iSpawnType = static_cast<_int>(eType);

				OutFile.write((_char*)&vPos, sizeof _vector);
				OutFile.write((_char*)&iSpawnType, sizeof _int);
				
				pSpawner->SetUse();
			}
		}
	}

	m_eSpawnTriggerMonsterRotType = ROT_TYPE::ROT_NONE;
	m_iSpawnTriggerNumMonsters = 0;

	OutFile.close();
}

void CLevel_Tool::SaveMoveEndPosInfo()
{
	_int iSpawnerCount = m_iSpawnTriggerNumMonsters;
	_int eRotType = static_cast<_int>(m_eMoveEndPosRotType);

	ofstream OutFile("../Bin/DataFiles/InterMove.dat", ios::out | ios_base::binary);
	if (!OutFile.is_open())
	{
		MSG_BOX("Save Current InterMove Error!");
		return;
	}

	CSpawnTrigger* pST = static_cast<CSpawnTrigger*>(m_pGameInstance->GetGameObjctInLayerBack(LEVEL_TOOL, LEVEL_TOOL, TEXT("Layer_InterMove")));
	if (false == pST->IsUsed())
	{
		pST->SetUse();
	}
	else
	{
		MSG_BOX("Save Current InterMove Error!");
		return;
	}

	_vector vCenter;
	_vector vExtents;

	if (nullptr != pST->GetComponent(TEXT("Com_Collider")))
	{
		CCollider* pCol = dynamic_cast<CCollider*>(pST->GetComponent(TEXT("Com_Collider")));
		const CBounding* pB = pCol->GetBounding();
		const CBoundingAABB* pAABB = dynamic_cast<const CBoundingAABB*>(pB);
		vExtents = XMLoadFloat3(&pAABB->GetBounding()->Extents);
		vCenter = XMLoadFloat3(&pAABB->GetBounding()->Center);
	}

	// SpawnTrigger 정보를 쓴다.
	OutFile.write((_char*)&iSpawnerCount, sizeof _int);
	OutFile.write((_char*)&eRotType, sizeof _int);
	OutFile.write((_char*)&vCenter, sizeof _vector);
	OutFile.write((_char*)&vExtents, sizeof _vector);


	// Spawner 정보를 쓴다.
	CLayer* pLayer = m_pGameInstance->FindLayer(LEVEL_TOOL, TEXT("Layer_InterMoveEndPos"));
	if (nullptr != pLayer)
	{
		auto& ObjList = pLayer->GetGameObjectList();

		for (auto& pObj : ObjList)
		{
			CSpawner* pSpawner = static_cast<CSpawner*>(pObj);
			if (false == pSpawner->IsUsed())
			{
				CTransform* pTranCom = static_cast<CTransform*>(pSpawner->GetComponent(TEXT("Com_Transform")));
				_vector vPos = pTranCom->GetTransformState(CTransform::STATE_POS);
				SPAWN_MONSTER_TYPE eType = pSpawner->GetSpawnMonsterType();
				_int iSpawnType = static_cast<_int>(eType);

				OutFile.write((_char*)&vPos, sizeof _vector);
				OutFile.write((_char*)&iSpawnType, sizeof _int);

				pSpawner->SetUse();
			}
		}
	}

	m_eMoveEndPosRotType = ROT_TYPE::ROT_NONE;
	m_iMoveEndPosPickingCount = 0;

	OutFile.close();
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

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	SafeRelease(m_pColFloor);
	SafeRelease(m_pColFloorBoundingBox);
	SafeRelease(m_pTempWater);
}
