#pragma once
#include "Base.h"


// 1. 이 애니매이션은(IDLE, ATT etc..)이 컨트롤 해야하는 뼈(Channel)의 정보를 가진다.

BEGIN(Engine)

class CModel;
class CBone;
class CChannel;

class CAnimation final : public CBase
{
public:
	enum PLAY_TYPE { ONCE, LOOP, REPLAY_WHEN_PLAY, REPLAY};

private:
	CAnimation();
	CAnimation(const CAnimation& Prototype);
	virtual ~CAnimation() override = default;

public:
	HRESULT						Initialize(const aiAnimation* pAIAnimation, const CModel* pModel);
	HRESULT						Initialize(AI_ANIMATION_INFO AnimInfo);
	_bool						UpdateTransformationMatrices(const vector<CBone*>& Bones, _bool isLoop, _float fTimeDelta);
	_bool						UpdateTransformationMatricesInterpolate(const vector<CBone*>& Bones, CAnimation* pTergetAnim, _float fTimeDelta, _bool bIsPlayer = false);
	_bool						UpdateTransformationMatricesInterpolateWithSameAnim(const vector<CBone*>& Bones, CAnimation* pTergetAnim, _float fTimeDelta, _bool bIsPlayer = false);

private:
	_bool						UpdateOnceAnimation(_float fTimeDelta);
	_bool						UpdateLoopAnimation(_float fTimeDelta);
	_bool						UpdateReplayAnimation(_float fTimeDelta);
	string						UpdateAlertPoints();
	string						UpdateAlertPointSounds();

private:
	void						ResetKeyFrameIndices();

public:
	void						InsertAlertPoint(string strAlertKey, _float fAlertTime) 
	{ 
		m_AlertPoints.insert({ strAlertKey, fAlertTime }); 
	}

	void						InsertAlertPointSound(string strAlertKey, _float fAlertTime)
	{
		m_AlertPointSounds.insert({ strAlertKey, fAlertTime });
	}
	
public:
	// TrackPositon을 0으로, 모든 채널들의 KeyFrameIndex들을 0으로 설정. (리셋한다)
	void						ResetAnimation();
	void						ResetTrackPosition() { m_fCurrentTrackPosition = 0.f; }

public:
	void						AddKeyFrameIndices(_uint val) { m_CurrentKeyFrameIndices.push_back(val); }

public:
	string						GetAnimationName() { return m_strAnimationName; }
	vector<CChannel*>&			GetChannelVector() { return m_Channels; }
	vector<_uint>&				GetKeyframeIndices() { return m_CurrentKeyFrameIndices; }
	string						GetAlertPointName()  { return m_strAlertPointName; }

	void						SetInterpolationTime(_float fInterpolationTime) { m_fInterpolateTime = fInterpolationTime; }
	void						SetPlayType(PLAY_TYPE eType) { m_ePlayType = eType; }
	void						SetUselessAnim() { m_bUseless = true; }
	void						SetAnimName(string strName) { m_strAnimationName = strName; }
	void						SetCurrentTrackPosition(_float fTrackPosition) { m_fCurrentTrackPosition = fTrackPosition; }

public:
	_bool						IsUseless() { return m_bUseless; }
	_bool						IsAnimationPlaying() 
	{ 
		return m_bIsAnimationPlaying; 
	}

public:
	void						SetFireName(string strName) { m_strFire = strName; }
	void						SetFireInterpolationSP(_float fSP) { m_fFireInterPolationSP = fSP; }


public:
	string						GetAlertPointSoundName() { return m_strAlertPointSoundName; }

private:
	_char						m_szName[MAX_PATH] = {};
	_uint						m_iNumChannels = {};
	vector<CChannel*>			m_Channels;

	_float						m_fDuration = {};
	_float						m_fTickPerSecond = {};
	_float						m_fPrevTickPerSecond = {};
	_float						m_fCurrentTrackPosition = {};

	vector<_uint>				m_CurrentKeyFrameIndices;		// CAnimation을 깊은 복사 할 것이기 때문에 애니매이션이 가지고 있는 Channel들을 각각 다른 Animation객체에서 관리를 위해 Channel별 KeyFrameIndex들을 애니매이션이 관리해준다.

	PLAY_TYPE					m_ePlayType = PLAY_TYPE::ONCE;
	_bool						m_bIsAnimationPlaying = false;
	const _float				m_fReplayDuration = 0.5f;		// 현재 애니매이션 반복 재생시 duration
	const _float				m_fReplayTickPerSecond = 2.f;	// 현재 애니매이션 반복 재생시 TickPerSecond

	_float						m_fInterpolateTime = 0.2f;
	_float						m_fCurrentInterpolateTrackPosition = 0.f;
	string						m_strAnimationName;

	map<string, _float>			m_AlertPoints;
	string						m_strAlertPointName = "NONE";

	map<string, _float>			m_AlertPointSounds;
	string						m_strAlertPointSoundName = "NONE";

private:
	_bool						m_bUseless = false;
	string						m_strFire = "NONE";
	_float						m_fFireInterPolationSP = 1.f;

public:
	static CAnimation*		Create(const aiAnimation* pAIAnimations, const CModel* pModel);
	static CAnimation*		Create(AI_ANIMATION_INFO AnimInfo);
	virtual CAnimation*		Clone();
	virtual void			Free() override;
};

END