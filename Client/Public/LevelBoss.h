#pragma once
#include "Client_Defines.h"
#include "Level.h"

BEGIN(Engine)

class CModel;
class CTransform;

END



BEGIN(Client)

class CLevelBoss final : public CLevel
{
private:
	CLevelBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevelBoss() = default;

public:
	virtual HRESULT			Initialize() override;
	virtual void			Update(const _float& fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	HRESULT					ReadyLights();
	HRESULT					ReadyLayerCameraFree(const _wstring& strLayerTag);
	HRESULT					ReadyLayerParticles(const _wstring& strLayerTag);
	HRESULT					ReadyLayerPlayer(const _wstring& strLayerTag);
	HRESULT					ReadyLayerStages(const _wstring& strLayerTag);
	HRESULT					ReadyLayerBoss(const _wstring& strLayerTag);
	HRESULT					ReadyLayerBossUI(CGameObject* pOwner, _int iLevelIdx, _int iMaxHP);
	HRESULT					ReadyLayerUI();

	HRESULT					ReadySkyBox(const _wstring& strLayerTag);
	HRESULT					ReadyBossRock(const _wstring& strLayerTag);
	HRESULT					ReadyMeteors(const _wstring& strLayerTag);

	HRESULT					ReadySkyBox();

private:
	HRESULT					LoadAABB();
	void					ReadAABB(string strFilePath);
	HRESULT					LoadSpawnTriggers();
	void					ReadSpawnTriggers(string strFilePath, _int iNavMeshIdx, _int iDir = -1);


	HRESULT					LoadPointLights();
	HRESULT					ReadPointlights(string strFilePath, const _wstring& strLayerTagTorch, const _wstring& strLayerTagTorchEffect);
	
public:
	static CLevelBoss*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void			Free() override;
};

END