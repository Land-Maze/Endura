#pragma once
#include <vulkan/vulkan_raii.hpp>

namespace Renderer
{
	class Shader
	{
	public:
		Shader(const vk::raii::Device& device, vk::ShaderStageFlagBits stage, const char* entryPoint, const std::vector<uint32_t>& spirV);

		vk::ShaderStageFlagBits getStage() const;

		vk::ShaderModule getModule() const;

		vk::PipelineShaderStageCreateInfo getStageInfo() const;

	private:
		vk::ShaderStageFlagBits stage;
		vk::PipelineShaderStageCreateInfo stageInfo;
		vk::raii::ShaderModule module = VK_NULL_HANDLE;
	};
}
