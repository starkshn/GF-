#include "Channel.h"
#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
	:
	CBase()
{
}

CChannel::~CChannel()
{

}

HRESULT CChannel::Initialize(const aiNodeAnim* pAIChannel, const CModel* pModel)
{
	strcpy_s(m_szName, pAIChannel->mNodeName.data);

	m_iBoneIndex = pModel->GetBoneIndex(m_szName);

	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

	_float3		vScale = {}, vPosition = {}; _float4	vRotation = {};

	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
	{
		KEYFRAME KeyFrame = {};

		if (i < pAIChannel->mNumScalingKeys)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.vKeyFramePosition = static_cast<_float>(pAIChannel->mScalingKeys[i].mTime);
		}

		if (i < pAIChannel->mNumRotationKeys)
		{
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
			KeyFrame.vKeyFramePosition = static_cast<_float>(pAIChannel->mRotationKeys[i].mTime);
		}

		if (i < pAIChannel->mNumPositionKeys)
		{
			memcpy(&vPosition, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.vKeyFramePosition = static_cast<_float>(pAIChannel->mPositionKeys[i].mTime);
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vPosition = vPosition;

		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

HRESULT CChannel::Initialize(AI_CHANNEL_INFO CHInfo)
{
	strcpy_s(m_szName, CHInfo.szChannelName);
	m_iBoneIndex = CHInfo.iBoneIndex;
	m_iNumKeyFrames = CHInfo.iNumKeyFrame;

	return S_OK;
}

void CChannel::UpdateTransformationMatrix(_float fCurrentTrackPosition, _uint* pKeyFrameIndex, const vector<CBone*>& Bones)
{
	// 처음재생하는 경우 fCurrentTrackPosition는 0이고 끝까지 다 재생후 처음으로 돌아오면 fCurrentTrackPosition는 0이다.
	// 따라서 pKeyFrameIndex를 0으로 셋팅해준다. (처음부터 재생하기 위해서)
	if (0.f == fCurrentTrackPosition)
		*pKeyFrameIndex = 0;

	KEYFRAME	LastKeyFrame = m_KeyFrames.back();		// 가장 뒤에 키프레임 가져온다.

	_float3		vScale, vPosition;
	_float4		vRotation;

	// fCurrentTraackPosition는 전체 트랙에서 현재 진행된 거리(시간, 이동량)을 의미한다. 아래에서 o부분에 해당함
	// |-----|-----|--o--|-----|  o 가 만약 가장 마지막 키프레임 포지션에 도달 했다면, 재생이 다 끝났다면 가장 마지막 키프레임으로 할당한다.
	if (fCurrentTrackPosition >= LastKeyFrame.vKeyFramePosition)
	{
		vScale = LastKeyFrame.vScale; vRotation = LastKeyFrame.vRotation; vPosition = LastKeyFrame.vPosition;
	}
	else
	{
		// |-----|-----|-----|-----|
		// |-----|-----|---o-k-----|	o 가 k보다 같거나 크다면 현재 키프레임을 증가 시켜주어야한다.	
		while(fCurrentTrackPosition >= m_KeyFrames[*pKeyFrameIndex + 1].vKeyFramePosition)
			++*pKeyFrameIndex;

		_float fRatio = (fCurrentTrackPosition - m_KeyFrames[*pKeyFrameIndex].vKeyFramePosition) / (m_KeyFrames[*pKeyFrameIndex + 1].vKeyFramePosition - m_KeyFrames[*pKeyFrameIndex].vKeyFramePosition);

		_float3	vS0, vS1, vP0, vP1; 
		_float4	vR0, vR1;

		vS0 = m_KeyFrames[*pKeyFrameIndex].vScale;		
		vR0 = m_KeyFrames[*pKeyFrameIndex].vRotation;		
		vP0 = m_KeyFrames[*pKeyFrameIndex].vPosition;

		vS1 = m_KeyFrames[*pKeyFrameIndex + 1].vScale;  
		vR1 = m_KeyFrames[*pKeyFrameIndex + 1].vRotation;	
		vP1 = m_KeyFrames[*pKeyFrameIndex + 1].vPosition;

		XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vS0), XMLoadFloat3(&vS1), fRatio));
		XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vR0), XMLoadFloat4(&vR1), fRatio));
		XMStoreFloat3(&vPosition, XMVectorLerp(XMLoadFloat3(&vP0), XMLoadFloat3(&vP1), fRatio));
	}

	Bones[m_iBoneIndex]->SetTransformationMatrix(XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMVectorSetW(XMLoadFloat3(&vPosition), 1.f)));
}

void CChannel::UpdateTransformationMatrix(_float fCurrentTrackPosition, _float fInterpolateTime, _uint* pKeyFrameIndex, KEYFRAME TargetKeyFrame, const vector<CBone*>& Bones)
{
	_float3		vScale, vPosition;
	_float4		vRotation;

	_float fRatio = fCurrentTrackPosition / fInterpolateTime;

	_float3	vS0, vS1, vP0, vP1;
	_float4	vR0, vR1;

	vS0 = m_KeyFrames[*pKeyFrameIndex].vScale;
	vR0 = m_KeyFrames[*pKeyFrameIndex].vRotation;
	vP0 = m_KeyFrames[*pKeyFrameIndex].vPosition;

	vS1 = TargetKeyFrame.vScale;
	vR1 = TargetKeyFrame.vRotation;
	vP1 = TargetKeyFrame.vPosition;

	XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vS0), XMLoadFloat3(&vS1), fRatio));
	XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vR0), XMLoadFloat4(&vR1), fRatio));
	XMStoreFloat3(&vPosition, XMVectorLerp(XMLoadFloat3(&vP0), XMLoadFloat3(&vP1), fRatio));

	Bones[m_iBoneIndex]->SetTransformationMatrix(XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMVectorSetW(XMLoadFloat3(&vPosition), 1.f)));
}

KEYFRAME CChannel::GetSecondKeyFrame()
{
	if (2 <= m_KeyFrames.size())
		return m_KeyFrames[1];
	else return *m_KeyFrames.begin();
}


CChannel* CChannel::Create(AI_CHANNEL_INFO CHInfo)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(CHInfo)))
	{
		MSG_BOX("Failed to Created : CChannel");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CChannel* CChannel::Create(const aiNodeAnim* pAIChannel, const CModel* pModel)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pAIChannel, pModel)))
	{
		MSG_BOX("Failed to Created : CChannel");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CChannel::Free()
{
	__super::Free();


}
