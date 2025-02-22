#include "Loader.h"
#include "GameInstance.h"
#include "VIBuffer_TempWater.h"
#include "TempWater.h"
#include "VIBuffer_Terrain.h"
#include "Tool_Terrain.h"
#include "Tool_Main_Camera.h"
#include "Model.h"


#include "VIBuffer_TempCube.h"
#include "TempCube.h"

#include "TestModel.h"
#include "TestModelBody.h"
#include "TestModelWeapon.h"

#include "ColCube.h"
#include "VIBuffer_ColCube.h"


// Player
#include "Tool_Player.h"

// ColBoudingBox
#include "ColBoundingBox.h"

// Monsters
#include "Tool_Default_Monster_01.h"
#include "Monster02.h"
#include "Monster03.h"
#include "Monster04.h"
#include "Monster05.h"
#include "Monster06.h"
#include "Monster07.h"
#include "Monster08.h"

// Stages
#include "Tool_Stage_01.h"
#include "Tool_Boss_Stage.h"


#include "Converter.h"


// 스테이지 밑 바닥 오브젝트
#include "ColFloor.h"

#include "CellLine.h"

#include "ToolPlayerBody.h"
#include "ToolPlayerWeapon.h"


// ===========================================
// Particles
// ===========================================
#include "VIBufferParticleRect.h"
#include "VIBufferParticlePoint.h"

#include "ParticleSnow.h"
#include "ParticleExplosion.h"

// ===========================================
// Effect
// ===========================================
#include "Effect.h"

// ===========================================
// SkyBox
// ===========================================
#include "SkyBox.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
    , m_pGameInst{ CGameInstance::GetInstance() }
{
    SafeAddRef(m_pDevice);
    SafeAddRef(m_pContext);
    SafeAddRef(m_pGameInst);
}


_uint APIENTRY LoadingMain(void* pArg)
{
    CLoader* pLoader = static_cast<CLoader*>(pArg);

    // 비정상 종료 일때 return 1을 해준다.
    if (FAILED(pLoader->Loading()))
        return 1;

    return 0;
}

HRESULT CLoader::Initialize(LEVELID eNextLevelID)
{
    m_eNextLevelID = eNextLevelID;

    InitializeCriticalSection(&m_Critical_Section);

    m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);

    if (0 == m_hThread)
        return E_FAIL;

    return S_OK;
}

#ifdef _DEBUG

void CLoader::Show_Debug()
{
    SetWindowText(g_hWnd, m_szLoadingText);
}

#endif

HRESULT CLoader::Loading()
{
    EnterCriticalSection(&m_Critical_Section);

    CoInitializeEx(nullptr, 0);

    HRESULT hr = {};

    switch (m_eNextLevelID)
    {
    case Tool::LEVEL_TOOL:
        hr = LoadingLevelTool();
        break;
    case Tool::LEVEL_END:
        break;
    default:
        break;
    }

    LeaveCriticalSection(&m_Critical_Section);

    return S_OK;
}

HRESULT CLoader::ReadyModelPlayer()
{
    _matrix PreTransformMatrix = XMMatrixIdentity();
    
   

    return S_OK;
}

HRESULT CLoader::ReadyModelMonsters()
{
    
    return S_OK;
}

HRESULT CLoader::ReadyTerrain()
{
    // Terrain Textures
    {
        // 241025 Mask를 위해 dds파일 두장을 로드해준다.
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Terrain"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/Terrain/Tile%d.dds"), 2)))) return E_FAIL;

        // Terrain Mask Texture
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Terrain_Mask"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"), 1)))) return E_FAIL;

        /* For.Prototype_Component_Texture_Terrain_Brush */
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Terrain_Brush"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/Terrain/Brush.png"), 1)))) return E_FAIL;

        // Terrain 높이맵 로드
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/Terrain/Height.bmp"))))) return E_FAIL;
    }

    // Terrain Shader
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxNorTex_Terrain"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex_Terrain.hlsl"), VTX_NORTEX::Elements, VTX_NORTEX::iNumElements)))) return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxNorTex_Terrain_Brush"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/02_Shader_VtxNorTex_Terrain_Brush.hlsl"), VTX_NORTEX::Elements, VTX_NORTEX::iNumElements)))) return E_FAIL;
    }
    

    // Terrain Proto
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Terrain"), CTool_Terrain::Create(m_pDevice, m_pContext)))) return E_FAIL;
    }

    return S_OK;
}

HRESULT CLoader::ReadyTestModel()
{
    _matrix PreTransformMatrix;

    // Fiona
    vector<_int> Useless = { -1 };
    PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Fiona"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/resources/Models/Fiona/Fiona.dat", PreTransformMatrix, Useless, false, true, "Fiona")))) return E_FAIL;


    PreTransformMatrix = XMMatrixIdentity();

    // Weapon
    vector<_int> vec = { -1 };
    PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_ForkLift"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/resources/Models/ForkLift/ForkLift.fbx", PreTransformMatrix, vec, false, false))))
        return E_FAIL;


    // Test Model 로드
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_TestModel"), CTestModel::Create(m_pDevice, m_pContext)))) return E_FAIL;

    // Test Model 로드
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_TestModelBody"), CTestModelBody::Create(m_pDevice, m_pContext)))) return E_FAIL;

    // Test Model 로드
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_TestModelWeapon"), CTestModelWeapon::Create(m_pDevice, m_pContext)))) return E_FAIL;


    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_ToolPlayerBody"), CToolPlayerBody::Create(m_pDevice, m_pContext)))) return E_FAIL;

    // Test Model 로드
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_ToolPlayerWeapon"), CToolPlayerWeapon::Create(m_pDevice, m_pContext)))) return E_FAIL;


    return S_OK;
}

HRESULT CLoader::ReadyBuffers()
{
    // For. Prototype_Component_VIBuffer_TempWater
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_TempWater"), CVIBuffer_TempWater::Create(m_pDevice, m_pContext))))
        return E_FAIL;


    // For. Prototype_Component_VIBuffer_TempCube
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_TempCube"), CVIBuffer_TempCube::Create(m_pDevice, m_pContext))))
        return E_FAIL;


    // For. Prototype_Component_VIBuffer_ColCube
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_ColCube"), CVIBuffer_ColCube::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoader::ReadyCompBoundingBox()
{
    // 컴포넌트 바운딩 박스 로드
    {
        // For. Prototype_Component_BoundingBox
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_BoundingBox"), CColBoundingBox::Create(m_pDevice, m_pContext, CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosColor.hlsl"), VTX_POSCOLOR::Elements, VTX_POSCOLOR::iNumElements), CVIBuffer_ColCube::Create(m_pDevice, m_pContext)))))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLoader::ReadyCollider()
{
    /* For.Prototype_Component_Collider_AABB */
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Collider_AABB"),
        CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
        return E_FAIL;

    /* For.Prototype_Component_Collider_OBB */
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Collider_OBB"),
        CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
        return E_FAIL;

    /* For.Prototype_Component_Collider_Sphere */
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Collider_Sphere"),
        CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
        return E_FAIL;
    
    return S_OK;
}

HRESULT CLoader::ReadyCellLine()
{
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_CellLine"), CCellLine::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    
    return S_OK;
}

HRESULT CLoader::ReadyParticles()
{
    // Snow
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Snow"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxRectInstance"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl") , VTX_RECT_INSTANCE::Elements, VTX_RECT_INSTANCE::iNumElements))))
            return E_FAIL;

        CVIBufferParticleInstance::PARTICLE_DESC			SnowDesc{};

        SnowDesc.iNumInstances = 5000;
        SnowDesc.vCenter = _float3(64.f, 20.f, 64.f);
        SnowDesc.vRange = _float3(128.f, 1.f, 128.f);
        SnowDesc.vSpeed = _float2(4.f, 10.f);
        SnowDesc.vLifeTime = _float2(2.f, 4.f);
        SnowDesc.vSize = _float2(0.1f, 0.5f);
        SnowDesc.bIsLoop = true;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Particle_Snow"), CVIBufferParticleRect::Create(m_pDevice, m_pContext, SnowDesc))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Particle_Snow"), CParticleSnow::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }


    // Explosion Rect
    {
       /* if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Explosion"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/ParticleExplosion.png"), 1))))
            return E_FAIL;

        
        CVIBufferParticleInstance::PARTICLE_DESC			ExplosionDesc{};

        ExplosionDesc.iNumInstances = 200;
        ExplosionDesc.vCenter = _float3(0.f, 0.f, 0.f);
        ExplosionDesc.vRange = _float3(1.f, 1.f, 1.f);
        ExplosionDesc.vSpeed = _float2(1.f, 10.f);
        ExplosionDesc.vLifeTime = _float2(0.3f, 1.f);
        ExplosionDesc.vSize = _float2(0.1f, 0.8f);
        ExplosionDesc.vPivot = _float3(0.f, -1.0f, 0.f);
        ExplosionDesc.bIsLoop = true;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Particle_Explosion"), CVIBufferParticleRect::Create(m_pDevice, m_pContext, ExplosionDesc))))
            return E_FAIL;


        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Particle_Explosion"), CParticleExplosion::Create(m_pDevice, m_pContext))))
            return E_FAIL;*/
    }

    // Explosion Point
    {
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Explosion"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/ParticleExplosion.png"), 1))))
            return E_FAIL;

         if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxPointInstance"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTX_POINT_INSTANCE::Elements, VTX_POINT_INSTANCE::iNumElements))))
            return E_FAIL;


        CVIBufferParticleInstance::PARTICLE_DESC			ExplosionDesc{};

        ExplosionDesc.iNumInstances = 200;
        ExplosionDesc.vCenter = _float3(0.f, 0.f, 0.f);
        ExplosionDesc.vRange = _float3(10.f, 1.f, 10.f);
        ExplosionDesc.vSpeed = _float2(1.f, 10.f);
        ExplosionDesc.vLifeTime = _float2(0.3f, 1.f);
        ExplosionDesc.vSize = _float2(0.1f, 0.8f);
        ExplosionDesc.vPivot = _float3(0.f, -1.0f, 0.f);
        ExplosionDesc.bIsLoop = true;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Particle_Explosion"), CVIBufferParticlePoint::Create(m_pDevice, m_pContext, ExplosionDesc))))
            return E_FAIL;

        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Particle_Explosion"), CParticleExplosion::Create(m_pDevice, m_pContext))))
            return E_FAIL;
    }


    return S_OK;
}

HRESULT CLoader::ReadyEffect()
{	

    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBufferRect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_Effect */
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Effect"), CEffect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Effect"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/Explosion/Explosion%d.png"), 90))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoader::ReadySkyBox()
{
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxCube_SkyBox"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/shaderFiles/Shader_VtxCube_SkyBox.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
        return E_FAIL;

    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_SkyBox"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
        return E_FAIL;

    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_SkyBox"), CVIBufferSkyBox::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_SkyBox */
    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_SkyBox"), CSkyBox::Create(m_pDevice, m_pContext))))
        return E_FAIL;


    return S_OK;
}

HRESULT CLoader::LoadingLevelTool()
{
    lstrcpy(m_szLoadingText, TEXT("Loading Textures..."));
    {
        // TempWater Tex
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Water"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/water.dds")))))
            return E_FAIL;


        // 로고 텍스쳐 로드
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Logo"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/resources/Textures/Collider/Collider.png")))))
            return E_FAIL;
    }

    lstrcpy(m_szLoadingText, TEXT("Loading Sounds"));
    // TODO

    lstrcpy(m_szLoadingText, TEXT("Loading Shader"));
    {
        // 실제 Shader 로드 코드
        {
            // TempWater
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxNorTex_TempWater"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex_TempWater.hlsl"), VTX_NORTEX::Elements, VTX_NORTEX::iNumElements))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_00_Shader_VtxNorTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/00_Shader_VtxNorTex.hlsl"), VTX_NORTEX::Elements, VTX_NORTEX::iNumElements))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_01_Shader_VtxNorTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/01_Shader_VtxNorTex.hlsl"), VTX_NORTEX::Elements, VTX_NORTEX::iNumElements))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_02_Shader_VtxNorTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/02_Shader_VtxNorTex.hlsl"), VTX_NORTEX::Elements, VTX_NORTEX::iNumElements))))
                return E_FAIL;

            // 모델 테스트용 쉐이더 파일(241010 직접 만든 쉐이더 파일)
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_00_Shader_VtxMesh"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/00_Shader_VtxMesh.hlsl"), VTX_MESH::Elements, VTX_MESH::iNumElements))))
                return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_00_Shader_VtxNorTex_Cube"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/00_Shader_VtxNorTex_Cube.hlsl"), VTX_NORTEX::Elements, VTX_NORTEX::iNumElements))))
                return E_FAIL;

            // ColCube 전용 쉐이더
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxPosColor"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosColor.hlsl"), VTX_POSCOLOR::Elements, VTX_POSCOLOR::iNumElements))))
                return E_FAIL;

            // AnimMesh 쉐이더
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxAnimMesh"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/00_Shader_VtxAnimMesh.hlsl"), VTX_ANIMMESH::Elements, VTX_ANIMMESH::iNumElements))))
                return E_FAIL;

            // AnimMesh 쉐이더
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxPosTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTX_POSTEX::Elements, VTX_POSTEX::iNumElements))))
                return E_FAIL;
        }
    }

    lstrcpy(m_szLoadingText, TEXT("Loading Models"));
    if (FAILED(ReadyModelPlayer()))
        return E_FAIL;

    if (FAILED(ReadyModelMonsters()))
        return E_FAIL;

    if (FAILED(ReadyTerrain()))
        return E_FAIL;

    if (FAILED(ReadyTestModel()))
        return E_FAIL;

    if (FAILED(ReadyBuffers()))
        return E_FAIL;

    if (FAILED(LoadStage()))
        return E_FAIL;

    if (FAILED(ReadyCompBoundingBox()))
        return E_FAIL;
    
    if (FAILED(ReadyCollider()))
        return E_FAIL;

    if (FAILED(ReadyCellLine()))
        return E_FAIL;
    
    if (FAILED(ReadyParticles()))
        return E_FAIL;

    if (FAILED(ReadyEffect()))
        return E_FAIL;

    if (FAILED(ReadySkyBox()))
        return E_FAIL;

    
    


    lstrcpy(m_szLoadingText, TEXT("Loading Buffer"));


    _matrix		PreTransformMatrix = XMMatrixIdentity();

    lstrcpy(m_szLoadingText, TEXT("Loading Objects"));

    PreTransformMatrix = XMMatrixIdentity();
    // 실제 객체 원형 로드
    {
        // Camera
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Camera"), CTool_Main_Camera::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        // Water
        if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_TempWater"), CTempWater::Create(m_pDevice, m_pContext))))
            return E_FAIL;

        


        // BoundingBox를 가지고 있는 객체들 로드
        {
            // CColFloor
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_ColFloor"), CColFloor::Create(m_pDevice, m_pContext))))
                return E_FAIL;

            // CColCube
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_ColCube"), CColCube::Create(m_pDevice, m_pContext))))
                return E_FAIL;

            // TempCube
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_TempCube"), CTempCube::Create(m_pDevice, m_pContext))))
                return E_FAIL;


            // Tool_Player
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Tool_Player"), CTool_Player::Create(m_pDevice, m_pContext))))
                return E_FAIL;


            // Prototype_GameObject_Monster01
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Monster01"), CTool_Default_Monster_01::Create(m_pDevice, m_pContext)))) return E_FAIL;

            // Prototype_GameObject_Monster02
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Monster02"), CMonster02::Create(m_pDevice, m_pContext)))) return E_FAIL;

            // Prototype_GameObject_Monster03
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Monster03"), CMonster03::Create(m_pDevice, m_pContext)))) return E_FAIL;

            // Prototype_GameObject_Monster04
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Monster04"), CMonster04::Create(m_pDevice, m_pContext)))) return E_FAIL;

            //// Prototype_GameObject_Monster05
            // 말 몬스터
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Monster05"), CMonster05::Create(m_pDevice, m_pContext)))) return E_FAIL;

            //// Prototype_GameObject_Monster06
            // 딱정 벌레
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Monster06"), CMonster06::Create(m_pDevice, m_pContext)))) return E_FAIL;

            //// Prototype_GameObject_Monster07
            // 폭탄 던지는 몬스터
            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Monster07"), CMonster07::Create(m_pDevice, m_pContext)))) return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Monster08"), CMonster08::Create(m_pDevice, m_pContext)))) return E_FAIL;

            if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Monster09"), CMonster08::Create(m_pDevice, m_pContext)))) return E_FAIL;


        }
    }



    lstrcpy(m_szLoadingText, TEXT("Finished Loading"));

    m_bIsFinished = true;

    return S_OK;
}



HRESULT CLoader::LoadStage()
{
    // _matrix		PreTransformMatrix = XMMatrixIdentity();

    //// Stage01 컴포넌트 원형 추가
    //{
    //    _matrix PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(-19.f, 0.f, 3.f);
    //    // _matrix PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

    //    // CConverter::GetInstance()->ExportAssetFile("../Bin/Resources/Models/GF_Stages/Stage01/Stage01.fbx", "../Bin/Resources/Models/GF_Stages/Stage01/Stage01.dat", CModel::TYPE_ANIM, PreTransformMatrix);

    //    vector<_int> Useless = { -1 };
    //    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Stage01"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/GF_Stages/Stage01/Stage01.fbx", PreTransformMatrix, Useless, false)))) return E_FAIL;
    //}

    //// 보스 스테이지 컴포넌트 원형 추가.
    //{
    //    vector<_int> Useless = { -1 };
    //    _matrix PreTransformMatrix = XMMatrixIdentity();
    //    if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_Component_Boss_Stage"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/GF_Stages/Boss_Stage/Boss_Stage.dat", PreTransformMatrix, Useless, false, true))))
    //        return E_FAIL;
    //}

    //// 사용할 실 객체 추가.

    //if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Stage01"), CTool_Stage_01::Create(m_pDevice, m_pContext))))
    //    return E_FAIL;

    //// Prototype_GameObject_Boss_Stage
    //if (FAILED(m_pGameInst->AddPrototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Boss_Stage"), CTool_Boss_Stage::Create(m_pDevice, m_pContext))))
    //    return E_FAIL;

    return S_OK;
}



CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID)
{
    CLoader* pInst = new CLoader(pDevice, pContext);

    if (FAILED(pInst->Initialize(eNextLevelID)))
    {
        MSG_BOX("Failed to Created : CLoader");
        SafeRelease(pInst);
    }

    return pInst;
}

void CLoader::Free()
{
    __super::Free();

    WaitForSingleObject(m_hThread, INFINITE);

    DeleteObject(m_hThread);
    // _endthread();

    // 끝으로 초기화 함수가 호출 되는 과정에서 할당된 리소스들이 존재하는데, 다음 함수를 사용해서 이를 반환 해야한다.
    DeleteCriticalSection(&m_Critical_Section);

    SafeRelease(m_pDevice);
    SafeRelease(m_pContext);
    SafeRelease(m_pGameInst);
}
