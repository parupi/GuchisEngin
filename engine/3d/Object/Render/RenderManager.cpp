#include "RenderManager.h"

RenderManager* RenderManager::instance = nullptr;
std::once_flag RenderManager::initInstanceFlag;

RenderManager* RenderManager::GetInstance()
{
	std::call_once(initInstanceFlag, []() {
		instance = new RenderManager();
		});
	return instance;
}

void RenderManager::Initialize(DirectXManager* dxManager)
{
	dxManager_ = dxManager;
}

void RenderManager::AddRender(std::unique_ptr<BaseRender> render)
{
	renders_.push_back(std::move(render));
}

BaseRender* RenderManager::FindRender(std::string renderName)
{
	for (auto& render : renders_) {
		if (render->name == renderName) {
			return render.get();
		}
	}
	Logger::Log("renderが見つかりませんでした");
	return nullptr;
}

