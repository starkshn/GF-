#include "pch.h"
#include "Player.h"
#include "GameInstance.h"
#include "Collider.h"
#include "StateObjectBody.h"
#include "Navigation.h"
#include "GlobalFuncs.h"

#include "Level_Loading.h"
#include "LevelBoss.h"

#include "UIHPBar.h"
#include "UIFont.h"
#include <math.h>
#include "UITex.h"
#include "UICrossHair.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CStateObjectContainer(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
    :
    CStateObjectContainer(Prototype)
{
}

HRESULT CPlayer::InitializePrototype()
{
    return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
    m_fMouseSensor = 0.5f;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (m_iLevelIndex == LEVEL_BOSS)
        SetNavMeshIdx(CStateObjectContainer::NAVMESH_IDX::S_BOSS);

    if (FAILED(ReadyComponents()))
        return E_FAIL;

    if (FAILED(ReadyPartObjects()))
        return E_FAIL;

    m_pTransformCom->SetTransformState(CTransform::STATE_POS, _vector{0.f, 0.f, 0.f, 1.f});

    SetForceState(OBJECT_STATE::STATE_IDLE);
    m_iPrevState = OBJECT_STATE::STATE_IDLE;
    LockCursorToCenter();

    SetHP(200);
    InitHPUI();

    InitGunUI();

    return S_OK;
}

void CPlayer::PriorityUpdate(_float fTimeDelta)
{
    static _float fAcc = 0.f;
    if (!m_bGetDamageCoolDone)
    {
        fAcc += fTimeDelta;
        if (fAcc > 0.5f)
        {
            m_bGetDamageCoolDone = true;
            fAcc = false;
        }
    }

    UpdateCurrentEndAnimation();
    Keyinput(fTimeDelta);

    if (m_pNavigationComs[GetNavMeshIdx()])
    {
        _vector vPos = m_pNavigationComs[GetNavMeshIdx()]->SetUpHeightOnNavigation(m_pTransformCom->GetTransformState(CTransform::STATE_POS));
        vPos.m128_f32[1] += 1.5f;
        m_pTransformCom->SetTransformState(CTransform::STATE_POS, vPos);
    }
    __super::PriorityUpdate(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->GetWorldMatrix());
    __super::Update(fTimeDelta);


    if (m_bSkillMid)
    {
        m_fAccSkillMid += fTimeDelta;
        if (m_fAccSkillMid > m_fSkillMidCoolTime)
        {
            m_fAccSkillMid = 0.f;
            m_bSkillMid = false;


            auto pSkillMidAnim = static_cast<CUITex*>(m_pGameInst->GetEleUIFromPanel("UIPlayerSkillMidBG", "UIPlayerSkillMidAnim"));
            pSkillMidAnim->StartAnim();
        }
        else
        {
            auto pSkillMid = static_cast<CUITex*>(m_pGameInst->GetEleUIFromPanel("UIPlayerSkillMidBG", "UIPlayerSkillMid"));
            if (!pSkillMid)
            {
                MSG_BOX("CPlayer::KeySkillMid Error nullptr");
                return;
            }

            pSkillMid->SetUseTexCoordFlex();
            pSkillMid->SetTexCoordX(m_fAccSkillMid / m_fSkillMidCoolTime);
            
        }
    }

    static _float fAcc = 0.f;
    static _int iIdx = 0;

    if (m_iState & WALK)
    {
        fAcc += fTimeDelta;
        if (fAcc > 0.35f)
        {
            if (iIdx == 0)
            {
                m_pGameInst->PlayEffect(TEXT("PlayerWalk1.mp3"), 2.5f);
                iIdx = 1;
                fAcc = 0.f;
            }
            else if (iIdx == 1)
            {
                m_pGameInst->PlayEffect(TEXT("PlayerWalk2.mp3"), 2.5f);
                iIdx = 0;
                fAcc = 0.f;
            }
            // PlayerWalk2.mp3
        }
    }
    else
    {
        fAcc = 0.f;
    }
}

void CPlayer::LateUpdate(_float fTimeDelta)
{
    m_pGameInst->AddRenderObject(CRenderer::RG_NONBLEND, this);

    __super::LateUpdate(fTimeDelta);

#ifdef _DEBUG
    m_pGameInst->AddDebugComponent(m_pColliderCom);
#endif
}

HRESULT CPlayer::Render()
{
    // m_pColliderCom->Render();

    return S_OK;
}

void CPlayer::InitHPUI()
{
    
    {
        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerHPBlueFive"))->SetNum(1);
        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerHPBlueSix"))->SetNum(0);
        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerHPBlueSeven"))->SetNum(0);
        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerHPBlueFour"))->SetNum(11);


        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerHPGreenFour"))->SetNum(11);
        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerHPGreenFive"))->SetNum(1);
        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerHPGreenSix"))->SetNum(0);
        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerHPGreenSeven"))->SetNum(0);
    }

    if (m_iHP > 100)
    {
        static_cast<CUIHPBar*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerBPBarBlue"))->SetHPValPlayer(100);
        static_cast<CUIHPBar*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerBPBarGreen"))->SetHPValPlayer(100);


        _uint iHP = 100;
        _uint iOne = iHP / 100;
        _uint iTwo = (iHP % 100) / 10;
        _uint iThree = iHP % 10;

        // Blue
        {
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerHPBlueFirst"))->SetNum(iOne);
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerHPBlueTwo"))->SetNum(iTwo);
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerHPBlueThree"))->SetNum(iThree);
        }

        // Green
        {
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerHPGreenFirst"))->SetNum(1);
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerHPGreenTwo"))->SetNum(0);
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerHPGreenThree"))->SetNum(0);
        }
    }
    else
    {
        static_cast<CUIHPBar*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerBPBarBlue"))->SetHPValPlayer(0);
        static_cast<CUIHPBar*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerBPBarGreen"))->SetHPValPlayer(m_iHP);

        _uint iHP = (m_iHP % 100);
        _uint iOne = iHP / 100;
        _uint iTwo = (iHP % 100) / 10;
        _uint iThree = iHP % 10;


        // Blue
        {
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerHPBlueFirst"))->SetNum(0);
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerHPBlueTwo"))->SetNum(0);
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerHPBlueThree"))->SetNum(0);
        }


        // Green
        {
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerHPBlueFirst"))->SetNum(iOne);
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerHPBlueTwo"))->SetNum(iTwo);
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerHPBlueThree"))->SetNum(iThree);
        }
    }
}

void CPlayer::InitGunUI()
{
    _int iModelIdx = GetCurrentModelIndex();
    _int iBullet = m_CurMagCnt[iModelIdx];
    if (m_CurMagCnt[iModelIdx] < 0)
    {
        MSG_BOX("CurMag Can't Neg");
    }

    static string strBulletFonts[6] =
    {
        "UIPlayerBulletFontFirst", "UIPlayerBulletFontTwo", "UIPlayerBulletFontThree", "UIPlayerBulletFontFour", "UIPlayerBulletFontFive",
        "UIPlayerBulletFontSix"
    };


    // 2자리라면
    if (iBullet / 10)
    {
        _int iFirst = iBullet / 10;
        _int iTwo = iBullet % 10;

        m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[0])->ActiveRender();
        m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[1])->ActiveRender();

        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[0]))->SetNum(iFirst);
        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[1]))->SetNum(iTwo);
    }
    // 1자리 라면
    else
    {
        _int iTwo = iBullet % 10;
        m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[0])->DeActiveRender();
        m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[1])->ActiveRender();

        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[1]))->SetNum(iTwo);
    }


    {
        // 슬래쉬로 설정
        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[2]))->SetNum(11);
        if (0 == iModelIdx)
        {
            for (_int i = 4; i < 6; ++i)
                static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[i]))->DeActiveRender();

            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[3]))->ActiveRender();
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[3]))->SetNum(10);
        }
        else
        {
            // 3자리 
            if (m_CurMagCnt[iModelIdx] / 100)
            {
                for (_int i = 3; i < 6; ++i)
                    static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[i]))->ActiveRender();

                _int iTemp = m_CurMagCnt[iModelIdx];

                // 192
                static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[3]))->SetNum(iTemp / 100);
                iTemp %= 100;

                // 92
                static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[3]))->SetNum(iTemp / 10);
                iTemp %= 10;

                // 2
                static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[3]))->SetNum(m_CurMagCnt[iModelIdx] / 1);
            }
            // 두자리
            else if (m_CurMagCnt[iModelIdx] / 10)
            {
                static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[5]))->DeActiveRender();

                for (_int i = 3; i < 5; ++i)
                    static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[i]))->ActiveRender();

                _int iTemp = m_CurMagCnt[iModelIdx];

                // 92
                static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[3]))->SetNum(iTemp / 10);
                iTemp %= 10;

                static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[4]))->SetNum(iTemp);
            }
            // 한자리
            else
            {
                for (_int i = 4; i < 6; ++i)
                    static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[i]))->DeActiveRender();

                _int iTemp = m_CurMagCnt[iModelIdx];

                static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[3]))->ActiveRender();
                static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[3]))->SetNum(iTemp);
            }

        }

        
    }
}

void CPlayer::Keyinput(_float fTimeDelta)
{
    KeyMove(fTimeDelta);

    if (!m_bDash)
    {
        KeyFire(fTimeDelta);
        KeyReload(fTimeDelta);
        KeySwap(fTimeDelta);
        KeyDash(fTimeDelta);

    }
    else
    {
        UpdateDash(fTimeDelta);
    }

    KeySkillMid(fTimeDelta);
    UpdateDashUI(fTimeDelta);


    if (m_1508Fire)
    {
        static _float fAcc = 0.f;
        static _int iNum = 0;
        fAcc += fTimeDelta;
        if (fAcc > 0.1f)
        {
            fAcc = 0.f;
            ++iNum;
            // m_pGameInst->PlayEffect(TEXT("1508_Fire2.mp3"), 1.f);
            m_pGameInst->PlayEffect(TEXT("1508_Fire.ogg"), 1.f);
            if (iNum >= 2)
            {
                iNum = 0;
                m_1508Fire = false;
            }
        }
    }

}

void CPlayer::KeyMove(_float fTimeDelta)
{
    if (!m_pNavigationComs[GetNavMeshIdx()])
        return;

    if (m_pGameInst->KeyDown(KEY::W))
    {
        m_iMovePressCount++;

        // FRONT추가
        m_iKeyDir |= FRONT;

        if (IDLE & m_iState)
            SetForceState(WALK);
        else if ((RELOAD & m_iState) && (RELOAD ^ m_iReserveState))
            SetReserverState(WALK);
        else if ((PUT & m_iState) && (PUT ^ m_iReserveState))
            SetReserverState(WALK);
        else if ((DRAW & m_iState) && (DRAW ^ m_iReserveState))
            SetReserverState(WALK);
        else if ((FIRE & m_iState) && (FIRE ^ m_iReserveState))
            SetReserverState(WALK);
    }
    else if (m_pGameInst->KeyPressing(KEY::W))
    {
        if (IDLE & m_iState)
            SetForceState(WALK);
        else if ((RELOAD & m_iState) && (RELOAD ^ m_iReserveState))
            SetReserverState(WALK);
        else if ((PUT & m_iState) && (PUT ^ m_iReserveState))
            SetReserverState(WALK);
        else if ((DRAW & m_iState) && (DRAW ^ m_iReserveState))
            SetReserverState(WALK);
        else if ((FIRE & m_iState) && (FIRE ^ m_iReserveState))
            SetReserverState(WALK);

        if (1 == m_iMovePressCount)
            m_pTransformCom->GoFoward(fTimeDelta, m_pNavigationComs[GetNavMeshIdx()]);
    }
    else if (m_pGameInst->KeyUp(KEY::W))
    {
        m_iMovePressCount--;

        if (m_iKeyDir & FRONT)
            m_iKeyDir &= ~FRONT;

        // 누른 키가 하나도 없을 때
        if (0 == m_iMovePressCount)
        {
            if (WALK & m_iState)
                SetForceState(IDLE);
            else if (FIRE & m_iState)
                SetReserverState(IDLE);
            else if (RELOAD & m_iState)
                SetReserverState(IDLE);
            else if (PUT & m_iState)
                SetReserverState(IDLE);
            else if (DRAW & m_iState)
                SetReserverState(IDLE);
            else if (FIRE & m_iState)
                SetReserverState(IDLE);
        }
    }

    if (m_pGameInst->KeyDown(KEY::S))
    {
        m_iMovePressCount++;

        m_iKeyDir |= BACK;

        if (IDLE & m_iState)
            SetForceState(WALK);
        else if ((RELOAD & m_iState) && (RELOAD ^ m_iReserveState))
            SetReserverState(WALK);
        else if ((PUT & m_iState) && (PUT ^ m_iReserveState))
            SetReserverState(WALK);
        else if ((DRAW & m_iState) && (DRAW ^ m_iReserveState))
            SetReserverState(WALK);
        else if ((FIRE & m_iState) && (FIRE ^ m_iReserveState))
            SetReserverState(WALK);
    }
    else if (m_pGameInst->KeyPressing(KEY::S))
    {
        if (IDLE & m_iState)
            SetForceState(WALK);
        else if ((RELOAD & m_iState) && (RELOAD ^ m_iReserveState))
            SetReserverState(WALK);
        else if ((PUT & m_iState) && (PUT ^ m_iReserveState))
            SetReserverState(WALK);
        else if ((DRAW & m_iState) && (DRAW ^ m_iReserveState))
            SetReserverState(WALK);
        else if ((FIRE & m_iState) && (FIRE ^ m_iReserveState))
            SetReserverState(WALK);

        if (1 == m_iMovePressCount)
            m_pTransformCom->GoBackward(fTimeDelta, m_pNavigationComs[GetNavMeshIdx()]);
    }
    else if (m_pGameInst->KeyUp(KEY::S))
    {
        m_iMovePressCount--;

        if (m_iKeyDir & BACK)
            m_iKeyDir &= ~BACK;

        // 누른 키가 하나도 없을 때
        if (0 == m_iMovePressCount)
        {
            if (WALK & m_iState)
                SetForceState(IDLE);
            else if (FIRE & m_iState)
                SetReserverState(IDLE);
            else if (RELOAD & m_iState)
                SetReserverState(IDLE);
            else if (PUT & m_iState)
                SetReserverState(IDLE);
            else if (DRAW & m_iState)
                SetReserverState(IDLE);
            else if (FIRE & m_iState)
                SetReserverState(IDLE);
        }
    }

    if (m_pGameInst->KeyDown(KEY::A))
    {
        m_iMovePressCount++;

        m_iKeyDir |= LEFT;

        if (IDLE & m_iState)
            SetForceState(WALK);
        else if ((RELOAD & m_iState) && (RELOAD ^ m_iReserveState))
            SetReserverState(WALK);
        else if ((PUT & m_iState) && (PUT ^ m_iReserveState))
            SetReserverState(WALK);
        else if ((DRAW & m_iState) && (DRAW ^ m_iReserveState))
            SetReserverState(WALK);
        else if ((FIRE & m_iState) && (FIRE ^ m_iReserveState))
            SetReserverState(WALK);
    }
    else if (m_pGameInst->KeyPressing(KEY::A))
    {
        if (IDLE & m_iState)
            SetForceState(WALK);
        else if ((RELOAD & m_iState) && (RELOAD ^ m_iReserveState))
            SetReserverState(WALK);
        else if ((PUT & m_iState) && (PUT ^ m_iReserveState))
            SetReserverState(WALK);
        else if ((PUT & m_iState) && (PUT ^ m_iReserveState))
            SetReserverState(WALK);
        else if ((DRAW & m_iState) && (DRAW ^ m_iReserveState))
            SetReserverState(WALK);
        else if ((FIRE & m_iState) && (FIRE ^ m_iReserveState))
            SetReserverState(WALK);

        if (1 == m_iMovePressCount)
            m_pTransformCom->GoLeft(fTimeDelta, m_pNavigationComs[GetNavMeshIdx()]);
    }
    else if (m_pGameInst->KeyUp(KEY::A))
    {
        m_iMovePressCount--;

        if (m_iKeyDir & LEFT)
            m_iKeyDir &= ~LEFT;

        // 누른 키가 하나도 없을 때
        if (0 == m_iMovePressCount)
        {
            if (WALK & m_iState)
                SetForceState(IDLE);
            else if (FIRE & m_iState)
                SetReserverState(IDLE);
            else if (RELOAD & m_iState)
                SetReserverState(IDLE);
            else if (PUT & m_iState)
                SetReserverState(IDLE);
            else if (DRAW & m_iState)
                SetReserverState(IDLE);
            else if (FIRE & m_iState)
                SetReserverState(IDLE);
        }
    }

    if (m_pGameInst->KeyDown(KEY::D))
    {
        m_iMovePressCount++;
        m_iKeyDir |= RIGHT;

        if (IDLE & m_iState)
            SetForceState(WALK);
        else if ((RELOAD & m_iState) && (RELOAD ^ m_iReserveState))
            SetReserverState(WALK);
        else if ((PUT & m_iState) && (PUT ^ m_iReserveState))
            SetReserverState(WALK);
        else if ((DRAW & m_iState) && (DRAW ^ m_iReserveState))
            SetReserverState(WALK);
        else if ((FIRE & m_iState) && (FIRE ^ m_iReserveState))
            SetReserverState(WALK);
    }
    else if (m_pGameInst->KeyPressing(KEY::D))
    {
        if (IDLE & m_iState)
            SetForceState(WALK);
        else if ((RELOAD & m_iState) && (RELOAD ^ m_iReserveState))
            SetReserverState(WALK);
        else if ((PUT & m_iState) && (PUT ^ m_iReserveState))
            SetReserverState(WALK);
        else if ((DRAW & m_iState) && (DRAW ^ m_iReserveState))
            SetReserverState(WALK);
        else if ((FIRE & m_iState) && (FIRE ^ m_iReserveState))
            SetReserverState(WALK);

        if (1 == m_iMovePressCount)
            m_pTransformCom->GoRight(fTimeDelta, m_pNavigationComs[GetNavMeshIdx()]);
    }
    else if (m_pGameInst->KeyUp(KEY::D))
    {
        m_iMovePressCount--;

        if (m_iKeyDir & RIGHT)
            m_iKeyDir &= ~RIGHT;

        // 누른 키가 하나도 없을 때
        if (0 == m_iMovePressCount)
        {
            if (WALK & m_iState)
                SetForceState(IDLE);
            else if (FIRE & m_iState)
                SetReserverState(IDLE);
            else if (RELOAD & m_iState)
                SetReserverState(IDLE);
            else if (PUT & m_iState)
                SetReserverState(IDLE);
            else if (DRAW & m_iState)
                SetReserverState(IDLE);
            else if (FIRE & m_iState)
                SetReserverState(IDLE);
        }
    }

    if (2 == m_iMovePressCount)
    {
        if ((RIGHT & m_iKeyDir) && (FRONT & m_iKeyDir))
            m_pTransformCom->GoRightFoward(fTimeDelta, m_pNavigationComs[GetNavMeshIdx()]);
        else if ((RIGHT & m_iKeyDir) && (BACK & m_iKeyDir))
            m_pTransformCom->GoRightBack(fTimeDelta, m_pNavigationComs[GetNavMeshIdx()]);
        else if ((LEFT & m_iKeyDir) && (FRONT & m_iKeyDir))
            m_pTransformCom->GoLeftFoward(fTimeDelta, m_pNavigationComs[GetNavMeshIdx()]);
        else if ((LEFT & m_iKeyDir) && (BACK & m_iKeyDir))
            m_pTransformCom->GoLeftBack(fTimeDelta, m_pNavigationComs[GetNavMeshIdx()]);
    }
    else if (3 <= m_iMovePressCount)
    {
        
    }

    if (m_pGameInst->KeyDown(KEY::TAB))
        m_bLockCursorPos ? m_bLockCursorPos = false : m_bLockCursorPos = true;

    if (m_bLockCursorPos)
        LockCursorToCenter();

    _long MouseMove = {};

    if (MouseMove = m_pGameInst->GetMouseMove(MOUSE_MOVE::X))
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * fTimeDelta * m_fMouseSensor);

    if (MouseMove = m_pGameInst->GetMouseMove(MOUSE_MOVE::Y))
        m_pTransformCom->Turn(m_pTransformCom->GetTransformState(CTransform::STATE_RIGHT), MouseMove * fTimeDelta * m_fMouseSensor);
}

void CPlayer::KeyReload(_float fTimeDelta)
{
    if ((IDLE & m_iState) || (WALK & m_iState) || (JUMP & m_iState))
    {
        if (m_pGameInst->KeyDown(KEY::R))
        {
            SetForceState(RELOAD);
            UpdateUIReload();
        }
    }
}

void CPlayer::KeyFire(_float fTimeDelta)
{
    if (m_pGameInst->MouseDown(MOUSE_KEY::LB) && CheckMag())
    {
        if (false == m_bStartAcc)
        {
            if ((m_iState & IDLE) || (m_iState & WALK))
            {
                _int iFire = GetPlayerBody()->GetAnimationIndexByAnimationName("FIRE", GetCurrentModelIndex());
                _int iCur = GetPlayerBody()->GetCurrentPlayingAnimIndex();
                _uint iModel = GetPlayerBody()->GetCurrentModelIndex();

                if (iFire != iCur)
                {
                    if (0 == GetCurrentModelIndex() || 1 == GetCurrentModelIndex())
                    {
                        m_iDefaultFireCount++;
                        m_iDefaultFireCount %= m_iDefaultFireCountMax;
                    }

                    RememberPrevState();
                    SetForceState(FIRE);
                    ActivateGunMuzzleEffect(m_iDefaultFireCount);
                    GetPlayerWeapon()->Fire();
                    m_bStartAcc = true;

                    UpdateUIGunFire();

                    m_bStartAccFontSize = true;

                    static_cast<CUICrossHair*>(m_pGameInst->GetEleUI("UIPlayerCrossHair"))->StartScaling();


                    if (iModel == 0)
                    {
                        m_pGameInst->PlayEffect(TEXT("1205_Fire.ogg"), 1.f);
                    }
                    else if (iModel == 1)
                    {
                        m_pGameInst->PlayEffect(TEXT("1004_Fire.ogg"), 1.f);

                    }
                    else if (iModel == 2)
                    {
                        m_pGameInst->PlayEffect(TEXT("1503_Fire3.ogg"), 1.f);
                    }
                    else if (iModel == 3)
                    {
                        m_1508Fire = true;
                        // m_pGameInst->PlayEffect(TEXT("1508_Fire2.mp3"), 1.f);
                        m_pGameInst->PlayEffect(TEXT("1508_Fire.ogg"), 1.f);
                    }
                }

               
                
            }
            else if (m_iState & FIRE)
            {
                _int iFire = GetPlayerBody()->GetAnimationIndexByAnimationName("FIRE", GetCurrentModelIndex());
                _int iCur = GetPlayerBody()->GetCurrentPlayingAnimIndex();
                _uint iModel = GetPlayerBody()->GetCurrentModelIndex();

                if (iFire == iCur)
                {
                    if (0 == GetCurrentModelIndex() || 1 == GetCurrentModelIndex())
                    {
                        m_iDefaultFireCount++;
                        m_iDefaultFireCount %= m_iDefaultFireCountMax;
                    }

                    SetResetState(FIRE);
                    ActivateGunMuzzleEffect(m_iDefaultFireCount);
                    GetPlayerWeapon()->Fire();
                    m_bStartAcc = true;
                    m_fAccAttackCoolTime = 0.f;

                    UpdateUIGunFire();
                    m_bStartAccFontSize = true;
                    static_cast<CUICrossHair*>(m_pGameInst->GetEleUI("UIPlayerCrossHair"))->StartScaling();


                    if (iModel == 0)
                    {
                        m_pGameInst->PlayEffect(TEXT("1205_Fire.ogg"), 1.f);
                    }
                    else if (iModel == 1)
                    {
                        m_pGameInst->PlayEffect(TEXT("1004_Fire.ogg"), 1.f);
                    }
                    else if (iModel == 3)
                    {
                        
                    }

                }
            }
            else if ((m_iState & JUMP))
            {

            }
        }
    }

    if (m_bStartAcc)
    {
        m_fAccAttackCoolTime += fTimeDelta;
        if (m_fAccAttackCoolTime >= m_fAttackCoolTimes[GetCurrentModelIndex()])
        {
            m_fAccAttackCoolTime = 0.f;
            m_bStartAcc = false;
        }
    }


    // UIPlayerGunInfoBG
    if (m_bStartAccFontSize)
    {
        const static _float fMaxTime = 0.2f;
        static _float fAcc = 0.f;
        _int iCurModel = GetCurrentModelIndex();

        // todo
        fAcc += fTimeDelta;
        if (fAcc < fMaxTime / 2.f)
        {
            CUIFont* pUIFont = static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", "UIPlayerBulletFontTwo"));
            if (pUIFont)
            {
                pUIFont->StartIncreaseScaling();
            }
        }
        else if (fAcc < fMaxTime)
        {
            CUIFont* pUIFont = static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", "UIPlayerBulletFontTwo"));
            if (pUIFont)
            {
                pUIFont->StartDecreaseScaling();
            }
        }
        else
        {
            fAcc = 0.f;
            m_bStartAccFontSize = false;

            CUIFont* pUIFont = static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", "UIPlayerBulletFontTwo"));
            if (pUIFont)
            {
                pUIFont->StopScaling();
            }
        }
    }
}

void CPlayer::KeySwap(_float fTimeDelta)
{
    if ((m_iState & IDLE) || (m_iState & WALK))
    {
        if (m_pGameInst->KeyDown(KEY::NUM1))
        {
            if (PLAYER_MODEL_INDEX::RIGHT_1201 != (PLAYER_MODEL_INDEX)m_iNextDrawModelIndex)
                ReadyToPlayPutAnimation(static_cast<_uint>(PLAYER_MODEL_INDEX::RIGHT_1201));

            UpdateUIGunMagSwap(0);
        }
        else if (m_pGameInst->KeyDown(KEY::NUM2))
        {
            if (PLAYER_MODEL_INDEX::RIGHT_1004 != (PLAYER_MODEL_INDEX)m_iNextDrawModelIndex)
                ReadyToPlayPutAnimation(static_cast<_uint>(PLAYER_MODEL_INDEX::RIGHT_1004));

            UpdateUIGunMagSwap(1);
        }
            
        else if (m_pGameInst->KeyDown(KEY::NUM3))
        {
            if (PLAYER_MODEL_INDEX::RIGHT_1503 != (PLAYER_MODEL_INDEX)m_iNextDrawModelIndex)
                ReadyToPlayPutAnimation(static_cast<_uint>(PLAYER_MODEL_INDEX::RIGHT_1503));

            UpdateUIGunMagSwap(2);
        }

        else if (m_pGameInst->KeyDown(KEY::NUM4))
        {
            if (PLAYER_MODEL_INDEX::RIGHT_1508 != (PLAYER_MODEL_INDEX)m_iNextDrawModelIndex)
                ReadyToPlayPutAnimation(static_cast<_uint>(PLAYER_MODEL_INDEX::RIGHT_1508));

            UpdateUIGunMagSwap(3);
        }

        RememberPrevState();
    }
}

// UITexPlayerDashEffect
void CPlayer::KeyDash(_float fTimeDelta)
{
    if (!m_bDashReStart)
        return;

    if (m_pGameInst->KeyDown(KEY::LSHIFT))
    {
        if (((WALK & m_iState) || RELOAD & m_iState) && m_iMovePressCount == 1)
        {
            if (LEFT & m_iKeyDir)
                m_iDashDir = LEFT;
            else if (RIGHT & m_iKeyDir)
                m_iDashDir = RIGHT;
            else if (FRONT & m_iKeyDir)
                m_iDashDir = FRONT;
            else if (BACK & m_iKeyDir)
                m_iDashDir = BACK;

            m_bDash = true;

            auto pDashUI = static_cast<CUITex*>(m_pGameInst->GetEleUI("UITexPlayerDashEffect"));
            if (pDashUI)
            {
                pDashUI->ActiveRender();
                pDashUI->StartAlphaEffect();
            }


            static _int g_iRand = 0;

            g_iRand = (_int)m_pGameInst->ComputeRandom(0.f, 1.99f);
            if (g_iRand == 0)
                m_pGameInst->PlayEffect(TEXT("PlayerDash.mp3"), 1.f);
            else if (g_iRand == 1)
                m_pGameInst->PlayEffect(TEXT("PlayerDash2.mp3"), 1.f);
        }
    }
}

// UIPlayerSkillMidBG : parent
// UIPlayerSkillMid : child
// UIPlayerSkillMidAnim
void CPlayer::KeySkillMid(_float fTimeDelta)
{
    if (m_pGameInst->KeyDown(KEY::Q))
    {
        if (((WALK & m_iState) || IDLE & m_iState))
        {
            m_bSkillMid = true;
            auto pSkillMid = static_cast<CUITex*>(m_pGameInst->GetEleUIFromPanel("UIPlayerSkillMidBG", "UIPlayerSkillMid"));
            if (!pSkillMid)
            {
                MSG_BOX("CPlayer::KeySkillMid Error nullptr");
                return;
            }

            pSkillMid->SetUseTexCoordFlex();
            pSkillMid->SetTexCoordX(0.f);

            /*CUITex* pUI = static_cast<CUITex*>(m_pGameInst->GetEleUI("UITexVictory"));
            if (pUI)
            {
                CUITex::UI_TEX_SCALE_DESC Desc;
                Desc.fScalingMaxSizeX = 800.f;
                Desc.fScalingMaxSizeY = 600.f;
                Desc.fScalingMinSizeX = 0.f;
                Desc.fScalingMinSizeY = 0.f;

                Desc.bHold = true;
                Desc.fHoldTime = 2.f;

                pUI->SetScalingDesc(Desc);
                pUI->StartScaling();
            }*/


            auto pSkillMidAnim = static_cast<CUITex*>(m_pGameInst->GetEleUIFromPanel("UIPlayerSkillMidBG", "UIPlayerSkillMidAnim"));
            pSkillMidAnim->StopAnim();
        }
    }
}

void CPlayer::LockCursorToCenter()
{
    // 클라이언트 영역의 가운데 위치 계산
    RECT rect;
    GetClientRect(g_hWnd, &rect);
    _int centerX = (rect.right - rect.left) / 2;
    _int centerY = (rect.bottom - rect.top) / 2;

    // 화면 좌표로 변환
    POINT centerPos = { centerX, centerY };
    ClientToScreen(g_hWnd, &centerPos);

    // 커서를 화면 가운데로 이동
    SetCursorPos(centerPos.x, centerPos.y);
}

void CPlayer::UpdateCurrentEndAnimation()
{
    if (false == strcmp("FIRE", m_strCurrentEndAnimationName.c_str()))
    {
        if (RESET == m_iReserveState)
            SetForceState(IDLE);
        else
            SetForceState(m_iReserveState);

        ResetReserveState();
        m_strCurrentEndAnimationName = "NONE";
    }
    else if (false == strcmp("RELOAD", m_strCurrentEndAnimationName.c_str()))
    {   
        if (RESET == m_iReserveState)
            SetForceState(IDLE);
        else
            SetForceState(m_iReserveState);

        ResetReserveState();
        m_strCurrentEndAnimationName = "NONE";
    }
    else if (false == strcmp("PUT", m_strCurrentEndAnimationName.c_str()))
    {
        cout << "Accep Alert PUT End" << endl;
        SetForceState(DRAW);
        ResetReserveState();
        m_strCurrentEndAnimationName = "NONE";
    }
    else if (false == strcmp("DRAW", m_strCurrentEndAnimationName.c_str()))
    {
        cout << "Accep Alert DRAW End" << endl;

        if (RESET == m_iReserveState)
        {
            SetForceState(IDLE);
        }
        else
        {
            SetForceState(m_iReserveState);
        }
        
        ResetReserveState();
        m_strCurrentEndAnimationName = "NONE";

        CUITex* pUITex = static_cast<CUITex*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", "UIPlayerGunTex"));
        if (pUITex)
        {
            pUITex->SetGunTexIdx(GetCurrentModelIndex());
        }
    }
}

CPlayerBody* CPlayer::GetPlayerBody()
{
    return static_cast<CPlayerBody*>(m_PartObjects[PART_BODY]);
}

CPlayerWeapon* CPlayer::GetPlayerWeapon()
{
    return static_cast<CPlayerWeapon*>(m_PartObjects[PART_WEAPON]);
}

_uint CPlayer::GetCurrentModelIndex()
{
    return GetPlayerBody()->GetCurrentModelIndex();
}

void CPlayer::ReadyToPlayPutAnimation(_uint iNextDrawModelIndex)
{
    SetForceState(PUT);
    SetNextDrawModelIndex(iNextDrawModelIndex);   
}

void CPlayer::ActivateGunMuzzleEffect(_uint iGunFireCount)
{
    dynamic_cast<CPlayerWeapon*>(m_PartObjects[PART_WEAPON])->ActivateGunMuzzleEffect(GetCurrentModelIndex());
}


// Layer_UI_PlayerInfoBG
// UIPlayerInfoBG
// UIPlayerBPBarBlue
// UIPlayerBPBarGreen


// UIPlayerHPBlueFirst

// UITexPlayerDamagedEffect
void CPlayer::SetDamage(_int iDamage)
{
    if (!m_bGetDamageCoolDone)
        return;

    m_bGetDamageCoolDone = false;

    __super::SetDamage(iDamage);

    if (m_iHP < 0)
        return;

    UpdateHPUI();

    auto pUITex = static_cast<CUITex*>(m_pGameInst->GetEleUI("UITexPlayerDamagedEffect"));
    if (pUITex)
    {
        pUITex->ActiveRender();
        pUITex->StartAlphaEffect();
    }
    

    static _int iRand = 1;
    if (iRand == 0)
    {
        m_pGameInst->PlayEffect(TEXT("PlayerGetDamage1.mp3"), 1.f);
        iRand = 1;
    }
    else if (iRand == 1)
    {
        m_pGameInst->PlayEffect(TEXT("PlayerGetDamage2.mp3"), 1.f);
        iRand = 0;
    }
}

void CPlayer::UpdateHPUI()
{
    _int iHalf = 200 >> 1;

    if (iHalf < m_iHP)
    {
        static_cast<CUIHPBar*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerBPBarBlue"))->SetHPValPlayer(m_iHP % 100);
        static_cast<CUIHPBar*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerBPBarGreen"))->SetHPValPlayer(100);

        _uint iHP = m_iHP - 100;
        _uint iMod = 100;

        for (_int i = 0; i < 3; ++i)
        {
            m_iFontBlueNums[i] = iHP / iMod;
            iHP -= m_iFontBlueNums[i] * iMod;
            iMod /= 10;
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", m_strHPFontBlueNames[i]))->SetNum(m_iFontBlueNums[i]);
        }
    }
    else
    {
        static_cast<CUIHPBar*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerBPBarBlue"))->SetHPValPlayer(0);
        static_cast<CUIHPBar*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerBPBarGreen"))->SetHPValPlayer(m_iHP);
        
        // FontBlue
        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerHPBlueFirst"))->SetNum(0);
        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerHPBlueTwo"))->SetNum(0);
        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", "UIPlayerHPBlueThree"))->SetNum(0);

        _uint iHP = m_iHP;
        _uint iMod = 100;

        for (_int i = 0; i < 3; ++i)
        {
            m_iFontGreenNums[i] = iHP / iMod;
            iHP -= m_iFontGreenNums[i] * iMod;
            iMod /= 10;
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerInfoBG", m_strHPFontGreenNames[i]))->SetNum(m_iFontGreenNums[i]);
        }
    }
}

_bool CPlayer::CheckMag()
{
    return m_CurMagCnt[GetCurrentModelIndex()];
}


// UIPlayerGunInfoBG
void CPlayer::UpdateUIGunFire()
{
    _int iModelIdx = GetCurrentModelIndex();
    _int iBullet = --m_CurMagCnt[iModelIdx];

    if (m_CurMagCnt[iModelIdx] < 0)
    {
        MSG_BOX("CurMag Can't Neg");
    }

    static string strBulletFonts[6] =
    {
        "UIPlayerBulletFontFirst", "UIPlayerBulletFontTwo", "UIPlayerBulletFontThree", "UIPlayerBulletFontFour", "UIPlayerBulletFontFive", \
        "UIPlayerBulletFontSix"
    };

    // 2자리라면
    if (iBullet / 10)
    {
        _int iFirst = iBullet / 10;
        _int iTwo = iBullet % 10;

        m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[0])->ActiveRender();
        m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[1])->ActiveRender();

        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[0]))->SetNum(iFirst);
        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[1]))->SetNum(iTwo);
    }
    // 1자리 라면
    else
    {
        _int iTwo = iBullet % 10;
        m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[0])->DeActiveRender();
        m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[1])->ActiveRender();

        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", strBulletFonts[1]))->SetNum(iTwo);
    }


    // GameInst->GetEleUIFromPanel("");
}

void CPlayer::UpdateUIReload()
{
    _int iCurModelIdx = GetCurrentModelIndex();

    if (!m_MagCnt[iCurModelIdx])
        return;

    if (0 == iCurModelIdx)
    {
        m_CurMagCnt[iCurModelIdx] = 9;
        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[1]))->SetNum(9);
    }
    else
    {
        _int iMaxGunMag = m_MaxMagCnt[iCurModelIdx];
        _int iCurBulletCnt = m_CurMagCnt[iCurModelIdx];
        _int iFillMagCnt = m_MaxMagCnt[iCurModelIdx] - iCurBulletCnt;

        if (0 >= m_MagCnt[iCurModelIdx] && iCurModelIdx > 0)
        {
            MSG_BOX("Bullet Cnt : 0");
        }

        if (m_MagCnt[iCurModelIdx] >= iFillMagCnt)
        {
            m_MagCnt[iCurModelIdx] -= iFillMagCnt;
        }
        else
        {
            iFillMagCnt = m_MagCnt[iCurModelIdx];
            m_MagCnt[iCurModelIdx] = 0;
        }

        m_CurMagCnt[iCurModelIdx] += iFillMagCnt;

        _int iCurCnt = m_CurMagCnt[iCurModelIdx];

        // 두자리 라면
        if (m_CurMagCnt[iCurModelIdx] / 10)
        {
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[0]))->SetNum(iCurCnt / 10);
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[1]))->SetNum(iCurCnt % 10);

            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[0]))->ActiveRender();
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[1]))->ActiveRender();
        }
        // 한자리 라면
        else if (m_CurMagCnt[iCurModelIdx] % 10)
        {
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[1]))->SetNum(iCurCnt % 10);
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[1]))->ActiveRender();
        }
        // 0이라면
        else
        {
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[1]))->SetNum(0);
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[1]))->ActiveRender();
        }

        _int iMagCnt = m_MagCnt[iCurModelIdx];
        if (iMagCnt / 100)
        {
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[3]))->ActiveRender();
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[4]))->ActiveRender();
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[5]))->ActiveRender();

            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[3]))->SetNum(iMagCnt / 100);
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[4]))->SetNum((iMagCnt % 100) / 10);
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[5]))->SetNum((iMagCnt % 10));
        }
        else if (iMagCnt / 10)
        {
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[3]))->ActiveRender();
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[4]))->ActiveRender();
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[5]))->DeActiveRender();

            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[3]))->SetNum((iMagCnt % 100) / 10);
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[4]))->SetNum(iMagCnt % 10);
        }
        else if (iMagCnt % 10)
        {
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[3]))->ActiveRender();
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[4]))->DeActiveRender();
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[5]))->DeActiveRender();

            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[3]))->SetNum(iMagCnt % 10);
        }
        else
        {
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[3]))->ActiveRender();
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[4]))->DeActiveRender();
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[5]))->DeActiveRender();

            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[3]))->SetNum(0);
        }
    }
}

void CPlayer::UpdateUIGunMagSwap(_int iNextModel)
{

    _int iCurBulletCnt = m_CurMagCnt[iNextModel];
    _int iCurMagCnt = m_MagCnt[iNextModel];

    if (0 == iNextModel)
    {
        // Render
        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[0]))->DeActiveRender();
        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[1]))->ActiveRender();
        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[3]))->ActiveRender();
        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[4]))->DeActiveRender();
        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[5]))->DeActiveRender();

        // Bullet, Mag Cnt
        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[1]))->SetNum(iCurBulletCnt);
        static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[3]))->SetNum(10);
    }
    else
    {
        // 두자리 인 경우
        if (iCurBulletCnt / 10)
        {
            // Render
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[0]))->ActiveRender();
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[1]))->ActiveRender();

            // Bullet
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[0]))->SetNum(iCurBulletCnt / 10);
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[1]))->SetNum(iCurBulletCnt % 10);
        }
        // 한자리 인 경우
        else if (iCurBulletCnt % 10)
        {
            // Render
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[0]))->DeActiveRender();
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[1]))->ActiveRender();

            // Bullet
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[1]))->SetNum(iCurBulletCnt % 10);
        }
        // 0 인경우
        else
        {
            // Render
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[0]))->DeActiveRender();
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[1]))->ActiveRender();

            // Bullet
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[1]))->SetNum(0);
        }

        _int iTmp = iCurMagCnt;
        // Mag
        if (iCurMagCnt / 100) // 3자리
        {
            // Render
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[3]))->ActiveRender();
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[4]))->ActiveRender();
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[5]))->ActiveRender();

            // Bullet
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[3]))->SetNum(iTmp / 100);
            iTmp -= (100 * (iTmp / 100));

            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[4]))->SetNum(iTmp / 10);
            iTmp -= (10 * (iTmp / 10));

            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[3]))->SetNum(iTmp % 10);
        }
        // 2자리 인경우
        else if (iCurMagCnt / 10)
        {
            // Render
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[3]))->ActiveRender();
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[4]))->ActiveRender();
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[5]))->DeActiveRender();


            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[3]))->SetNum(iTmp / 10);
            iTmp -= (10 * (iTmp / 10));

            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[4]))->SetNum(iTmp % 10);
        }
        // 한자리 인경우
        else if (iCurMagCnt % 10)
        {
            // Render
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[3]))->ActiveRender();
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[4]))->DeActiveRender();
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[5]))->DeActiveRender();

            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[3]))->SetNum(iTmp % 10);

        }
        // 0인 경우
        else
        {
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[3]))->ActiveRender();
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[4]))->DeActiveRender();
            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[5]))->DeActiveRender();

            static_cast<CUIFont*>(m_pGameInst->GetEleUIFromPanel("UIPlayerGunInfoBG", m_strBulletFonts[3]))->SetNum(0);
        }
    }
}

void CPlayer::UpdateDash(_float fTimeDelta)
{
    static _float fAcc = 0.f;

    fAcc += fTimeDelta;

    if (fAcc >= m_fDashTime)
    {
        fAcc = 0.f;
        // UpdateDashUI(fAcc);
        m_bDash = false;
        return;
    }

    if (LEFT & m_iDashDir)
    {
        m_pTransformCom->GoLeft(fTimeDelta * m_fDashSP, m_pNavigationComs[GetNavMeshIdx()]);
    }
    else if (RIGHT & m_iDashDir)
    {
        m_pTransformCom->GoRight(fTimeDelta * m_fDashSP, m_pNavigationComs[GetNavMeshIdx()]);
    }
    else if (FRONT & m_iDashDir)
    {
        m_pTransformCom->GoFoward(fTimeDelta * m_fDashSP, m_pNavigationComs[GetNavMeshIdx()]);
    }
    else if (BACK & m_iDashDir)
    {
        m_pTransformCom->GoBackward(fTimeDelta * m_fDashSP, m_pNavigationComs[GetNavMeshIdx()]);
    }
}

// UIPlayerSkillBG
void CPlayer::UpdateDashUI(_float fTimeDelta)
{
    static _bool bDash = false;
    static _float fCurRatio = 0.f;
    const static _float fCoolTime = 1.f;

    if (m_bDash)
    {
        m_bDashReStart = false;
        bDash = true;
    }

    if (bDash)
    {
        fCurRatio += fTimeDelta;

        if (fCurRatio > fCoolTime)
        {
            fCurRatio = fCoolTime;
            bDash = false;
            m_bDashReStart = true;
        }

        CUITex* pUITex = static_cast<CUITex*>(m_pGameInst->GetEleUIFromPanel("UIPlayerSkillBG", "UIPlayerSkillDash"));
        if (pUITex)
        {
            pUITex->SetUseTexCoordFlex();
            pUITex->SetTexCoordX(fCurRatio);
        }

        if (!bDash)
            fCurRatio = 0.f;
    }
}

void CPlayer::UpdateDashUIEffect(_float fTimeDelta)
{

}

HRESULT CPlayer::ReadyComponents()
{
    // Com_Collider
    {
        CBoundingAABB::BOUND_AABB_DESC		ColliderDesc{};
        ColliderDesc.vExtents = _float3(0.3f, 0.7f, 0.3f);
        ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y - 1.5f, 0.f);

        if (FAILED(AddComponent(LEVEL_GLOBAL, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
            return E_FAIL;

        m_pColliderCom->SetComOwner(this);
    }

    /* Com_Navigation */
    {
        if (FAILED(ReadyNavMesh()))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CPlayer::ReadyPartObjects()
{
    // Body
    {
        CStateObjectBody::STATE_OBJECT_BODY_DESC Desc = {};
        Desc.pParentMatrix = m_pTransformCom->GetWorldMatrixPtr();
        Desc.pState = &m_iState;
        Desc.pPrevState = &m_iPrevState;
        Desc.pOwner = this;
        Desc.iNumModels = 4;
        Desc.strObjectTag = "PLAYER_BODY";
        Desc.iLevelIndex = m_iLevelIndex;

        m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_GLOBAL, TEXT("Prototype_GameObject_PlayerBody"), &Desc));
        if (nullptr == m_PartObjects[PART_BODY]) 
            return E_FAIL;
    }

    // Weapon
    {
        CPlayerWeapon::PLAYER_WEAPON_DESC PlayerWeaponDesc = {};

        PlayerWeaponDesc.pParentMatrix = m_pTransformCom->GetWorldMatrixPtr();

        PlayerWeaponDesc.iNumModels = 4;

        vector<const _float4x4*> pSocketMatrices;
        CPlayerBody* pBody = static_cast<CPlayerBody*>(m_PartObjects[PART_BODY]);
        PlayerWeaponDesc.pSocketMatrix = pBody->GetModelSocketMatrix("Bip001 Prop1", 0);

        pSocketMatrices.push_back(pBody->GetModelSocketMatrix("Bip001 Prop1", 0));
        pSocketMatrices.push_back(pBody->GetModelSocketMatrix("Bip001 Prop1", 1));
        pSocketMatrices.push_back(pBody->GetModelSocketMatrix("Bip001 Prop1", 2));
        pSocketMatrices.push_back(pBody->GetModelSocketMatrix("Bip001 Prop1", 3));

        PlayerWeaponDesc.pOwner = this;
        PlayerWeaponDesc.pState = &m_iState;
        PlayerWeaponDesc.strObjectTag = "PLAYER_WEAPON";
        PlayerWeaponDesc.pSocketMatrices = &pSocketMatrices;
        PlayerWeaponDesc.iLevelIndex = m_iLevelIndex;

        
        m_PartObjects[PART_WEAPON] = static_cast<CPartObject*>(m_pGameInst->ClonePrototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_GLOBAL, TEXT("Prototype_GameObject_PlayerWeapon"), &PlayerWeaponDesc));
        if (nullptr == m_PartObjects[PART_WEAPON])
            return E_FAIL;
    }

    return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayer* pInstance = new CPlayer(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CPlayer");
        SafeRelease(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
    CPlayer* pInstance = new CPlayer(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CPlayer");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CPlayer::Free()
{
    __super::Free();
}
