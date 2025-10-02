#include "Shader.h"

namespace Renderer
{
	Shader::Shader(const vk::raii::Device& device, const vk::ShaderStageFlagBits stage, const char* entryPoint,
	               const std::vector<uint32_t>& spirV)
	{
		this->stage = stage;

		const vk::ShaderModuleCreateInfo createInfo(
			{},
			spirV.size() * sizeof(uint32_t),
			spirV.data()
		);

		module = vk::raii::ShaderModule(device, createInfo);

		stageInfo = vk::PipelineShaderStageCreateInfo(
			{},
			stage,
			module,
			entryPoint
		);
	}

	vk::ShaderModule Shader::getModule() const
	{
		return module;
	}

	vk::ShaderStageFlagBits Shader::getStage() const
	{
		return stage;
	}

	vk::PipelineShaderStageCreateInfo Shader::getStageInfo() const
	{
		return stageInfo;
	}
}
