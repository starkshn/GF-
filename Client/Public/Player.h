#pragma once

#include "Client_Defines.h"
#include "StateObjectContainer.h"
#include "Model.h"
#include "PlayerBody.h"
#include "PlayerWeapon.h"

BEGIN(Engine)

class CCollider;

END


BEGIN(Client)

class CPlayerBody;
class CPlayerWeapon;

class CPlayer final : public CStateObjectContainer
{
public:
	typedef struct player_desc : public CONTAINER_OBJECT_DESC
	{

		_float fMouseSensor = {};

	} PLAYER_DESC;

public:
	enum PS
	{
		RESET = 0,
		IDLE = 0x00000001,
		WALK = STATE_IDLE << 1,
		JUMP = STATE_IDLE << 2,
		FIRE = STATE_IDLE << 3,
		RELOAD = STATE_IDLE << 4,
		PUT = STATE_IDLE << 5,
		DRAW = STATE_IDLE << 6,
		DASH = STATE_IDLE << 7,
	};

private:
	enum KEY_DOWN_DIR
	{
		LEFT = 0x000000001,
		RIGHT = LEFT << 1,
		FRONT = LEFT << 2,
		BACK = LEFT << 3,
	};

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT			InitializePrototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			PriorityUpdate(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			LateUpdate(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	void					InitHPUI();
	void					InitGunUI();

private:
	void					Keyinput(_float fTimeDelta);
	void					KeyMove(_float fTimeDelta);
	void					KeyReload(_float fTimeDelta);
	void					KeyFire(_float fTimeDelta);
	void					KeySwap(_float fTimeDelta);
	void					KeyDash(_float fTimeDelta);
	void					KeySkillMid(_float fTimeDelta);
	void					LockCursorToCenter();
	void					UpdateCurrentEndAnimation();

private:
	CPlayerBody*			GetPlayerBody();
	CPlayerWeapon*			GetPlayerWeapon();

public:
	_uint					GetCurrentModelIndex();
	_uint					GetNextDrawModelIndex() { return m_iNextDrawModelIndex; }
	_uint					GetReserveState() { return m_iReserveState; }
	void					SetReserveState(_uint iState) { m_iReserveState = iState; }

private:
	void					SetNextDrawModelIndex(_uint iNextDrawModelIndex) { m_iNextDrawModelIndex = iNextDrawModelIndex; }
	void					ReadyToPlayPutAnimation(_uint iNextDrawModelIndex);

public:
	void					SetCurrentEndAnimationName(string strCurEndAnimName) { m_strCurrentEndAnimationName = strCurEndAnimName; }
	void					SetCurrentAlertAnimationName(string strCurAlertAnimName) { m_strCurrentAlertAnimationName = strCurAlertAnimName; }

private:
	void					ActivateGunMuzzleEffect(_uint iGunFireCount);
	void					SetReserverState(_uint iState) { m_iReserveState &= RESET; m_iReserveState = iState; }
	void					ResetReserveState() { m_iReserveState &= RESET; }

public:
	void					SetResetState(_uint iState) { m_iResetState &= RESET; m_iResetState = iState; }
	_uint					GetResetState() { return m_iResetState; }

public:
	virtual void			SetDamage(_int iDamage) override;
	virtual void			UpdateHPUI() override;

	_bool					CheckMag();
	void					UpdateUIGunFire();
	void					UpdateUIReload();
	void					UpdateUIGunMagSwap(_int iNextModel);

private:
	void					UpdateDash(_float fTimeDelta);

	void					UpdateDashUI(_float fTimeDelta);
	void					UpdateDashUIEffect(_float fTimeDelta);

private:
	_uint					m_iNextDrawModelIndex = 0;
	_float					m_fMouseSensor = 1.f;
	_bool					m_bLockCursorPos = true;
	string					m_strCurrentEndAnimationName = "NONE";
	string					m_strCurrentAlertAnimationName = "NONE";
	_uint					m_iReserveState = 0;
	_uint					m_iMovePressCount = 0;
	_uint					m_iResetState = 0;

private:
	_bool					m_bStartAcc = false;
	_float					m_fAccAttackCoolTime = 0.f;
	const _float			m_fAttackCoolTimes[4] = { 0.35f, 0.4f, 0.55f, 0.6f };

private:
	_uint					m_iKeyDir = 0;

private:
	_uint					m_iDefaultFireCount = 0;
	const _uint				m_iDefaultFireCountMax = 2;

private:
	const _uint				m_iMaxHP = 200;
	string					m_strHPFontBlueNames[3] = { "UIPlayerHPBlueFirst", "UIPlayerHPBlueTwo", "UIPlayerHPBlueThree" };
	_uint					m_iFontBlueNums[3] = {};

	string					m_strHPFontGreenNames[3] = { "UIPlayerHPGreenFirst", "UIPlayerHPGreenTwo", "UIPlayerHPGreenThree" };
	_uint					m_iFontGreenNums[3] = {};

private:
	vector<_int>			m_CurMagCnt{ 9, 5, 6, 7 };		// ÇöÀç ÅºÃ¢ ÃÑ¾Ë ¼ö
	vector<_int>			m_MagCnt{ -1, 99, 99, 99 };		// ÀüÃ¼ ÅºÃ¢ ÃÑ¾Ë ¼ö
	vector<_int>			m_MaxMagCnt{9, 5, 6, 7};		// ÅºÃ¢ ÃÖ´ë ÃÑ¾Ë ¼ö
	
	string m_strBulletFonts[6] =
	{
		"UIPlayerBulletFontFirst", "UIPlayerBulletFontTwo", "UIPlayerBulletFontThree", "UIPlayerBulletFontFour", "UIPlayerBulletFontFive",
		"UIPlayerBulletFontSix"
	};

private:
	_bool					m_bDash = false;
	_int					m_iDashDir = BACK;
	_float					m_fDashTime = 0.2f;
	_float					m_fDashSP = 5.f;
	_bool					m_bDashReStart = true;

private:
	_bool					m_bSkillMid = false;

private:
	_bool					m_bStartAccFontSize = false;
	const _float			m_fSkillMidCoolTime = 3.f;
	_float					m_fAccSkillMid = 0.f;
	
private:
	_bool					m_bGetDamageCoolDone = true;

private:
	_bool					m_1508Fire = false;

private:
	virtual HRESULT			ReadyComponents() override;
	virtual HRESULT			ReadyPartObjects() override;

public:
	static CPlayer*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END