#pragma once
#include "Client_Defines.h"
#include "PartObject.h"
#include "Model.h"

BEGIN(Engine)

class CShader;

END

BEGIN(Client)

class CStateObjectContainer;

class CStateObjectBody : public CPartObject
{
public:
	typedef struct state_object_body_desc : public PARTOBJECT_DESC
	{
		_uint*					pState;
		_uint*					pPrevState;
		_uint					iNumModels;
		CStateObjectContainer*	pOwner;

	} STATE_OBJECT_BODY_DESC;

protected:
	CStateObjectBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStateObjectBody(const CStateObjectBody& Prototype);
	virtual ~CStateObjectBody() = default;

public:
	virtual HRESULT				InitializePrototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				PriorityUpdate(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				LateUpdate(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

private:
	virtual void				SetPlayAnimation(_uint iState) {}
	virtual void				ChangeAnimation(_uint iState, _uint iCurrentModelIndex = 0) {}
	virtual void				ChangeAnimationByAnimIndex(_uint iFrom, _uint iTo, _uint iModelIndex = 0) {}
	virtual void				AnimationEnd(_uint iCurrentModelIndex = 0) {}

private:
	virtual void				ChangeAnimationWhenEnd(_uint iCurrentModelIndex = 0) {}
	virtual void				ChangeAnimationWhenPlaying(_uint iState, _uint iCurrentModelIndex = 0) {}
	virtual void				ChangeAnimationWhenPlayingByAnimIndex(_uint iFrom, _uint iTo, _uint iCurrentModelIndex = 0) {}
	virtual void				ChangeAnimationImmediately(_uint iState, _uint iCurrentModelIndex = 0) {}
	virtual void				ChangeAnimationImmediatelyByAnimIndex(_uint iTo, _uint iCurrentModelIndex = 0) {}
	virtual void				ChangeAnimationInterpolation(_uint iCurrentModelIndex = 0) {}
	virtual void				ChangeAnimationInterpolationByAnimIndex(_uint iFrom, _uint iTo, _uint iCurrentModelIndex = 0) {}

public:
	const _float4x4*			GetBoneCombiedMatrix(const _char* pBoneName) const { return m_pModelComs[m_iCurrentModelIndex]->GetBoneCombinedMatrix(pBoneName); }
	CModel*						GetModelByName(string strName);

public:
	virtual _int				GetAnimationIndexByState(_uint iState, _uint iModelIndex = 0);
	virtual string				GetAnimationNameByAnimationName(string strAnimKeyName, _uint iModelIndex = 0);
	virtual _int				GetAnimationIndexByAnimationName(string strAnimName, _uint iModelIndex = 0);
	virtual string				GetAnimationNameByState(_uint iState);
	virtual string				GetAnimationNameByAnimationIndex(_int iAnimationIndex, _uint iModelIndex = 0);
	_uint						GetCurrentModelIndex() { return m_iCurrentModelIndex; }

	void						SetCurrentModelIndex(_uint iCurrentModelIndex) { m_iCurrentModelIndex = iCurrentModelIndex; }

protected:
	virtual void				FindAlertPoint(_uint iCurState, _uint iState, string strCompAnimName, string AlertPointName) {};
	virtual void				FindAlertPointSounds(_uint iCurState, _uint iState, string strCompAnimName, string AlertPointName) {};

	virtual void				CheckAlertPointSoundName() {};


protected:
	CShader*					m_pShaderCom = nullptr;
	vector<CModel*>				m_pModelComs;

protected:
	CStateObjectContainer*		m_pOwner = nullptr;

protected:
	_uint*						m_pState = nullptr;
	_uint*						m_pPrevState = nullptr;
	_uint						m_iCurState = 1;

protected:
	_uint						m_iNumModels = 0;

private:
	_uint						m_iCurrentModelIndex = 0;

protected:
	vector<map<string, _int>>	m_ModelsAnimationNames;
	_bool						m_bIsAnimationPlaying = false;
	_bool						m_bIsInterpolationing = false;

protected:
	string						m_strPrevAlertName = "NONE";

private:
	virtual HRESULT				ReadyComponents();
	virtual HRESULT				BindShaderResources();

public:
	virtual void				Free() override;
};

END