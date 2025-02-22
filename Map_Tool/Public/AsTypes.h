#pragma once
#include "Engine_Defines.h"

using VertexType = VTX_MESH;
using Color = XMFLOAT4;


struct asBone
{
	string				name;
	_int				index = -1;
	_int				parent = -1;
	_matrix				transform;
};


struct asMesh
{
	string				name;
	aiMesh*				pMesh;
	vector<VertexType>	vertices;
	vector<_uint>		indices;

	_int				boneIndex;
	string				materialName;
};	

struct asMaterial
{
	string strName;
	Color diffuse;
	Color ambient;
	Color specular;
	Color emissive;

	string diffuseMapFile;
	string specularMapFile;
	string normapMapFile;
};