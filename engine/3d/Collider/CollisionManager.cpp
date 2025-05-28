#include "CollisionManager.h"
#include <Logger.h>

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
        if (collider->name == colliderName) {
            return collider.get();
        }
    }
    Logger::Log("renderが見つかりませんでした");
    return nullptr;
}
