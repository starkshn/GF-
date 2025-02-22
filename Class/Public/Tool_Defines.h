#pragma once

// 스레드를 사용하기 위한 헤더
#include <process.h>
#include <wtypes.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>


#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

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








#define NO_COPY(CLASSNAME)										\
			private:											\
			CLASSNAME(const CLASSNAME&) = delete;				\
			CLASSNAME& operator = (const CLASSNAME&) = delete;	\
																
#define DECLARE_SINGLETON_TOOL(CLASSNAME)						\
			NO_COPY(CLASSNAME)									\
			private:											\
			static CLASSNAME*	m_pInstance;					\
			public:												\
			static CLASSNAME*	GetInstance( void );			\
			static void DestroyInstance( void );				\

#define IMPLEMENT_SINGLETON_TOOL(CLASSNAME)						\
			CLASSNAME*	CLASSNAME::m_pInstance = NULL;			\
			CLASSNAME*	CLASSNAME::GetInstance( void )	{		\
				if(NULL == m_pInstance) {						\
					m_pInstance = new CLASSNAME;				\
				}												\
				return m_pInstance;								\
			}													\
			void CLASSNAME::DestroyInstance( void ) {			\
				if(NULL != m_pInstance)	{						\
					delete m_pInstance;							\
					m_pInstance = NULL;							\
				}												\
			}													\

