#pragma once
#include "Base.h"

BEGIN(Engine)

class CGameInstance;

class CFrustum final : public CBase
{
private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT			Initialize();

	void			Update();

	// 특정 오브젝트의 로컬로 변환하기 위해 필요에 따라 호출되는 함수
	void			TransformToLocalSpace(_matrix WorldMatrix);


	_bool			IsInWorldSpace(_fvector vWorldPos, _float fRange = 0.f);
	_bool			IsInLocalSpace(_fvector vLocalPos, _float fRange = 0.f);
	
private:
	CGameInstance*	m_pGameInst = nullptr;
	_float3			m_vPoints[8] = {};
	_float3			m_vPointsInWorld[8] = {};
	_float4			m_vPlanesInWorld[6] = {};
	_float4			m_vPlanesInLocal[6] = {};

private:
	void			MakePlanes(const _float3* pPoints, _Out_ _float4* pPlanes);

public:
	static CFrustum* Create();
	virtual void Free() override;
};

END