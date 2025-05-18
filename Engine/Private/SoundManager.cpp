#include "SoundManager.h"


CSoundManager::CSoundManager() : m_iUpdate(0)
{
	m_pSystem = nullptr;
}



HRESULT CSoundManager::Ready_SoundMgr()
{
#ifdef _CLIENT
	// 사운드를 담당하는 대표객체를 생성하는 함수
	FMOD_System_Create(&m_pSystem);

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	FMOD_System_Init(m_pSystem, 64, FMOD_INIT_NORMAL, NULL);

	FMOD_System_CreateChannelGroup(m_pSystem, "Master", &m_pMasterGroup);

	if (FAILED(LoadSoundFile()))
	{
		MSG_BOX("Load Sound File Failed");
		return E_FAIL;
	}
	m_eID = SOUND_EFFECT0;
#endif

	return S_OK;
}



void CSoundManager::PlayEffect(const TCHAR* pSoundKey, float fVolume)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE;

	if (FMOD_Channel_IsPlaying(m_pChannelArr[m_eID], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, m_pMasterGroup, FALSE, &m_pChannelArr[m_eID]);
	}

	FMOD_Channel_SetVolume(m_pChannelArr[m_eID], fVolume);

	FMOD_System_Update(m_pSystem);

	int iCID = (int)m_eID + 1;
	if (iCID == SOUND0)
		iCID = 0;
	m_eID = (CHANNELID)iCID;
}

void CSoundManager::PlayBGM(const TCHAR* pSoundKey, float fVolume)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem, iter->second, m_pMasterGroup, FALSE, &m_pChannelArr[SOUND_BGM]);
	FMOD_Channel_SetMode(m_pChannelArr[SOUND_BGM], FMOD_LOOP_NORMAL);
	FMOD_Channel_SetVolume(m_pChannelArr[SOUND_BGM], fVolume);
	FMOD_System_Update(m_pSystem);
	m_fBGMVolume = fVolume;
	m_iUpdate = 0;
}

void CSoundManager::PlayBGM(const TCHAR* pSoundKey, float fVolume, int iUpdate)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem, iter->second, m_pMasterGroup, FALSE, &m_pChannelArr[SOUND_BGM]);
	FMOD_Channel_SetMode(m_pChannelArr[SOUND_BGM], FMOD_LOOP_NORMAL);
	FMOD_Channel_SetVolume(m_pChannelArr[SOUND_BGM], fVolume);
	FMOD_System_Update(m_pSystem);
	m_fBGMVolume = fVolume;
	m_iUpdate = iUpdate;
}

_uint CSoundManager::Get_NowEffectID()
{
	_int iNowID = (int)m_eID-1;
	if (iNowID < 0)
		iNowID = (int)SOUND0 - 1;
	return iNowID;
}

void CSoundManager::Resume()
{
	m_iUpdate = 1;
	FMOD_BOOL bPlay = FALSE;

	FMOD_Channel_SetPaused(m_pChannelArr[SOUND_BGM], false);

}

void CSoundManager::Pause()
{
	m_iUpdate = 2;
}

void CSoundManager::StopSound(_uint eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void CSoundManager::StopBGM()
{
	FMOD_Channel_Stop(m_pChannelArr[SOUND_BGM]);

}

void CSoundManager::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);
}


void CSoundManager::SetBGMVolume(float fVolume)
{
	FMOD_Channel_SetVolume(m_pChannelArr[SOUND_BGM], fVolume);

	FMOD_System_Update(m_pSystem);
}

void CSoundManager::SetBGMPitch(float fPitch)
{
	FMOD_Channel_SetPitch(m_pChannelArr[SOUND_BGM], fPitch);
}

void CSoundManager::UpdateBGM(const _float& fTimeDelta, const _float& fSpeed)
{
	FMOD_BOOL bPlay(false);

	if (m_iUpdate)
	{
		if (1 == m_iUpdate)
		{
			m_fBGMVolume += fTimeDelta * fSpeed;
			if (0.6f < m_fBGMVolume)
			{
				m_fBGMVolume = 0.6f;
				m_iUpdate = 0;
			}
			FMOD_Channel_SetVolume(m_pChannelArr[SOUND_BGM], m_fBGMVolume);
			FMOD_System_Update(m_pSystem);

		}
		else
		{
			m_fBGMVolume -= fTimeDelta * fSpeed;
			if (0.f > m_fBGMVolume)
			{
				m_fBGMVolume = 0.f;
				m_iUpdate = 0;
				FMOD_Channel_SetPaused(m_pChannelArr[SOUND_BGM], false);

			}
			else
			{
				FMOD_Channel_SetVolume(m_pChannelArr[SOUND_BGM], m_fBGMVolume);
				FMOD_System_Update(m_pSystem);
			}

		}
	}

}

void CSoundManager::SetBGM(int iUpdate)
{
	m_iUpdate = iUpdate;
}

HRESULT CSoundManager::LoadSoundFile()
{
	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	__finddata64_t fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	intptr_t  handle = _findfirsti64("../Bin/Resources/Sounds/*.*", &fd);

	if (handle == -1)
		return E_FAIL;

	int iResult = 0;

	char szCurPath[128] = "../Bin/Resources/Sounds/";	 // 상대 경로
	char szFullPath[128] = "";


	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);

		// "../Sound/" + "Success.wav"
		strcat_s(szFullPath, fd.name);
		// "../Sound/Success.wav"

		FMOD_SOUND* pSound = nullptr;

		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			int iLength = strlen(fd.name) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

			m_mapSound.emplace(pSoundKey, pSound);
		}
		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnexti64(handle, &fd);
	}

	FMOD_System_Update(m_pSystem);

	_findclose(handle);
	return S_OK;
}

CSoundManager* CSoundManager::Create()
{
	CSoundManager* pInstance = new CSoundManager();

	if (FAILED(pInstance->Ready_SoundMgr()))
	{
		MSG_BOX("Failed to Created : CSoundManager");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CSoundManager::Free()
{
	for (auto& Mypair : m_mapSound)
	{
		delete[] Mypair.first;
		FMOD_Sound_Release(Mypair.second);
	}
	m_mapSound.clear();

	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}