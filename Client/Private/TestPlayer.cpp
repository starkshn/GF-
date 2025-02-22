#include "pch.h"
#include "TestPlayer.h"

#include "GameInstance.h"
#include "TestPlayerBody.h"
#include "TestPlayerWeapon.h"

CTestPlayer::CTestPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CContainerObject(pDevice, pContext)
{
}

CTestPlayer::CTestPlayer(const CTestPlayer& Prototype)
    :
    CContainerObject(Prototype)
{
}

HRESULT CTestPlayer::InitializePrototype()
{
    return S_OK;
}

HRESULT CTestPlayer::Initialize(void* pArg)
{
    CONTAINER_OBJECT_DESC Desc = {};
    Desc.iNumPartObjects = PART_END;
    Desc.fSpeedPerSec = 10.f;
    Desc.fRotSpeedPerSec = XMConvertToRadians(90.f);

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    return S_OK;
}

void CTestPlayer::PriorityUpdate(_float fTimeDelta)
{
    __super::PriorityUpdate(fTimeDelta);
}

void CTestPlayer::Update(_float fTimeDelta)
{
    if (m_pGameInst->KeyPressing(KEY::DOWN))
    {
        m_pTransformCom->GoBackward(fTimeDelta);
        m_iState &= ~STATE_IDLE;
        m_iState |= STATE_WALK;
    }
    // 땐 경우
    else if (m_pGameInst->KeyUp(KEY::DOWN))
    {
        if (m_iState & STATE_WALK)
            m_iState ^= STATE_WALK;

        m_iState |= STATE_IDLE;
    }

    if (m_pGameInst->KeyPressing(KEY::UP))
    {
        m_pTransformCom->GoFoward(fTimeDelta);
        m_iState &= ~STATE_IDLE;
        m_iState |= STATE_WALK;
    }
    // 땐 경우
    else if (m_pGameInst->KeyUp(KEY::UP))
    {
        if (m_iState & STATE_WALK)
            m_iState ^= STATE_WALK;

        m_iState |= STATE_IDLE;
    }

    if (m_pGameInst->KeyPressing(KEY::LEFT))
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);

    if (m_pGameInst->KeyPressing(KEY::RIGHT))
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);

    __super::Update(fTimeDelta);
}

void CTestPlayer::LateUpdate(_float fTimeDelta)
{
    __super::LateUpdate(fTimeDelta);
}

HRESULT CTestPlayer::Render()
{
    return S_OK;
}

HRESULT CTestPlayer::Ready_Components()
{
    // TOOD : 241024 일단 아무것도 안한다.
    return S_OK;
}

HRESULT CTestPlayer::Ready_PartObjects()
{
    {
        CTestPlayerBody::TESTPLAYERBODY_DESC Desc = {};
        Desc.pParentMatrix = m_pTransformCom->GetWorldMatrixPtr();
        Desc.pState = &m_iState;

        m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_TestPlayerBody"), &Desc));
        if (nullptr == m_PartObjects[PART_BODY]) return E_FAIL;
    }
    

    {
        CTestPlayerWeapon::TESTWEAPON_DESC Desc = {};
        Desc.pParentMatrix = m_pTransformCom->GetWorldMatrixPtr();
        Desc.pState = &m_iState;
        Desc.pSocketMatrix = static_cast<CTestPlayerBody*>(m_PartObjects[PART_BODY])->GetBoneCombiedMatrix("SWORD");

        m_PartObjects[PART_WEAPON] = static_cast<CPartObject*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_TestPlayerWeapon"), &Desc));
        if (nullptr == m_PartObjects[PART_WEAPON]) return E_FAIL;
    }

    return S_OK;
}

CTestPlayer* CTestPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTestPlayer* pInstance = new CTestPlayer(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CTestPlayer");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CTestPlayer::Clone(void* pArg)
{
    CTestPlayer* pInstance = new CTestPlayer(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CTestPlayer");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CTestPlayer::Free()
{
    __super::Free();
}
