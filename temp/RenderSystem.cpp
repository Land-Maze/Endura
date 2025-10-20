#include "../Engine/Graphics/RenderSystem.h"

namespace Renderer
{
	void RenderSystem::submit(Renderable renderable)
	{
		m_renderables.emplace_back(renderable);
	}

	void RenderSystem::render(VulkanContext& context)
	{
	}

	void RenderSystem::clear()
	{
		m_renderables.clear();
	}
}
