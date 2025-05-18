#pragma once
#include "Client_Defines.h"
#include "MonsterBodyBase.h"

BEGIN(Engine)

class CShader;

END


BEGIN(Client)

class CBossBody final : public CMonsterBodyBase
{
private:
	CBossBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossBody(const CBossBody& Prototype);
	virtual ~CBossBody() = default;

public:
	virtual HRESULT				InitializePrototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				PriorityUpdate(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				LateUpdate(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

private:
	virtual void				InitializeAnimNameAndIndex() override;

private:
	virtual void				AnimationEnd(_uint iCurrentModelIndex = 0) override;

private:
	virtual string				GetAnimationNameByState(_uint iState) override;

private:
	_bool						IsUsedAttackAlert(string strAlertName);
	void						ResetAttackAlert(string strAlertName);
	void						UseAttackAlert(string strAlertName);

	virtual void				FindAlertPoint(_uint iMonsterState, _uint iState, string strCompAnimName, string AlertPointName) override;
	virtual void				FindAlertPointSounds(_uint iMonsterState, _uint iState, string strCompAnimName, string AlertPointName) override;


	void						CheckAlertPointSoundName() override;



	virtual void				EndNoise() override;


private:
	map<string, _bool>			m_AttackAlertChecks;


private:
	_float						m_fDeadMaxTime = 8.f;
	_float						m_fDeadAccTime = 0.f;

	_bool m_bRender = true;

private:
	virtual HRESULT				ReadyComponents() override;
	virtual HRESULT				BindShaderResources() override;

public:
	static CBossBody*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END