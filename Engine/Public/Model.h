#pragma once
#include "Component.h"
#include "Animation.h"

BEGIN(Engine)

class CMesh;
class CShader;
class CBone;

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END, };

	

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	virtual	HRESULT			InitializePrototype(TYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix, const vector<_int>& UselessAnimIndex, _bool bisPlayer = false, _bool bReadFromFile = false, _bool bReverseLoad = false, string strName = "");
	virtual HRESULT			Initialize(void* pArg) override;
	HRESULT					Render(_uint iMeshIndex);
	_bool					PlayAnimation(_float fTimeDelta, _bool bIsPlayer = false); // 현재 재생중인지 아닌지를 반환한다.

private:
	_bool					InterpolateAnimation(_float fTimeDelta, _bool bIsPlayer = false); // 애니매이션 선형보간 함수

public:
	HRESULT					LoadDatFile(const _char* pModelFilePath, TYPE eModelType, const vector<_int>& UselessAnimIndex, _fmatrix PreTransformMatrix, _bool bIsPlayer = false, _bool bReverseLoad = false);
	HRESULT					LoadAnimFile(LOADANIMTYPE eType, string strModelNameFrom);


	// Biding Resources
public:
	HRESULT					BindBoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT					BindMaterial(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eType, _uint iTextureIndex = 0);


	// Model
public:
	TYPE					GetModelType() { return m_eModelType; }
	string					GetModelName() { return m_strModelName; }

	// Bones
public:
	_uint					GetBoneIndex(const _char* pBoneName) const;
	const _float4x4*		GetBoneCombinedMatrix(const _char* pTargetBoneName) const;
	CBone*					GetBone(const _char* pTargetBoneName);
	

	// Meshes
public:
	_uint					GetNumMeshes() const { return m_iNumMeshes; }


	// Animations
public:
	void					ResetInterpolation() { m_bInterpolateAnimation = false; }
	void					ResetAnimationTrackPosition(_uint iAnimationIndex) { m_Animations[iAnimationIndex]->ResetTrackPosition(); }
	void					ResetCurrentAnimation() { m_Animations[m_iCurrentAnimIndex]->ResetAnimation(); }	// 애니매이션 재생시 CurTrackPos를 처음으로 보낸다.
	void					ResetAnimation(_uint iIndex) { m_Animations[iIndex]->ResetAnimation(); }	// 애니매이션 재생시 CurTrackPos를 처음으로 보낸다.

	void					SetTransformationMatriciesFirstTime(_uint iTargetAnimationIndex, _float fTimeDelta = 0.1f);					// 겜 처음 시작시 모델이 뭉그러지는 것을 방지하는 함수
	void					SetupAnimation(_uint iAnimIndex, CAnimation::PLAY_TYPE eType) { m_iCurrentAnimIndex = iAnimIndex; m_Animations[m_iCurrentAnimIndex]->SetPlayType(eType); }
	void					SetAnimationName(_uint iAnimIndex, string strName);
	void					SetCurrentAnimationIndex(_int iAnimIndex);
	void					SetAnimationPlayType(CAnimation::PLAY_TYPE eType) { m_Animations[m_iCurrentAnimIndex]->SetPlayType(eType); }
	void					SetInterpolationAnimationIndex(_int iFrom, _int iTo);

	CAnimation*				GetAnimation(_int iAnimIndex);
	string					GetAnimationName(_uint iAnimIndex) { return m_Animations[iAnimIndex]->GetAnimationName(); }
	_int					GetNumAnimations() { return m_iNumAnimations; }
	_int					GetCurrentAnimationIndex() { return m_iCurrentAnimIndex; }
	_int					GetNextValidAnimationIndex();	// 유효한 다음 애니매이션 인덱스 반환
	_int					GetPrevValidAnimationIndex();	// 유요한 이전 애니매이션 인덱스 반환
	const vector<CBone*>&	GetBoneVector() { return m_Bones; }

	_bool					IsAnimationPlaying() 
	{ 
		return m_Animations[m_iCurrentAnimIndex]->IsAnimationPlaying(); 
	}

public:
	void					SetLockAnim()  { m_bLockAnimation = true; }
	void					ReleaseLockAnim();

public:
	// Map Tool에서 NavMesh찍을대 사용하는 함수
	_vector					FindHitPos(_fvector vOriginPos, _fvector vRayDir, _float fDist, _matrix WorldMatrix);

public:
	string					GetAlertPointSoundName() { return m_Animations[m_iCurrentAnimIndex]->GetAlertPointSoundName(); }

public:
	void					SetInterpolationWithSameAnim();
	_bool					InterpolationwithSameAnim = false;
	
private:
	Assimp::Importer		m_pImporter;
	const aiScene*			m_pAIScene = nullptr;
	TYPE					m_eModelType = TYPE_END;
	string					m_strModelName;

private:
	vector<CBone*>			m_Bones;
	vector<CBone*>			m_TempOriginBones;

private:
	_uint					m_iNumMeshes = 0;
	vector<CMesh*>			m_Meshes;

private:
	_uint												m_iNumMaterials = {};
	vector<vector<vector<ID3D11ShaderResourceView*>>>	m_Materials;

private:
	_int					m_iCurrentAnimIndex = {};
	_int					m_iNumAnimations = {};
	vector<CAnimation*>		m_Animations;
	_bool					m_isLoop = false;
	_bool					m_isFinisedAnim = false;

	_bool					m_bIsPlayingAnim = false;
	_bool					m_bIsInterpolationing = false;

private:
	_float4x4				m_PreTransformMatrix = {};		// 모델을 양의 Z축으로 돌리기 위한 행렬

private:
	_bool					m_bInterpolateAnimation = false;
	_int					m_iInterpolateFromIndex = 0;
	_int					m_iInterpolateToIndex = 0;

private:
	_bool					m_bLockAnimation = false;

private:
	HRESULT					ReadyMeshes();
	HRESULT					ReadyMaterials(const _char* pModelFilePath, _bool bIsPlayer = false);
	HRESULT					ReadyBones(const aiNode* pNode, _int iParentBoneIndex);
	HRESULT					ReadyAnimations();

public:
	static CModel*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix, const vector<_int>& UselessAnimIndex, _bool bIsPlayer = false, _bool bReadFromFile = false, _bool bReverseLoad = false, string strName = "");
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

END