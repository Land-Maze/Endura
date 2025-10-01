#include "Shader.h"

namespace Renderer
{
	Shader::Shader(const vk::raii::Device& device,  vk::ShaderStageFlagBits stage, const std::vector<uint32_t>& spirV)
	{
		this->stage = stage;

		const vk::ShaderModuleCreateInfo createInfo({},
		                                            spirV.size() * sizeof(uint32_t),
		                                            spirV.data()
		);

		module = vk::raii::ShaderModule(device, createInfo);
	}

	Shader::operator const vk::raii::ShaderModule&() const
	{
		return module;
	}

	vk::ShaderStageFlagBits Shader::getStage() const
	{
		return stage;
	}
}
