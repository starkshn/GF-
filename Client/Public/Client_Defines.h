#pragma once

// 스레드를 사용하기 위한 헤더
#include <process.h>

namespace Client
{
	const unsigned int			g_iWinSizeX = { 1280 };
	const unsigned int			g_iWinSizeY = { 720 };

	enum LEVELID { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_BOSS, LEVEL_GLOBAL, LEVEL_END };

	enum class PLAYER_MODEL_INDEX
	{
		RIGHT_1201 = 0,
		RIGHT_1004 = 1,
		RIGHT_1503 = 2,
		RIGHT_1508 = 3,
	};

	enum class PLAYER_MODEL_ANIM_INDEX_1201
	{
		IDLE = 4,
		WALK = 5,
		JUMP = 6,
		FIRE = 0,
		RELOAD = 2,
		PUT = 7,
		DRAW = 8,
		HOME = 12,
		END_1201 = 13,
	};

	enum class PLAYER_MODEL_ANIM_INDEX_1004
	{
		IDLE = 5,
		WALK = 6,
		JUMP = 9,
		FIRE = 4,
		RELOAD = 1,
		PUT = 7,
		DRAW = 8,
		// HOME_1004 = 12,
		END_1004 = 13,
	};

	enum class PLAYER_MODEL_ANIM_INDEX_1503
	{
		IDLE = 6,
		WALK = 7,
		FIRE = 3,
		RELOAD = 1,
		PUT = 8,
		DRAW = 9,
		JUMP = 10,
		END_1503 = 13,
	};

	enum class PLAYER_MODEL_ANIM_INDEX_1508
	{
		IDLE = 12,
		WALK = 13,
		FIRE = 0,
		RELOAD = 6,
		PUT = 14,
		DRAW = 15,
		JUMP = 16,
		END_1508 = 17,
	};

	enum class PLAYER_WEAPON_PROJECTILE_TYPE
	{
		BULLET,
		ARROW,
		NONE,
	};


	enum class PROJECTILE_TYPE { PJ_DEFAULT, PJ_1004, PJ_1503_DEFAULT, PJ_1508_DEFAULT, PJ_END };

	enum class PROJECTILE_TRAIL_TYPE { PLAYER01, PLAYER02, PLAYER03, PLAYER04, MONSTER03, MONSTER04, TYPE_END };
	enum class MUZZLE_EFFECT_TYPE { PLAYER01, PLAYER02, PLAYER03, PLAYER04, TYPE_END };
	enum class BULLET_HIT_TYPE { PLAYER01, PLAYER02, PLAYER03, PLAYER04, TYPE_END};
	
}

extern HINSTANCE	g_hInst;
extern HWND			g_hWnd;

using namespace		Client;


