#include "CollisionManager.h"
#include "base/Logger.h"


CollisionManager* CollisionManager::instance = nullptr;
std::once_flag CollisionManager::initInstanceFlag;

CollisionManager* CollisionManager::GetInstance()
{
    std::call_once(initInstanceFlag, []() {
        instance = new CollisionManager();
        });
    return instance;
}

void CollisionManager::Initialize()
{
}

void CollisionManager::Finalize()
{
    delete instance;
    instance = nullptr;
}

void CollisionManager::Update()
{
    for (auto& collider : colliders_) {
        if (collider) {
            collider->Update();
        }
    }
    // コリジョンが当たっているかを判定
    CheckAllCollisions();
}

void CollisionManager::Draw()
{
    for (auto& collider : colliders_) {
        if (collider) {
            collider->DrawDebug();
        }
    }
}

void CollisionManager::AddCollider(std::unique_ptr<BaseCollider> collider)
{
    colliders_.push_back(std::move(collider));
}

BaseCollider* CollisionManager::FindCollider(std::string colliderName)
{
    for (auto& collider : colliders_) {
        if (collider->name_ == colliderName) {
            return collider.get();
        }
    }
    Logger::Log("colliderが見つかりませんでした");
    return nullptr;
}

void CollisionManager::CheckAllCollisions()
{
    currentCollisions_.clear();

    size_t colliderCount = colliders_.size();
    for (size_t i = 0; i < colliderCount; ++i) {
        BaseCollider* a = colliders_[i].get();
        if (!a) continue;

        for (size_t j = i + 1; j < colliderCount; ++j) {
            BaseCollider* b = colliders_[j].get();
            if (!b) continue;

            if (CheckCollision(a, b)) {
                ColliderPair pair = std::minmax(a, b);

                currentCollisions_.insert(pair);

                if (previousCollisions_.count(pair)) {
                    // すでに衝突していた
                    if (a->owner_) a->owner_->OnCollisionStay(b);
                    if (b->owner_) b->owner_->OnCollisionStay(a);
                } else {
                    // 今回初めて衝突
                    if (a->owner_) a->owner_->OnCollisionEnter(b);
                    if (b->owner_) b->owner_->OnCollisionEnter(a);
                }
            }
        }
    }

    // 衝突が終了したペアを検出
    for (const auto& pair : previousCollisions_) {
        if (!currentCollisions_.count(pair)) {
            if (pair.first->owner_) pair.first->owner_->OnCollisionExit(pair.second);
            if (pair.second->owner_) pair.second->owner_->OnCollisionExit(pair.first);
        }
    }

    // 今回の衝突情報を次回用に保存
    previousCollisions_ = currentCollisions_;
}

bool CollisionManager::CheckCollision(BaseCollider* a, BaseCollider* b)
{
    auto typeA = a->GetShapeType();
    auto typeB = b->GetShapeType();

    if (typeA == CollisionShapeType::Sphere && typeB == CollisionShapeType::Sphere) {
        return CheckSphereToSphereCollision(static_cast<SphereCollider*>(a), static_cast<SphereCollider*>(b));
    }
    if (typeA == CollisionShapeType::AABB && typeB == CollisionShapeType::AABB) {
        return CheckAABBToAABBCollision(static_cast<AABBCollider*>(a), static_cast<AABBCollider*>(b));
    }
    return false;
}

bool CollisionManager::CheckAABBToAABBCollision(AABBCollider* a, AABBCollider* b)
{
    // コライダーがどちらもactiveになってるか確認
    if (!a->GetColliderData().isActive || !b->GetColliderData().isActive) return false;

    Vector3 MaxPosA = a->GetMax();
    Vector3 MaxPosB = b->GetMax();
    Vector3 MinPosA = a->GetMin();
    Vector3 MinPosB = b->GetMin();

    bool isTachX = MaxPosA.x > MinPosB.x && MinPosA.x < MaxPosB.x;
    bool isTachY = MaxPosA.y > MinPosB.y && MinPosA.y < MaxPosB.y;
    bool isTachZ =  MaxPosA.z >MinPosB.z && MinPosA.z < MaxPosB.z;

    return isTachX && isTachY && isTachZ;
}

bool CollisionManager::CheckSphereToSphereCollision(SphereCollider* a, SphereCollider* b)
{
    // コライダーがどちらもactiveになってるか確認
    if (!a->GetColliderData().isActive || !b->GetColliderData().isActive) return false;

    float distSq = Length(a->GetCenter() - b->GetCenter());
    float radiusSum = a->GetRadius() + b->GetRadius();
    return distSq <= (radiusSum * radiusSum);
}
