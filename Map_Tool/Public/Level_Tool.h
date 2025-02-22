#pragma once
#include "Tool_Defines.h"
#include "Level.h"
#include "Tool_Main_Camera.h"
#include "ColBoundingBox.h"
#include "Model.h"
#include <filesystem>
#include "tinyxml2.h"
#include "Bone.h"
#include "GameObject.h"
#include "CellLine.h"
#include "Tool_Stage_01.h"

BEGIN(Tool)

class CTempWater;
class CBase;
class CTool_Terrain;
class CColFloor;


struct BONE
{
	string			strName;
	vector<BONE>	Child;
};

struct RUL
{
	_float3 vRight;
	_float3 vUp;
	_float3 vLook;
	_float3 vPos;
};


class CLevel_Tool final : public CLevel
{
public:
	enum PICKING_TYPE
	{
		PICK_NONE,

		PICK_SOMTHING,
		PICK_SPAWN,

		PICK_END,
	};

	enum SPAWN_TPYE
	{
		SPAWN_NONE,
		SPAWN_TEMP_CUBE,
		SPAWN_DEFAULT_MONSTER_01,
		SPAWN_DEFAULT_MONSTER_02,
		SPAWN_DEFAULT_MONSTER_03,
		SPAWN_BOSS,

		SPAWN_END,
	};

	enum ANIM_NAME_TYPE 
	{ 
		ANIM_IDLE, 
		ANIM_WALK, 
		ANIM_PUT,
		ANIM_DRAW,
		ANIM_JUMP,
		ANIM_HOME,
		ANIM_TEST,
		ANIM_END,
	};

	enum CELL_CLICK_COUNT
	{
		FIRST,
		SECOND,
		THIRD,
		CL_END,
	};

	

private:
	CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Tool() = default;

public:
	virtual HRESULT			Initialize() override;
	virtual void			Update(const _float& fTimeDelta) override;
	virtual HRESULT			Render() override;


public:
	void					CharToWChar(const _char* pStrInput, _tchar* pStrOutput)
	{
		_int iLen = MultiByteToWideChar(CP_ACP, 0, pStrInput, -1, NULL, NULL);
		MultiByteToWideChar(CP_ACP, 0, pStrInput, static_cast<_int>(strlen(pStrInput) + 1), pStrOutput, iLen);
	}

	void					WCharToChar(const _tchar* pStrInput, _char* pStrOutput)
	{
		_int iLen = WideCharToMultiByte(CP_ACP, 0, pStrInput, -1, NULL, 0, NULL, NULL);
		WideCharToMultiByte(CP_ACP, 0, pStrInput, -1, pStrOutput, iLen, 0, 0);
	}

	// 뼈정보 읽고 렌더 & 애니매이션 툴 관련 함수들
public:
	string					ReadBoneHierachyFromXML(const _char* pFilePath, BONE& pRootBone);
	void					ParseBoneElement(tinyxml2::XMLElement* pElement, BONE& parentBone);
	void					RenderPlayerAnimInfo();
	void					ShowBoneHierarchy();
	void					RenderBones(const BONE& bone, string strRootBoneName);
	void					RenderAnimTool();

	HRESULT 				OpenKeyFrameDialog(ANIM_NAME_TYPE eAnimType);
	HRESULT					SaveAnimationKeyFrame(const _char* pFilePath, string strSavefileName, ANIM_NAME_TYPE eAnimType);

	void					SetRightUpLookPosFromTransformationMatrix(_float3& vRight, _float3& vUp, _float3& vLook, _float3& vPos, const _fmatrix& TransformationMatrix);
	void					CreateRotationQuaternion(const _float3& vRight, const _float3& vUp, const _float3& vLook, _float4& vRotation);

private:
	HRESULT					ReadyLights();
	HRESULT					Ready_Layer_Test_Camera(const _wstring& strLayerTag);
	HRESULT					ReadyLayerMainCamera(const _wstring& strLayerTag);
	HRESULT					ReadyLayerTempWater(const _wstring& strLayerTag);
	HRESULT					ReadyLayerTerrain(const _wstring& strLayerTag);

	// 충돌 기반이 되는 CColFloor Layer
private:
	HRESULT					ReadyLayerColFloor(const _wstring& strLayerTag);
	HRESULT					ReadyLayerStages(const _wstring& strLayerTag);
	HRESULT					ReadyLayerTestModel(const _wstring& strLayerTag);
	HRESULT					ReadyLayerDefaultMonster01(const _wstring& strLayerTag);
	HRESULT					ReadyLayerToolPlayer(const _wstring& strLayerTag);
	HRESULT					ReadyLayerCellLine(const _wstring& strLayerTag);
	HRESULT					ReadyLayerParticles(const _wstring& strLayerTag);

public:
	void					Cal_RayDirAndRayOriginPos(_int iScreenX, _int iScreenY, _vector& vRayDirection, _vector& vRayOriginPos);
	_bool					Picking(PICKING_TYPE ePickType, SPAWN_TPYE eSpawnType, _int iScreenX, _int iScreenY, _float4& vPickPos, _float& fDistance);

private:
	void					ResetVtxCnt(_int iCntX, _int iCntZ);

	// IMGUI
private:
	void					Initialize_IMGUI();
	void					Render_IMGUI();
	void					Render_Camera_Pos();
	void					Render_TempWater();
	void					Render_Picking_Info();
	void					Reset_Picking_Info();
	void					RenderPickedObjectAnimInfo();

	void					RenderSpawnTriggerInfo();
	void					RenderInterMoveInfo();

private:
	void					AttachPlayerToCamera();
	void					DetachPlayerFromCamera();

public:
	void					Set_Current_Picking_Object(CGameObject* pObj);


	// NAV
private:
	void					PickCellLines();
	void					RenderCellLine();
	CCellLine*				CreateCellLine();
	void					FindClosetCellPos(_float4& vSrc);
	_bool					IsLeftHandedCross();
	HRESULT					ExportNavMesh();
	void					ResetCellLine();


private:
	_vector					MeshPicking(_int iScreenX, _int iScreenY, _float4& vPickPos, _float& fDistance);
	
	// Monster Bones
private:
	void					RenderMonsterBonesFromXML();

private:
	void					SaveAABBInfo();

private:
	void					SaveCurrentSpawnTriggerInfo();
	void					SaveMoveEndPosInfo();

private:
	CGameObject*			m_pObj = nullptr;
	CTool_Main_Camera*		m_pCam = nullptr;

	// Terrain
private:
	CTool_Terrain*			m_pTerrain = nullptr;

	// Picking Object
private:
	CColFloor*				m_pColFloor = nullptr;
	CTransform*				m_pColFloorTransform = nullptr;

	CColBoundingBox*		m_pColFloorBoundingBox = nullptr;

	_bool					m_bNotPicking = true;
	_bool					m_bUsePicking = false;

	_bool					m_bPickOnFloor = false;
	_bool					m_bPickOnTerrain = false;
	_bool					m_bPickOnGameObject = false;

	_bool					m_bSpawnColCube = false;
	_bool					m_bSpawnAABB = false;
	_bool					m_bSpawn_SpawnTrigger = false;
	_bool					m_bSpawn_Spawner = false;

	_bool					m_bSpawnInterMove = false;
	_bool					m_bSpawnInterMoveEndPos = false;

	_bool					m_bSpawnMonster01 = false;
	_bool					m_bSpawnMonster02 = false;
	_bool					m_bSpawnMonster03 = false;
	_bool					m_bSpawnMonster04 = false;
	_bool					m_bSpawnMonster05 = false;
	_bool					m_bSpawnMonster06 = false;
	_bool					m_bSpawnMonster07 = false;

	_bool					m_bSpawnMonster08 = false;
	_bool					m_bSpawnMonster09 = false;





private:
	CGameObject*			m_pPlayer = nullptr;
	CTransform*				m_pPlayerTransform = nullptr;
	CModel*					m_pPlayerModel = nullptr;

	CGameObject*			m_pCurrent_Picked_Object = nullptr;
	CTransform*				m_pCurrent_Picked_Object_Transform = nullptr;
	_bool					m_bFollowCameraPos = false;

	_float4					m_vCurrent_Picked_Object_Pos = { 0.f, 0.f, 0.f, 0.f };
	_float3					m_vCurrent_Picked_Object_Scale = { 1.f, 1.f, 1.f};
	_float4					m_vCurrent_Picked_Object_Move_Pos = { 0.f, 0.f, 0.f, 0.f };
	_float					m_fCurrent_Picked_Object_Rot = 0.f;

private:
	CModel*					m_pModel = nullptr;
	_uint					m_iNumModelAnimations = 0;

private:
	PICKING_TYPE			m_ePickingType = PICKING_TYPE::PICK_NONE;

private:
	SPAWN_TPYE				m_eSpawnType = SPAWN_TPYE::SPAWN_NONE;

private:
	BONE							m_RootBone1201;
	BONE							m_RootBone1205;
	BONE							m_RootBone1003;
	BONE							m_RootBone1004;
	BONE							m_RootBone1401;
	BONE							m_RootBone1008;
	BONE							m_RootBone1503;
	BONE							m_RootBone1508;


	BONE							m_RootBoneMonster01;
	BONE							m_RootBoneMonster02;



	string							m_str1201;
	string							m_str1205;
	string							m_str1003;
	string							m_str1004;
	string							m_str1401;
	string							m_str1008;
	string							m_str1503;
	string							m_str1508;


	string							m_strMonster01;
	string							m_strMonster02;


	// Anim & Keyframes
	_bool							m_bLockAnim = false;						// 모델 애니매이션 멈춤
	_bool							m_bStartKeyframe = false;					// 모델 애니매이션 멈춤
	unordered_set<string>			m_SelectedBonesName;						// 현재 ShowBone에서 선택한 뼈 이름
	map<string, CBone*>				m_SelectedBones;							// 선택한 뼈 이름을 통해서 실제 뼈 저장
	map<string, RUL>				m_SelectedBonesRUL;							// bLock시 Right Up Look을 저장하기 위한 map
	map<string, _float4x4*>			m_SelectedBonesTransfomationMatrix;			// bLock시 TransformationMatrix의 주소를 저장하기 위한 map
	map<string, _float4x4>			m_SelectedBonesTransfomationMatrixOrigin;	// bLock시 TransformationMatrix의 값을 저장하기 위한 map
	map<string, vector<_float>>		m_SelectedBonesRadian;						// bLock시 TransformationMatrix의 값을 수정하기 위한 Radian
	map<string, vector<KEYFRAME>>	m_SelectedBonesKeyframes;


	ANIM_NAME_TYPE					m_eSaveAnimNameType = ANIM_END;
	ANIM_NAME_TYPE					m_eLoadAnimNameType = ANIM_END;
	string							m_strSaveAnimName = "";

	_float							m_fKeyframePosition = 0.f;
	_float							m_fAnimSP = 1.f;


	_matrix							m_matCurTransformation;
	_int							m_iSelectBoneCount = 0;
	vector<vector<_matrix>>			m_vTempKeyFrames;
	vector<vector<_matrix>>			m_vPlayerKeyFrames;

private:
	_float4x4*						m_pPlayerBoneMatrix;
	_float4x4						m_pPlayerBoneMatrixOrigin;


	// NavMesh
private:
	_bool							m_bPickNavMesh = false;
	vector<_vector>					m_CellPositions;
	vector<CCellLine*>				m_CellLines;
	_uint							m_iCellClickCount = 0;
	_vector							m_vCurrentCellLineEndPos;

	_float							m_fCellGlobalY = 0.f;
	_float							m_fCellCurrentPickingY = 0.f;

	_float							m_fFloorScaleX = 10.f;
	_float							m_fFloorScaleZ = 10.f;


	_float							m_fFloorX = 0.f;
	_float							m_fFloorY = 0.f;
	_float							m_fFloorZ = 0.f;


	// SpawnTrigger
private:
	enum ROT_TYPE			{ ROT_LEFT, ROT_BACK, ROT_RIGHT, ROT_FRONT,	ROT_NONE, };
	_int					m_iSpawnTriggerNumMonsters = 0;
	SPAWN_MONSTER_TYPE		m_eSpawnTriggerMonsterType;
	_float3					m_vSpawnTriggerMonsterPos;
	ROT_TYPE				m_eSpawnTriggerMonsterRotType;

	_int					m_iSpawnerPickingCount = 0;

	_bool					m_bUseSpawnTrigger = false;
	_bool					m_bSelectedSpawner = false;

	vector<_int>			m_SpawnerMonsterTypes;

	// InterMove
private:
	_float3					m_vMoveEndPos;
	ROT_TYPE				m_eMoveEndPosRotType;
	_int					m_iMoveEndPosPickingCount = 0;



private:
	CTool_Stage_01* m_pStage01 = nullptr;



	// Temp Water
private:
	CTempWater*				m_pTempWater = nullptr;

	_int					m_iTempWater_CntZ = 0;
	_int					m_iTempWater_CntX = 0;

	_vector					m_vTempWater_LightDir = { 0.f, -1.f, 0.f, 0.f };
	_float					m_fTempWater_LightX = 0.f;
	_float					m_fTempWater_LightY = -1.f;
	_float					m_fTempWater_LightZ = 0.f;

	_float					m_fTempWater_Light_DiffuseR = 1.f;
	_float					m_fTempWater_Light_DiffuseG = 1.f;
	_float					m_fTempWater_Light_DiffuseB = 1.f;

	_float					m_fTempWater_Light_AmbientR = 1.f;
	_float					m_fTempWater_Light_AmbientG = 1.f;
	_float					m_fTempWater_Light_AmbientB = 1.f;

	_float					m_fTempWater_Light_SpecularR = 1.f;
	_float					m_fTempWater_Light_SpecularG = 1.f;
	_float					m_fTempWater_Light_SpecularB = 1.f;

	_float					m_fTempWater_DiffuseMaterialR = 1.f;
	_float					m_fTempWater_DiffuseMaterialG = 1.f;
	_float					m_fTempWater_DiffuseMaterialB = 1.f;

	_float					m_fP = 50.f;

public:
	static CLevel_Tool*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void			Free() override;
};

END