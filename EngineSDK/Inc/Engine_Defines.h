#pragma once

#pragma warning (disable : 4251)

#include <d3d11.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

using namespace DirectX;

#include "Effects11\d3dx11effect.h"
#include "d3dcompiler.h"


#include "DirectXTK\DDSTextureLoader.h"
#include "DirectXTK\WICTextureLoader.h"
#include "DirectXTK\ScreenGrab.h"


// Collider그리기 위한 헤더 (241029)
#include "DirectXTK\PrimitiveBatch.h"
#include "DirectXTK\VertexTypes.h"
#include "DirectXTK\Effects.h"
#include "DirectXTK\SpriteBatch.h"
#include "DirectXTK\SpriteFont.h"


// assimp 헤더
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"


namespace Engine
{
	enum class PROTOTYPE { PROTO_GAMEOBJ, PROTO_COMPONENT };

	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };
	enum CAMERA_STATE{ RIGHT, UP, LOOK, POS};


	enum LOADANIMTYPE { LOADANIM_IDLE, LOADANIM_WALK, LOADANIM_PUT, LOADANIM_DRAW, LOADANIM_JUMP, LOADANIM_HOME, LOADANIM_END, };


	enum SPAWN_MONSTER_TYPE
	{
		MONSTER01,		// 기본 병사 몬스터
		MONSTER02,		// 빨간 창
		MONSTER03,		// 화살
		MONSTER04,		// 노랑이
		MONSTER05,		// 말
		MONSTER06,		// 기본 창 쓰는애

	};

	enum class UI_SHOW_TYPE
	{
		UI_ORTHO, // 직교
		UI_PERSPECTIVE,	// 원근
		UI_END,
	};

	enum class UI_TYPE
	{
		UI_PANEL,
		UI_TEX,
		UI_FONT,
		UI_BAR,
		UI_DAMAGE_FONT,
		UI_END,
	};

	//const unsigned int g_iMaxWidth	= 1280;
	//const unsigned int g_iMaxHeight = 720;

	const unsigned int g_iMaxWidth = 8192;
	const unsigned int g_iMaxHeight = 4608;

}


#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <fstream>
#include <iostream>
using namespace std;

#include "Engine_Typedef.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"

using namespace Engine;

#define ZERO(Src, iSize) ZeroMemory(Src, iSize)

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW

#endif

#endif // _DEBUG



