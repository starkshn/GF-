#pragma once
#include "MonsterBodyBase.h"


BEGIN(Engine)
class CShader;
END


BEGIN(Client)

class CMonsterBody02 final : public CMonsterBodyBase
{
private:
	CMonsterBody02(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterBody02(const CMonsterBody02& Prototype);
	virtual ~CMonsterBody02() = default;

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
	virtual HRESULT				ReadyComponents() override;
	virtual HRESULT				BindShaderResources() override;

	virtual void				FindAlertPoint(_uint iMonsterState, _uint iState, string strCompAnimName, string AlertPointName) override;

	virtual void				FindAlertPointSounds(_uint iMonsterState, _uint iState, string strCompAnimName, string AlertPointName) override;
	void						CheckAlertPointSoundName() override;

public:
	static CMonsterBody02*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END