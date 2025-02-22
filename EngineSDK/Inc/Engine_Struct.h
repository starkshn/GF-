#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#include "Engine_Typedef.h"

namespace Engine
{
	typedef struct
	{
		HINSTANCE		hInstance;
		HWND			hWnd;
		bool			isWindowed;
		unsigned int	iNumLevels;
		unsigned int	iViewportWidth;
		unsigned int	iViewportHeight;

	}	ENGINE_DESC;


	typedef struct light_desc
	{
		enum TYPE { TYPE_POINT, TYPE_DIRECTIONAL, TYPE_SPOTLIGHT, TYPE_END };

		TYPE		eType;
		XMFLOAT4	vDirection;
		XMFLOAT4	vPosition;
		float		fRange;

		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;

		string		strLightName;
		float		fMaxActiveTime;

	} LIGHT_DESC;

	
	typedef struct material_desc
	{
		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;

	} MATERIAL_DESC;

	
	typedef struct ENGINE_DLL vtx_pos
	{
		XMFLOAT3	vPosition;

		static const unsigned int				iNumElements = 1;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];

	} VTX_POS;



	
	typedef struct ENGINE_DLL hit_result
	{

		XMFLOAT3		vHitPos;
		XMFLOAT3		vNormal;
		void*			pOther;

	}	HIT_RESULT;


	typedef struct ENGINE_DLL spawn_info
	{

		SPAWN_MONSTER_TYPE	eType;
		XMFLOAT3			vPos;
		XMMATRIX			MatrixPre;	// 스폰후 회전해야할 양 PreTrasformMatrix랑 다르다. 그냥 스폰후 어느 방향으로 회전해 있을 것인지

	}	SPAWN_INFO;



	typedef struct ENGINE_DLL vtxposcolor
	{
		XMFLOAT3	vPosition;
		XMFLOAT3	vColor;

		static const unsigned int				iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}	VTX_POSCOLOR;


	typedef struct ENGINE_DLL vtxpostex
	{
		XMFLOAT3	vPosition;
		XMFLOAT2	vTexcoord;

		static const unsigned int				iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}	VTX_POSTEX;



	typedef struct ENGINE_DLL vtxpostextime
	{
		XMFLOAT3	vPosition;
		XMFLOAT2	vTexcoord;
		XMFLOAT2	fLifeTime;

		static const unsigned int				iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}	VTX_POSTEX_TIME;


	typedef struct ENGINE_DLL vtxnortex
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int				iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];

	}	VTX_NORTEX;

	typedef struct ENGINE_DLL vtxcube
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexcoord;

		static const unsigned int					iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[2];
	}VTXCUBE;

	typedef struct ENGINE_DLL vtxmesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		static const unsigned int					iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];

	}	VTX_MESH;


	typedef struct ENGINE_DLL vtxanimmesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndices;
		XMFLOAT4		vBlendWeights;
		static const unsigned int					iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}	VTX_ANIMMESH;


	typedef struct ENGINE_DLL keyframe
	{
		XMFLOAT3	vScale;
		XMFLOAT4	vRotation;
		XMFLOAT3	vPosition;
		float		vKeyFramePosition;
	} KEYFRAME;




	typedef struct ENGINE_DLL AI_NON_ANIM_INFO
	{
		_float3 vPosition;
		_float3 vNormal;
		_float2 vTexcoord;
		_float3 vTangent;
	} AI_NONANIM_VTX;


	typedef struct ENGINE_DLL ai_nonanim_indices
	{
		_int iIndex0;
		_int iIndex1;
		_int iIndex2;
	} AI_NONANIM_INDEX;


	typedef struct ENGINE_DLL ai_bone_info
	{
		_int iParentIndex;
		_int iNumChild;
		_int iLenBoneName;
		_char szBoneName[MAX_PATH];
		_float4x4 TransformMatrix = {};
		_float4x4 CombinedMatrix = {};

	} AI_BONE_INFO;


	typedef struct ENGINE_DLL ai_anim_info
	{
		_int iLenAnimName;
		_char szAnimName[MAX_PATH];
		_float fDuration;
		_float fTickPerSecond;
		_int iNumChannels;

	} AI_ANIMATION_INFO;


	typedef struct ENGINE_DLL ai_anim_vtx_info
	{
		_int iLenAnimMeshName;
		_char szAnimMeshName[MAX_PATH];
		_int iAnimMeshMaterialIndex;
		_int iAnimMeshVertices;
		_int iAnimMeshNumFaces;
		_int iNumBones;
	} AI_ANIM_MESH_INFO;
	
	typedef struct ENGINE_DLL ai_channel_info
	{
		_int		iLenChannelName;
		_char		szChannelName[MAX_PATH];
		_uint		iBoneIndex;
		_uint		iNumKeyFrame;

	} AI_CHANNEL_INFO;


	typedef struct ENGINE_DLL ai_bone_index_offsetmatrix
	{
		_int		iBoneIndex;
		_float4x4	OffsetMatrix;

	} AI_BONEINDEX_OFFSETMATRX;


	typedef struct ENGINE_DLL vtxinstance
	{

		XMFLOAT4 vRight;
		XMFLOAT4 vUp;
		XMFLOAT4 vLook;
		XMFLOAT4 vTranslation;
		XMFLOAT2 vLifeTime;

	} VTX_INSTANCE;

	
	typedef struct ENGINE_DLL vtx_rect_instance
	{

		static const unsigned int iNumElements = 7;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];

	} VTX_RECT_INSTANCE;


	typedef struct ENGINE_DLL vtx_point
	{

		XMFLOAT3		vPosition;
		XMFLOAT2		vSize;

	} VTX_POINT;

	typedef struct ENGINE_DLL vtx_point_instance
	{

		static const unsigned int					iNumElements = { 7 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];

	}	VTX_POINT_INSTANCE;

}


#endif // Engine_Struct_h__
