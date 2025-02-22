#include "Animation.h"
#include "Bone.h"
#include "Model.h"
#include "Channel.h"
#include "GlobalFuncs.h"

CAnimation::CAnimation()
    :
    CBase()
{
}

CAnimation::CAnimation(const CAnimation& Prototype)
    :
    m_iNumChannels(Prototype.m_iNumChannels),
    m_Channels(Prototype.m_Channels), // 채널 얕은 복사?
    m_fDuration(Prototype.m_fDuration),
    m_fTickPerSecond(Prototype.m_fTickPerSecond),
    m_fCurrentTrackPosition(Prototype.m_fCurrentTrackPosition),
    m_CurrentKeyFrameIndices(Prototype.m_CurrentKeyFrameIndices),
    m_ePlayType(Prototype.m_ePlayType),
    m_bIsAnimationPlaying(Prototype.m_bIsAnimationPlaying),
    m_fReplayDuration(Prototype.m_fReplayDuration),
    m_fReplayTickPerSecond(Prototype.m_fReplayTickPerSecond),
    m_bUseless(Prototype.m_bUseless)
{
    for (auto& pChannel : m_Channels)
        SafeAddRef(pChannel);

    strcpy_s(m_szName, Prototype.m_szName);
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, const CModel* pModel)
{
    strcpy_s(m_szName, pAIAnimation->mName.data);

    m_fDuration = static_cast<_float>(pAIAnimation->mDuration);                     // 현재 이 애니매이션 재생 거리(시간)
    m_fTickPerSecond = static_cast<_float>(pAIAnimation->mTicksPerSecond);          // 현재 이 애니매이션이 재생하는 속도(한 Tick당 속도를 의미한다)
    m_iNumChannels = pAIAnimation->mNumChannels;                                    // 현재 이 애니매이션이 컨트롤 해야하는(가지고 있는) 뼈(Channel)들의 개수

    m_CurrentKeyFrameIndices.resize(m_iNumChannels);                                // 채널의 개수만큼 resize해준다.

    // 채널의 개수만큼 CChannel(뼈)들을 만들어준다.
    for (_uint i = 0; i < m_iNumChannels; i++)
    {
        CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], pModel);
        if (nullptr == pChannel)
            return E_FAIL;

        m_Channels.push_back(pChannel);
    }
    return S_OK;
}

HRESULT CAnimation::Initialize(AI_ANIMATION_INFO AnimInfo)
{
    strcpy_s(m_szName, AnimInfo.szAnimName);

    m_fDuration = AnimInfo.fDuration;
    m_fTickPerSecond = AnimInfo.fTickPerSecond;
    m_iNumChannels = AnimInfo.iNumChannels;
    m_CurrentKeyFrameIndices.resize(m_iNumChannels);

    return S_OK;
}

_bool CAnimation::UpdateTransformationMatrices(const vector<CBone*>& Bones, _bool isLoop, _float fTimeDelta)
{
    switch (m_ePlayType)
    {
    case Engine::CAnimation::ONCE:
    {
        m_bIsAnimationPlaying = UpdateOnceAnimation(fTimeDelta);
    }
        break;
    case Engine::CAnimation::LOOP:
    {
        m_bIsAnimationPlaying = UpdateLoopAnimation(fTimeDelta);
    }
        break;
    case Engine::CAnimation::REPLAY_WHEN_PLAY:
    {
        m_bIsAnimationPlaying = UpdateReplayAnimation(fTimeDelta);

        if (!m_bIsAnimationPlaying) // 재생이 끝났다면
            m_ePlayType = CAnimation::ONCE;
    }
        break;
    default:
        break;
    }

    m_strAlertPointName = UpdateAlertPoints();
    m_strAlertPointSoundName = UpdateAlertPointSounds();
    
    for (_uint i = 0; i < m_iNumChannels; ++i)
        m_Channels[i]->UpdateTransformationMatrix(m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[i], Bones);

    return m_bIsAnimationPlaying;
}

// 보간이 끝났다면 false, 보간중이라면 true를 반환
_bool CAnimation::UpdateTransformationMatricesInterpolate(const vector<CBone*>& Bones, CAnimation* pTergetAnim, _float fTimeDelta, _bool bIsPlayer)
{
    m_fCurrentInterpolateTrackPosition += m_fTickPerSecond * fTimeDelta;

    m_strAlertPointName = UpdateAlertPoints();
    m_strAlertPointSoundName = UpdateAlertPointSounds();



    if (m_fCurrentInterpolateTrackPosition >= m_fInterpolateTime)
    {
        if (bIsPlayer)
        {
            m_fCurrentInterpolateTrackPosition = m_fInterpolateTime;

            if (false == strcmp(pTergetAnim->GetAnimationName().c_str(), "IDLE"))
            {
                _float fScondKeyPosition = pTergetAnim->GetChannelVector().front()->GetSecondKeyFrame().vKeyFramePosition;
                _float fNextPlayTrackPosition = fScondKeyPosition - m_fCurrentInterpolateTrackPosition;
                pTergetAnim->SetCurrentTrackPosition(fNextPlayTrackPosition + m_fCurrentInterpolateTrackPosition);
                ResetTrackPosition();
            }
            else if (false == strcmp(pTergetAnim->GetAnimationName().c_str(), "WALK"))
            {
                _float fScondKeyPosition = pTergetAnim->GetChannelVector().front()->GetSecondKeyFrame().vKeyFramePosition;
                _float fNextPlayTrackPosition = fScondKeyPosition - m_fCurrentInterpolateTrackPosition;

                pTergetAnim->SetCurrentTrackPosition(m_fCurrentInterpolateTrackPosition + 0.15f);
                ResetTrackPosition();
            }
            else if (false == strcmp(pTergetAnim->GetAnimationName().c_str(), "PUT"))
            {
                ResetTrackPosition();
                pTergetAnim->SetCurrentTrackPosition(m_fCurrentInterpolateTrackPosition);
            }
        }
        else
        {
            pTergetAnim->ResetKeyFrameIndices();
            pTergetAnim->ResetTrackPosition();
            pTergetAnim->SetCurrentTrackPosition(m_fCurrentInterpolateTrackPosition);
        }

        m_fCurrentInterpolateTrackPosition = 0.f;
        return false;
    }

    vector<CChannel*>& vecTargetChannels = pTergetAnim->GetChannelVector();
    for (_uint i = 0; i < m_Channels.size(); ++i)
    {
        _int iSrcBoneIndex = m_Channels[i]->GetBoneIndex();

        for (CChannel* pCH : vecTargetChannels)
        {
            if (pCH->GetBoneIndex() == iSrcBoneIndex)
            {
                KEYFRAME TargetKeyFrame = {};

                if (bIsPlayer)
                {
                    if ((false == strcmp(pTergetAnim->GetAnimationName().c_str(), "IDLE")) || (false == strcmp(pTergetAnim->GetAnimationName().c_str(), "WALK")) || (false == strcmp(pTergetAnim->GetAnimationName().c_str(), "PUT")))
                        TargetKeyFrame = pCH->GetSecondKeyFrame();
                    else
                        TargetKeyFrame = pCH->GetFirstKeyFrame();
                }
                else
                    TargetKeyFrame = pCH->GetFirstKeyFrame();

                m_Channels[i]->UpdateTransformationMatrix(m_fCurrentInterpolateTrackPosition, m_fInterpolateTime, &m_CurrentKeyFrameIndices[i], TargetKeyFrame, Bones);
                break;
            }
        }
    }


    return true;
}

// 처음으로 되돌린다.
_bool CAnimation::UpdateTransformationMatricesInterpolateWithSameAnim(const vector<CBone*>& Bones, CAnimation* pTergetAnim, _float fTimeDelta, _bool bIsPlayer)
{
    m_fCurrentTrackPosition -= m_fTickPerSecond * fTimeDelta * m_fFireInterPolationSP;

    if (m_fCurrentTrackPosition >= 0.f)
    {
        for (_uint i = 0; i < m_iNumChannels; ++i)
            m_Channels[i]->UpdateTransformationMatrix(m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[i], Bones);

        return true;
    }
    else
    {
        m_fCurrentTrackPosition = 0.f;
        return false;
    }
}

// Update가 끝났다면 false, Update중이라면 true를 반환
_bool CAnimation::UpdateOnceAnimation(_float fTimeDelta)
{
    m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

    /*if (m_strAnimationName == "FIRE" && m_strFire == "1004")
    {
        if (m_fCurrentTrackPosition >= 0.15f)
            return false;
    }*/

    if (m_fCurrentTrackPosition >= m_fDuration)
    {
        m_fCurrentTrackPosition = m_fDuration;
        return false;
    }

    return true;
}

// Update가 끝났다면 false, Update중이라면 true를 반환
_bool CAnimation::UpdateLoopAnimation(_float fTimeDelta)
{
    m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

    if (m_fCurrentTrackPosition >= m_fDuration)
    {
        // m_fCurrentTrackPosition = 0.f;
        ResetAnimation();
        return false;
    }

    return true;
}

// Update가 끝났다면 false, Update중이라면 true를 반환
_bool CAnimation::UpdateReplayAnimation(_float fTimeDelta)
{
    m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta * 3.f;

    if (m_fCurrentTrackPosition >= m_fDuration)
    {
        m_fCurrentTrackPosition = 0.f;
        return false;
    }

    return true;
}

string CAnimation::UpdateAlertPoints()
{
    if (0 == m_AlertPoints.size())
        return "NONE";

    auto iter = find_if(m_AlertPoints.begin(), m_AlertPoints.end(), [&](const pair<string, _float>& pair)
        {
            if (AlmostEqual(m_fCurrentTrackPosition, pair.second))
                return true;
            return false;
        });

    if (iter != m_AlertPoints.end())
        return iter->first;
    return "NONE";
}

string CAnimation::UpdateAlertPointSounds()
{
    if (0 == m_AlertPointSounds.size())
        return "NONE";

    auto iter = find_if(m_AlertPointSounds.begin(), m_AlertPointSounds.end(), [&](const pair<string, _float>& pair)
        {
            if (AlmostEqual(m_fCurrentTrackPosition, pair.second))
                return true;
            return false;
        });

    if (iter != m_AlertPointSounds.end())
        return iter->first;
    return "NONE";
}

void CAnimation::ResetKeyFrameIndices()
{
    for (_uint i = 0; i < m_iNumChannels; ++i)
        m_CurrentKeyFrameIndices[i] = 0;
}


void CAnimation::ResetAnimation()
{
    m_fCurrentTrackPosition = 0.f;

    ResetKeyFrameIndices();
}

CAnimation* CAnimation::Create(const aiAnimation* pAIAnimation, const CModel* pModel)
{
    CAnimation* pInstance = new CAnimation();

    if (FAILED(pInstance->Initialize(pAIAnimation, pModel)))
    {
        MSG_BOX("Failed to Created : CAnimation");
        SafeRelease(pInstance);
    }
    return pInstance;
}

CAnimation* CAnimation::Create(AI_ANIMATION_INFO AnimInfo)
{
    CAnimation* pInstance = new CAnimation();

    if (FAILED(pInstance->Initialize(AnimInfo)))
    {
        MSG_BOX("Failed to Created : CAnimation");
        SafeRelease(pInstance);
    }
    return pInstance;
}

CAnimation* CAnimation::Clone()
{
    return new CAnimation(*this);
}

void CAnimation::Free()
{
    __super::Free();
    
    for (auto& pChannel : m_Channels)
        SafeRelease(pChannel);

    m_Channels.clear();
}
