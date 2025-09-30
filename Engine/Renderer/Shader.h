#pragma once
#include <vulkan/vulkan_raii.hpp>

namespace Renderer
{
	class Shader
	{
		Shader(const vk::raii::Device& device, const std::vector<uint32_t>& spirV, vk::ShaderStageFlagBits stage);

		vk::ShaderStageFlagBits getStage() const;

		explicit operator const vk::raii::ShaderModule&() const;

	private:
		vk::ShaderStageFlagBits stage;
		vk::raii::ShaderModule module = VK_NULL_HANDLE;
	};
}
