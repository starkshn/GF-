#pragma once
#include "Base.h"
#include <io.h>
#include "fmod.h"
#include "fmod.hpp"
#pragma comment(lib, "fmodL_vc.lib")

BEGIN(Engine)
class ENGINE_DLL CSoundManager : public CBase
{
	
public:

	enum CHANNELID
	{
		SOUND_EFFECT0,
		SOUND_EFFECT1,
		SOUND_EFFECT2,
		SOUND_EFFECT3,
		SOUND_EFFECT4,
		SOUND_EFFECT5,
		SOUND_EFFECT6,
		SOUND_EFFECT7,
		SOUND_EFFECT8,
		SOUND_EFFECT9,
		SOUND_EFFECT10,
		SOUND_EFFECT11,
		SOUND_EFFECT12,
		SOUND_EFFECT13,
		SOUND_EFFECT14,
		SOUND_EFFECT15,
		SOUND_EFFECT16,
		SOUND_EFFECT17,
		SOUND_EFFECT18,
		SOUND_EFFECT19,
		SOUND_EFFECT20,
		SOUND_EFFECT21,
		SOUND_EFFECT22,
		SOUND_EFFECT23,
		SOUND_EFFECT24,
		SOUND_EFFECT25,
		SOUND_EFFECT26,
		SOUND_EFFECT27,
		SOUND_EFFECT28,
		SOUND_EFFECT29,
		SOUND_EFFECT30,

		// 불가침 영역
		SOUND0,
		SOUND1,
		SOUND2,
		SOUND3,
		SOUND4,
		SOUND5,
		SOUND6,
		SOUND7,
		SOUND8,
		SOUND9,
		SOUND10,

		SOUND_BGM,
		MAXCHANNEL
	};

private:
	explicit CSoundManager();
	virtual ~CSoundManager()=default;

public:
	HRESULT Ready_SoundMgr();

public:
	void PlayEffect(const TCHAR* pSoundKey, float fVolume);
	void PlayBGM(const TCHAR* pSoundKey, float fVolume);
	void PlayBGM(const TCHAR* pSoundKey, float fVolume, int iUpdate);
	//void Play_Sound(const TCHAR* pSoundKey, _uint eID, float fVolume);
	void StopSound(_uint eID);
	_uint Get_NowEffectID();

	void Resume();
	void Pause();
	void StopBGM();
	void StopAll();
	//void SetChannelVolume(CHANNELID eID, float fVolume);
	void SetBGMVolume(float fVolume);
	void SetBGMPitch(float fPitch);
	void UpdateBGM(const _float& fTimeDelta, const _float& fSpeed);
	void SetBGM(int iUpdate);


private:
	HRESULT LoadSoundFile();

private:
	_int					 m_iUpdate = {};
	float					 m_fBGMVolume = {};
	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD_SOUND*> m_mapSound;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];

	FMOD_CHANNELGROUP* m_pMasterGroup = {};

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pSystem = {};

	CHANNELID m_eID = {};

public:
	static CSoundManager* Create();
	virtual void Free();
};
END
