#ifndef InputDev_h__
#define InputDev_h__

#include "Base.h"

BEGIN(Engine)

enum class KEY_STATE
{
	NONE,			// 이전에도 눌리지 않았고, 현재도 눌리지 않은.
	DOWN,			// 막 누른 시점.
	PRESSING,		// 누르고 있는.
	UP,				// 막 뗀 시점.
};

// test

enum class MOUSE_KEY { LB, RB, MB, LAST, };
enum class MOUSE_MOVE { X, Y, Z, LAST, };

enum class KEY // 자체적으로 지원 할 키 종류
{
	LEFT,
	RIGHT,
	UP,
	DOWN,
	NUM1,
	NUM2,
	NUM3,
	NUM4,
	NUM5,
	NUM6,
	NUM7,
	NUM8,
	NUM9,
	NUM0,
	NUMPAD8,
	NUMPAD4,
	NUMPAD5,
	NUMPAD6,
	NUMPAD2,
	Q,
	W,
	E,
	R,
	T,
	Y,
	U,
	H,
	J,
	K,
	M,
	I,
	O,
	P,
	A,
	S,
	D,
	F,
	G,
	Z,
	X,
	C,
	V,
	B,
	ALT,
	CTRL,
	LSHIFT,
	SPACE,
	ENTER,
	ESC,
	TAB,
	LAST, // 끝을 알리는.
};

// 키 정보를 담을 구조체
struct tKeyInfo
{
	KEY_STATE   eState;		// 키의 상태값
	bool		bPrevPush;	// 이전 프레임에서 눌렸는지 여부.
};




// KeyMgr
#define KEY_CHECK(key, state)   Engine::Get_KeyState(key) == state
#define KEY_PRESSING(key)       KEY_CHECK(key, KEY_STATE::PRESSING)
#define KEY_DOWN(key)           KEY_CHECK(key, KEY_STATE::DOWN)
#define KEY_UP(key)				KEY_CHECK(key, KEY_STATE::UP)
#define KEY_NONE(key)           KEY_CHECK(key, KEY_STATE::NONE)

#define MOUSE_CHECK(key, state)	Engine::Get_MouseKeyState(key) == state
#define MOUSE_PRESSING(key)		MOUSE_CHECK(key, KEY_STATE::PRESSING)
#define MOUSE_DOWN(key)			MOUSE_CHECK(key, KEY_STATE::DOWN)
#define MOUSE_UP(key)			MOUSE_CHECK(key, KEY_STATE::UP)
#define MOUSE_NONE(key)			MOUSE_CHECK(key, KEY_STATE::NONE)

#define MOUSE_MOVE(key)			Engine::Get_DIMouseMove(key)

class CInput_Device : public CBase
{
private:
	CInput_Device(void);
	virtual ~CInput_Device(void) = default;
	
public:

	// Get
	const KEY_STATE& GetKeyState(KEY _eKEY) // 키입력 동기화 o 
	{
		return m_vecKey[(int)_eKEY].eState;
	}

	const KEY_STATE& GetMouseKeyState(MOUSE_KEY _eMouse)
	{
		return m_arrMouseKey[(int)_eMouse].eState;
	}

	_long	Get_DIMouseMove(MOUSE_MOVE eMouseMove)
	{
		return *(((_long*)&m_tMouseState) + (int)eMouseMove);
	}

	_byte	Get_DIKeyState(_ubyte byKeyID)		
	{ 
		return m_byKeyState[byKeyID]; 
	}

	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse) 
	{ 	
		return m_tMouseState.rgbButtons[eMouse]; 	
	}

	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)	
	{	
		return *(((_long*)&m_tMouseState) + eMouseState);	
	}
	
public:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
	void	Update_InputDev(void);

private:
	LPDIRECTINPUT8			m_pInputSDK = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pMouse = { nullptr };

private:
	tKeyInfo				m_arrMouseKey[(int)MOUSE_KEY::LAST];
	_byte					m_byKeyState[256] = {};		// 키보드에 있는 모든 키값을 저장하기 위한 변수
	DIMOUSESTATE			m_tMouseState = {};

private:
	vector<tKeyInfo> m_vecKey; // 키 인포 타입을 저장 할 가변배열. // 벡터의 인덱스번호가 해당 키값.


public:
	static CInput_Device*	Create(HINSTANCE hInst, HWND hWnd);
	virtual void			Free(void);

};
END
#endif // InputDev_h__
