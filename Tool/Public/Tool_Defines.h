#pragma once

// 스레드를 사용하기 위한 헤더
#include <process.h>
#include <wtypes.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

namespace Tool
{
	const unsigned int			g_iWinSizeX = { 1280};
	const unsigned int			g_iWinSizeY = { 720 };

	enum LEVELID { LEVEL_STATIC, LEVEL_LOADING, LEVEL_TOOL, LEVEL_END };
}

extern HINSTANCE	g_hInst;
extern HWND			g_hWnd;

using namespace		Tool;

