#pragma once
#include "MonsterBodyBase.h"

BEGIN(Engine)
class CShader;
END


BEGIN(Client)

class CMonsterBody03 final : public CMonsterBodyBase
{
private:
	CMonsterBody03(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterBody03(const CMonsterBody03& Prototype);
	virtual ~CMonsterBody03() = default;

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
	void						CheckAlertPointSoundName() override;

private:
	virtual HRESULT				ReadyComponents() override;
	virtual HRESULT				BindShaderResources() override;

public:
	static CMonsterBody03*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END