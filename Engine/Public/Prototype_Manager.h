#pragma once

#include "Base.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBufferRect.h"
#include "VIBuffer_Terrain.h"
#include "VIBufferSkyBox.h"
#include "VIBufferParticleInstance.h"
#include "VIBufferParticleRect.h"
#include "VIBufferParticlePoint.h"
#include "VIBufferTrail.h"

#include "BoundingAABB.h"
#include "BoundingOBB.h"
#include "BoundingSphere.h"

/* 원형객체들을 레벨별로 관리하겠다.*/
/* 특정 레벨에서 복제할 필요가 없는 원형들은 삭제하겠다. */
/* 지정한 객체를 복제하여 리턴한다. */

BEGIN(Engine)

class CBase;

class CPrototype_Manager final : public CBase
{
private:
	CPrototype_Manager();
	virtual ~CPrototype_Manager() = default;

public:
	HRESULT								Initialize(_uint iNumLevels);
	HRESULT								AddPrototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype);
	CBase*								ClonePrototype(Engine::PROTOTYPE eType, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg);
	void								Clear(_uint iLevelIdx);

private:
	_uint								m_iNumLevels = { 0 };

	// 클라에 레벨이 몇개 존재 할지 모르기 때문에 std::map<>* 타입으로 선언해준다.
	map<const _wstring, class CBase*>*	m_pPrototypes = { nullptr };
	typedef map<const _wstring, class CBase*>	PROTOTYPE;

private:
	CBase*								FindPrototype(_uint iLevelIndex, const _wstring& strPrototypeTag);

public:
	static CPrototype_Manager*			Create(_uint iNumLevels);
	virtual void						Free();
};

END