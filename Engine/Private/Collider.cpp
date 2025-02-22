#include "Collider.h"

#include "GameInstance.h"
#include "BoundingAABB.h"
#include "GameObject.h"


CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :
    CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider& Prototype)
    :
    CComponent(Prototype),
    m_eType(Prototype.m_eType),
    m_pBatch(Prototype.m_pBatch),
    m_pEffect(Prototype.m_pEffect),
    m_pInputLayout(Prototype.m_pInputLayout)
{
    SafeAddRef(m_pInputLayout);
}

HRESULT CCollider::InitializePrototype(COLLIDER_TYPE eType)
{
    m_eType = eType;

#ifdef _DEBUG

    // 박스, 구를 그려내기 위한 객체들을 선언해 놓는다.
    m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);     // VB, IB를 대체하는 객체
    m_pEffect = new BasicEffect(m_pDevice);                             // Shader를 대체하는 객체

    const void* pShaderByteCode = nullptr;
    size_t      iShaderBytecodeLength = 0;

    // !텍스쳐가 우선시 되기 때문에 Color를 우선 하겠다고 GetVertexShaderBytecode를 호출하기 이전에 호출 해야한다.
    m_pEffect->SetVertexColorEnabled(true);
    m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderBytecodeLength);

    // 렌더링 파이프 라인을 거쳐서 충돌체도 렌더가 되어야 하기때문에 IA를 만들어준다.
    if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderBytecodeLength, &m_pInputLayout)))
        return E_FAIL;

#endif

    return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
    if (nullptr != pArg)
    {
        CBounding::BOUND_DESC* pDesc = static_cast<CBounding::BOUND_DESC*>(pArg);
        
        switch (m_eType)
        {
        case Engine::CCollider::TYPE_SPHERE:
            m_pBounding = CBoundingSphere::Create(m_pDevice, m_pContext, pDesc);
            break;
        case Engine::CCollider::TYPE_AABB:
            m_pBounding = CBoundingAABB::Create(m_pDevice, m_pContext, pDesc);
            break;
        case Engine::CCollider::TYPE_OBB:
            m_pBounding = CBoundingOBB::Create(m_pDevice, m_pContext, pDesc);
            break;
        case Engine::CCollider::TYPE_END:
            break;
        default:
            break;
        }
    }

    return S_OK;
}

void CCollider::Update(_fmatrix WorldMatrix)
{
    if (nullptr == m_pBounding)
        return;

    m_pBounding->Update(WorldMatrix);
}

#ifdef _DEBUG

HRESULT CCollider::Render()
{
    // 241115 기준으로 POINT INSTANCE에서 GS를 사용중이기 때문에 내가 직접만든 쉐이더를 통해서 Font는 렌더하지 않아 GS를 쓰지 않겠다고 명시함
    m_pContext->GSSetShader(nullptr, nullptr, 0);

    m_pEffect->SetWorld(XMMatrixIdentity());
    m_pEffect->SetView(m_pGameInst->GetTransformMatrix(CPipeLine::D3DTS_VIEW));
    m_pEffect->SetProjection(m_pGameInst->GetTransformMatrix(CPipeLine::D3DTS_PROJ));


    m_pEffect->Apply(m_pContext);
    m_pContext->IASetInputLayout(m_pInputLayout);

    m_pBatch->Begin();

    m_pBounding->Render(m_pBatch);

    m_pBatch->End();

    return S_OK;
}

HRESULT CCollider::Render(_fvector vColor)
{
    m_pContext->GSSetShader(nullptr, nullptr, 0);

    m_pEffect->SetWorld(XMMatrixIdentity());
    m_pEffect->SetView(m_pGameInst->GetTransformMatrix(CPipeLine::D3DTS_VIEW));
    m_pEffect->SetProjection(m_pGameInst->GetTransformMatrix(CPipeLine::D3DTS_PROJ));
    m_pEffect->SetEmissiveColor(vColor);

    m_pEffect->Apply(m_pContext);
    m_pContext->IASetInputLayout(m_pInputLayout);

    m_pBatch->Begin();

    m_pBounding->Render(m_pBatch);

    m_pBatch->End();

    // m_pEffect->SetEmissiveColor(vColor);

    return S_OK;
}

#endif

_bool CCollider::Intersect(const CCollider* pOtherColiider)
{
    return m_pBounding->Intersect(pOtherColiider->m_eType, pOtherColiider->m_pBounding);
}

// Bullet이 해당함수 호출시 pSrc는 상대방(Wall)이 된다. this = CBullet
_bool CCollider::Intersect(const CCollider* pSrc, HIT_RESULT& HitResult)
{
    if (m_pBounding->Intersect(pSrc->m_eType, pSrc->m_pBounding, HitResult))
    {
        if (this->m_pOwner && pSrc->m_pOwner)
        {
            string strSrcObjTag = this->m_pOwner->GetObjectTag();
            string strOtherObjTag = pSrc->m_pOwner->GetObjectTag();

            if (strSrcObjTag == "WallAABB")
            {
                // TODO..??
                HitResult.pOther = (void*)(this->m_pOwner);

                if (strOtherObjTag == "PlayerBullet")
                    const_cast<CCollider*>(pSrc)->SetEndCollision();
                return true;
            }
            else if (strSrcObjTag == "Monster")
            {
                HitResult.pOther = (void*)(this->m_pOwner);

                if (strOtherObjTag == "PlayerBullet")
                    const_cast<CCollider*>(pSrc)->SetEndCollision();
                return true;
            }
            else if (strSrcObjTag == "Player")
            {
                HitResult.pOther = (void*)(this->m_pOwner);

                if (strOtherObjTag == "MonsterBullet")
                    const_cast<CCollider*>(pSrc)->SetEndCollision();
                return true;
            }
            else
                return false;
        }
        else
        {
            if (!IsEndCollision())
            {
                HitResult.pOther = (void*)(pSrc->m_pOwner);
                SetEndCollision();
            }
            return true;
        }
    }
    return false;
}

_bool CCollider::IntersectOnlyProjectile(CCollider* pProjectileColiider, HIT_RESULT& HitResult)
{
    if (m_pBounding->Intersect(pProjectileColiider->m_eType, pProjectileColiider->m_pBounding, HitResult))
    {
        if (pProjectileColiider->IsEndCollision())
            return false;

        HitResult.pOther = this->m_pOwner;
        pProjectileColiider->SetEndCollision();
        return true;
    }
    return false;
}

_bool CCollider::Intersect(_fvector vRayOrigin, _fvector vRayDir, _float& fDist, _float4& vPickingPos)
{
    return m_pBounding->Intersect(vRayOrigin, vRayDir, fDist, vPickingPos);
}

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER_TYPE eType)
{
    CCollider* pInstance = new CCollider(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype(eType)))
    {
        MSG_BOX("Failed to Created : CCollider");
        SafeRelease(pInstance);
    }
    return pInstance;
}

CComponent* CCollider::Clone(void* pArg)
{
    CCollider* pInstance = new CCollider(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CCollider");
        SafeRelease(pInstance);
    }

    return pInstance;
}

void CCollider::Free()
{
    __super::Free();

    SafeRelease(m_pBounding);

    if (false == m_bIsCloned)
    {
        SafeDelete(m_pBatch);
        SafeDelete(m_pEffect);
    }

    SafeRelease(m_pInputLayout);
}
