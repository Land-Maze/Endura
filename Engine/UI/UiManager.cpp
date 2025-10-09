#include "UiManager.h"

namespace UI
{
	void UIManager::setWindow(const std::shared_ptr<Core::Window>& window)
	{
		this->m_window = window;
	}

	void UIManager::initImGUI(const Renderer::VulkanContext& vkContext)
	{
		return;
	}
}
