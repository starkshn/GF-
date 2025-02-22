#include "Level_Tool.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Tool_Camera.h"
#include "TempWater.h"
#include <iostream>
#include "Tool_Main_Camera.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>

CLevel_Tool::CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{

}

HRESULT CLevel_Tool::Initialize()
{
	if (FAILED(ReadyLayerMainCamera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(ReadyLayerTempWater(TEXT("Layer_TempWater"))))
		return E_FAIL;

	// 내가 만든 카메라
	// if (FAILED(Ready_Layer_Test_Camera(TEXT("Layer_Camera"))))
	// 	return E_FAIL;

	if (FAILED(ReadyLayerTerrain(TEXT("Layer_Terrain"))))
		return E_FAIL;

	Initialize_IMGUI();

	// 콘솔 창 할당
	AllocConsole();

	// 콘솔 출력을 위한 스트림 연결
	FILE* stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);  // 표준 출력을 콘솔로 리디렉션
	freopen_s(&stream, "CONOUT$", "w", stderr);  // 표준 에러 출력을 콘솔로 리디렉션

	return S_OK;
}

void CLevel_Tool::Update(const _float& fTimeDelta)
{
	
}

int value1 = 0;  // 첫 번째 정수 변수
int value2 = 0;  // 두 번째 정수 변수
CBase* pObj = nullptr;
CTempWater* pTempWater = nullptr;
CTool_Camera* pCam = nullptr;


_vector vLightDir = { 0.f, 1.f, 0.f };
float LightDirX = 0.0f;
float LightDirY = 1.0f;
float LightDirZ = 0.0f;


float AR1 = 1.0f;
float AG1 = 1.0f;
float AB1 = 1.0f;

float AMR1 = 1.0f;
float AMG1 = 1.0f;
float AMB1 = 1.0f;

HRESULT CLevel_Tool::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("Tool 레벨 입니다."));
#endif

	// ImGui 프레임 시작
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// ImGui UI 생성 (예시)
	{
		ImGui::Begin("Water");
		ImGui::Text("Input X, Y");

		// 첫 번째 정수 입력 공간
		ImGui::InputInt("Input Integer 1", &value1);

		// 두 번째 정수 입력 공간
		ImGui::InputInt("Input Integer 2", &value2);

		if (ImGui::Button("Adjust"))
		{
			ResetVtxCnt(value1, value2); // 버튼이 클릭되면 MyFunction 호출
		}

		
		ImGui::Text("Input Light Dir");
		ImGui::InputFloat("DIR X", &LightDirX);
		ImGui::InputFloat("DIR Y", &LightDirY);
		ImGui::InputFloat("DIR Z", &LightDirZ);

		if (ImGui::Button("Adjust Light"))
		{
			vLightDir = { LightDirX, LightDirY, LightDirZ };
			
			pObj = m_pGameInstance->Find_GameObjct_In_Layer(LEVEL_TOOL, TEXT("Prototype_GameObject_TempWater"), LEVEL_TOOL, TEXT("Layer_TempWater"), pObj);

			if (pObj != nullptr)
			{
				pTempWater = static_cast<CTempWater*>(pObj);

				vLightDir = XMVector3Normalize(vLightDir);
				pTempWater->Set_Light_Direction(vLightDir);
			}
		}


		ImGui::Text("Input Ambient Light Color");
		ImGui::InputFloat("R", &AR1);
		ImGui::InputFloat("G", &AG1);
		ImGui::InputFloat("B", &AB1);

		if (ImGui::Button("Adjust Ambient Light Color"))
		{
			pObj = m_pGameInstance->Find_GameObjct_In_Layer(LEVEL_TOOL, TEXT("Prototype_GameObject_TempWater"), LEVEL_TOOL, TEXT("Layer_TempWater"), pObj);

			if (pObj != nullptr)
			{
				pTempWater = static_cast<CTempWater*>(pObj);
				pTempWater->Set_Light_Diffuse({AR1, AG1, AB1});
			}
		}


		ImGui::Text("Input Ambient Material");
		ImGui::InputFloat("Ambient Material R", &AMR1);
		ImGui::InputFloat("Ambient Material G", &AMG1);
		ImGui::InputFloat("Ambient Material B", &AMB1);

		if (ImGui::Button("Adjust Ambient Material"))
		{
			pObj = m_pGameInstance->Find_GameObjct_In_Layer(LEVEL_TOOL, TEXT("Prototype_GameObject_TempWater"), LEVEL_TOOL, TEXT("Layer_TempWater"), pObj);

			if (pObj != nullptr)
			{
				pTempWater = static_cast<CTempWater*>(pObj);
				pTempWater->Set_Light_Ambient({ AMR1, AMG1, AMB1 });
			}
		}


		ImGui::End();

		pObj = m_pGameInstance->Find_GameObjct_In_Layer(LEVEL_TOOL, TEXT("Prototype_GameObject_Camera"), LEVEL_TOOL, TEXT("Layer_Camera"), pObj);
		if (pObj)
		{
			pCam = static_cast<CTool_Camera*>(pObj);
			
			if (pCam)
			{
				ImGui::Begin("Camera");
				_vector vPos = pCam->Get_Pos();
				
				_float fX = *((_float*)&vPos + 0);
				_float fY = *((_float*)&vPos + 1);
				_float fZ = *((_float*)&vPos + 2);
				_float fW = *((_float*)&vPos + 3);

				ImGui::Text("Values: %.2f, %.2f, %.2f, %.2f", fX, fY, fZ, fW);

				ImGui::End();
			}
		}
		
	}

	// ImGui 프레임 완료
	ImGui::Render();

	// 렌더 타겟을 클리어
	// ... (기존 렌더링 코드)

	// ImGui 그리기
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
	return S_OK;
}

HRESULT CLevel_Tool::ReadyLayerTempWater(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->AddGameObjectToLayer(_uint(LEVEL_TOOL), TEXT("Prototype_GameObject_TempWater"), _uint(LEVEL_TOOL), strLayerTag, nullptr))) 
		return E_FAIL;

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

	Desc.fSpeedPerSec = 10.f;
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

void CLevel_Tool::ResetVtxCnt(_int iCntX, _int iCntZ)
{
	cout << iCntX << ' ' << iCntZ << endl;

	pObj = m_pGameInstance->Find_GameObjct_In_Layer(LEVEL_TOOL, TEXT("Prototype_GameObject_TempWater"), LEVEL_TOOL, TEXT("Layer_TempWater"), pObj);
	
	if (pObj != nullptr)
	{
		pTempWater = static_cast<CTempWater*>(pObj);
		pTempWater->Reset_Vtx_Cnt(value1, value2);
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

CLevel_Tool* CLevel_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Tool* pInst = new CLevel_Tool(pDevice, pContext);

	if (FAILED(pInst->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Tool");
		Safe_Release(pInst);
	}

	return pInst;
}

void CLevel_Tool::Free()
{
	__super::Free();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
