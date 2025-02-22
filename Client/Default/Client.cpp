// Client.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "framework.h"
#include "Client.h"
#include "MainApp.h"
#include "GameInstance.h"

#ifdef _DEBUG
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#endif

#define MAX_LOADSTRING 100

HINSTANCE   g_hInst;                               
HWND        g_hWnd;
WCHAR       szTitle[MAX_LOADSTRING];
WCHAR       szWindowClass[MAX_LOADSTRING];

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int       nCmdShow)
{

#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    CMainApp* pMainApp = nullptr;

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    pMainApp = CMainApp::Create();
    if (nullptr == pMainApp)
        return FALSE;

    CGameInstance* pGameInst = CGameInstance::GetInstance();
    if (nullptr == pGameInst)
        return FALSE;

    SafeAddRef(pGameInst);

    if (FAILED(pGameInst->AddTimer(TEXT("Timer_Default"))))
        return E_FAIL;
    if (FAILED(pGameInst->AddTimer(TEXT("Timer_60"))))
        return E_FAIL;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));

    MSG msg;

    _float fTimeAcc = 0.f;

    while (1)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (WM_QUIT == msg.message)
                break;

            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        pGameInst->UpdateTimeDelta(TEXT("Timer_Default"));
        fTimeAcc += pGameInst->GetTimeDelta(TEXT("Timer_Default"));

        if (/*fTimeAcc > 1.f / 60.f*/1)
        {
            fTimeAcc = 0.f;
            pGameInst->UpdateTimeDelta(TEXT("Timer_60"));
            pMainApp->Update(pGameInst->GetTimeDelta(TEXT("Timer_60")));
            pMainApp->Render();
        }
    }

    SafeRelease(pGameInst);
    SafeRelease(pMainApp);

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CLIENT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   // 인스턴스 핸들을 전역 변수에 저장합니다.
   g_hInst = hInstance;
   
   // AdjustWindowRect 함수란? : 위에 bar 제외하고 영역을 제대로 찝어주는 함수이다.
   // http://www.soen.kr/lecture/win32api/reference/Function/AdjustWindowRect.htm#:~:text=AdjustWindowRect%20%ED%95%A8%EC%88%98%EB%8A%94%20%EC%9C%88%EB%8F%84%EC%9A%B0%20%EC%8A%A4%ED%83%80%EC%9D%BC,%EC%98%81%EC%97%AD%EC%9D%84%20%EB%A6%AC%ED%84%B4%ED%95%B4%20%EC%A4%80%EB%8B%A4.
   RECT    rcWindowed = { 0, 0, g_iWinSizeX, g_iWinSizeY };
   AdjustWindowRect(&rcWindowed, WS_OVERLAPPEDWINDOW, TRUE);

   // HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT, 0, rcWindowed.right - rcWindowed.left, rcWindowed.bottom - rcWindowed.top, nullptr, nullptr, hInstance, nullptr);


   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   g_hWnd = hWnd;

   return TRUE;
}


#ifdef _DEBUG

// Win32 메시지 처리 함수
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

#ifdef _DEBUG

    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

#endif

    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
