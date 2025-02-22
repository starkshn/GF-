#pragma once
#include "StateObjectBody.h"
#include "Model.h"
#include "MonsterBase.h"

BEGIN(Engine)

class CShader;
class CTexture;

END

BEGIN(Client)

class CMonsterBodyBase : public CStateObjectBody
{
protected:
	CMonsterBodyBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterBodyBase(const CMonsterBodyBase& Prototype);
	virtual ~CMonsterBodyBase() = default;

public:
	virtual HRESULT				InitializePrototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				PriorityUpdate(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				LateUpdate(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

protected:
	virtual void				InitializeAnimNameAndIndex() {};

protected:
	virtual void				SetPlayAnimation(_uint iState) final;
	virtual void				ChangeAnimation(_uint iState, _uint iCurrentModelIndex = 0) final;
	virtual void				ChangeAnimation(_uint iFrom, _uint iTo, _uint iCurrentModelIndex = 0) final;
	virtual void				ChangeAnimationByAnimIndex(_uint iFrom, _uint iTo, _uint iCurrentModelIndex = 0) override;
	virtual void				AnimationEnd(_uint iCurrentModelIndex = 0) {};

protected:
	virtual void				ChangeAnimationWhenEnd(_uint iCurrentModelIndex = 0) final;
	virtual void				ChangeAnimationWhenPlaying(_uint iState, _uint iCurrentModelIndex = 0) final;
	virtual void				ChangeAnimationImmediately(_uint iState, _uint iCurrentModelIndex = 0) final;
	virtual void				ChangeAnimationInterpolation(_uint iCurrentModelIndex = 0) final;

public:
	virtual string				GetAnimationNameByState(_uint iState) override;
	
protected:
	virtual void				FindAlertPoint(_uint iMonsterState, _uint iState, string strCompAnimName, string AlertPointName) override;

public:
	void						StartNoise() { m_bNoiseStart = true; }
	virtual void				EndNoise() {}

protected:
	void						ResetAlertCnt(_uint iCurrentEndAnimIdx)
	{
		if (m_iAlertCnt[iCurrentEndAnimIdx])
			m_iAlertCnt[iCurrentEndAnimIdx] = 0;
	}

protected:
	CMonsterBase*				m_pOwnerMonster = nullptr;

protected:
	_uint						m_iPrevTickState = CMonsterBase::MONSTER_STATE::MS_IDLE;
	_int						m_iNextPlayAnimationIndex = 0;

	_uint						m_iPrevPlayAnimIndex = 0;


protected:
	CTexture*					m_pTextureNoiseCom = nullptr;
	_float						m_fNoiseMaxTime = 5.f;
	_float						m_fNoiseAccTime = 0.f;
	_bool						m_bNoiseStart = false;

	_int						m_iAlertCnt[30] = {};

private:
	virtual HRESULT				ReadyComponents() override;
	virtual HRESULT				BindShaderResources() override;

public:
	virtual void				Free() override;
};

END