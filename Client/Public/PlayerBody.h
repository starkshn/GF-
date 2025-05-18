#pragma once
#include "Client_Defines.h"
#include "StateObjectBody.h"
#include "Model.h"

BEGIN(Engine)
class CShader;
END

BEGIN(Client)

class CPlayer;

class CPlayerBody final : public CStateObjectBody
{

private:
	CPlayerBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerBody(const CPlayerBody& Prototype);
	virtual ~CPlayerBody() = default;

public:
	virtual HRESULT				InitializePrototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				PriorityUpdate(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				LateUpdate(_float fTimeDelta) override;
	virtual HRESULT				Render() override;
	virtual HRESULT				RenderShadow() override;

private:
	HRESULT						LoadPlayerAnimations();								// .anim파일들 로드하는 함수
	HRESULT						SetupPlayerModelFristTransformationMatrices();
	HRESULT						SetupPlayerModelFirstAnimationsState();

private:
	virtual void				SetPlayAnimation(_uint iState) override;	void						ResetCurrentEndAnimName() { m_strCurrentEndAnimName = "NONE"; }
	virtual void				AnimationEnd(_uint iCurrentModelIndex) override;

private:
	virtual void				ChangeAnimationByAnimIndex(_uint iFrom, _uint iTo, _uint iModelIndex = 0) override;
	virtual void				ChangeAnimationImmediatelyByAnimIndex(_uint iTo, _uint iCurrentModelIndex = 0) override { m_pModelComs[iCurrentModelIndex]->SetCurrentAnimationIndex(iTo); }
	virtual void				ChangeAnimationInterpolationByAnimIndex(_uint iFrom, _uint iTo, _uint iCurrentModelIndex = 0) override;
	void						ChangeAnimationWhenFireEnd(_uint iFrom, _uint iTo, _uint iModelIndex);
	void						ChangeAnimationWhenPutEnd(_uint iFrom, _uint iTo, _uint iModelIndex);
	void						ChangeAnimationWhenDrawEnd(_uint iFrom, _uint iTo, _uint iModelIndex);

private:
	_uint						GetAnimIndex(string strAnimName, _uint iModelIndex);

public:
	_int						GetCurrentPlayingAnimIndex() { return m_pModelComs[GetCurrentModelIndex()]->GetCurrentAnimationIndex(); }
	const _float4x4*			GetModelSocketMatrix(string strSocketBoneName, _uint iModelIndex);

private:
	_bool						CanChangeAnimation();

	virtual void				FindAlertPoint(_uint iCurState, _uint iState, string strCompAnimName, string AlertPointName) override;


private:
	void						CheckAlertPointSoundName() override;
	
private:
	CPlayer*					m_pPlayer = nullptr;

private:
	_uint						m_iPrevTickState = 1;
	string						m_strCurrentEndAnimName = "NONE";


	_uint						m_iCurAnimIndex = 0;
	_int						m_iInterpolationIndex = 0;

	_float						m_vTempAccTime = 0.f;

private:
	virtual HRESULT				ReadyComponents() override;
	virtual HRESULT				BindShaderResources() override;

public:
	static CPlayerBody*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END
