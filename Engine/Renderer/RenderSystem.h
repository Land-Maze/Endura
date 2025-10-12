#pragma once
#include <vector>

#include <Renderer/Renderable.h>
#include <Renderer/VulkanContext.h>

namespace Renderer
{
	class RenderSystem
	{
	public:
		RenderSystem() = default;
		~RenderSystem() = default;

		void submit(Renderable renderable);

		void render(VulkanContext& context);

		void clear();

	private:
		std::vector<Renderer::Renderable> m_renderables;
	};
}
