#pragma once
#include <vulkan/vulkan_raii.hpp>

namespace Renderer
{
	class Shader
	{
	public:
		Shader(const vk::raii::Device& device, vk::ShaderStageFlagBits stage, const std::vector<uint32_t>& spirV);

		vk::ShaderStageFlagBits getStage() const;

		explicit operator const vk::raii::ShaderModule&() const;

	private:
		vk::ShaderStageFlagBits stage;
		vk::raii::ShaderModule module = VK_NULL_HANDLE;
	};
}
