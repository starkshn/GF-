#pragma once
#include "Base.h"


// 1. 이 뼈가 소속된 애니매이션을 표현하기 위해서는 뼈는 어떤 상태(Key Frame)(행렬)들을 가지는가? 에 대한 정보를 가진다.
// 2. 현재 재생시간에 맞는 뼈의 상태 행렬을 만들어 낸다.
// 3. 같은 이름을 가진 채널이 다수 존재할 수 있다.

BEGIN(Engine)

class CModel;
class CBone;

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() override;

public:
	HRESULT				Initialize(const aiNodeAnim* pAIChannel, const CModel* pModel);
	HRESULT				Initialize(AI_CHANNEL_INFO CHInfo);
	void				UpdateTransformationMatrix(_float fCurrentTraackPosition, _uint* pKeyFrameIndex, const vector<CBone*>& Bones);
	void				UpdateTransformationMatrix(_float fCurrentTraackPosition, _float fInterpolateTime, _uint* pKeyFrameIndex, KEYFRAME TargetKeyFrame, const vector<CBone*>& Bones);

public:
	void				AddKeyFrame(KEYFRAME Keyframe) { m_KeyFrames.push_back(Keyframe); }
	vector<KEYFRAME>&	GetKeyFrameVector() { return m_KeyFrames; }
	KEYFRAME			GetFirstKeyFrame() { return m_KeyFrames.front(); }
	KEYFRAME			GetSecondKeyFrame();
	_uint				GetBoneIndex() { return m_iBoneIndex; }
	const _char*		GetChannelName() { return m_szName; }
	
private:
	_char				m_szName[MAX_PATH] = {};
	_uint				m_iNumKeyFrames = {};
	vector<KEYFRAME>	m_KeyFrames;
	_uint				m_iBoneIndex = {};

public:
	static	CChannel*	Create(AI_CHANNEL_INFO CHInfo);
	static	CChannel*	Create(const aiNodeAnim* pAIChannel, const CModel* pModel);
	virtual void		Free() override;

};

END