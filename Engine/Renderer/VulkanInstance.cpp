#include "VulkanInstance.h"

#include <iostream>
#include <GLFW/glfw3.h>

namespace Renderer
{
	void VulkanInstance::initialize(const char* appName, const uint32_t appVersion)
	{
		createInstance(appName, appVersion);
		setupDebugMessenger();
	}

	void VulkanInstance::createInstance(const char* appName, const uint32_t appVersion)
	{
		const vk::ApplicationInfo applicationInfo(
			appName,
			appVersion,
			{},
			{},
			VULKAN_API_VERSION
		);

		std::vector<const char*> requiredLayers;
		if(ENABLE_VALIDATION_LAYERS)
		{
			requiredLayers.assign(m_validationLayers.begin(), m_validationLayers.end());
			checkLayers(requiredLayers);
		}

		const auto requiredExtensions = getGLFWRequiredExtensions();

		// This is for future OSX support
		constexpr vk::InstanceCreateFlags instanceCreateFlags;

		const vk::InstanceCreateInfo createInfo(
			instanceCreateFlags,
			&applicationInfo,
			requiredLayers.size(),
			requiredLayers.data(),
			requiredExtensions.size(),
			requiredExtensions.data()
		);

		m_instance = vk::raii::Instance(m_context, createInfo);
	}


	void VulkanInstance::checkLayers(const std::vector<char const*>& requiredLayers) const
	{
		const auto layerProperties = m_context.enumerateInstanceLayerProperties();

		bool areLayersSupported = true;
		for(const auto layer : requiredLayers)
		{
			bool isLayerSupported = false;
			for(auto supportedLayer : layerProperties)
			{
				if(strcmp(supportedLayer.layerName, layer) == 0)
				{
					isLayerSupported = true;
					break;
				}
			}
			if(!isLayerSupported)
			{
				areLayersSupported = false;
				break;
			}
		}

		if(!areLayersSupported)
		{
			throw std::runtime_error(
				"One or more required layers are not supported: areLayersSupported is false."
			);
		}
	}


	std::vector<const char*> VulkanInstance::getGLFWRequiredExtensions() const
	{
		u_int32_t glfwExtensionCount = 0;
		const auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		const auto extensionProperties = m_context.enumerateInstanceExtensionProperties();
		bool areExtensionsSupported = true;
		for(uint32_t i = 0; i < glfwExtensionCount; i++)
		{
			bool isExtensionSupported = false;
			for(auto supportedExtension : extensionProperties)
			{
				if(strcmp(supportedExtension.extensionName, glfwExtensions[i]) == 0)
				{
					isExtensionSupported = true;
					break;
				}
			}
			if(!isExtensionSupported)
			{
				areExtensionsSupported = false;
				break;
			}
		}
		if(!areExtensionsSupported)
		{
			throw std::runtime_error(
				"One or more required extensions are not supported: areExtensionsSupported is false."
			);
		}

		std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if(ENABLE_VALIDATION_LAYERS) extensions.push_back(vk::EXTDebugUtilsExtensionName);

		return extensions;
	}

	void VulkanInstance::setupDebugMessenger()
	{
		if constexpr(!ENABLE_VALIDATION_LAYERS) return;

		constexpr vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
		);
		constexpr vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
			vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
		);
		constexpr vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo(
			{},
			severityFlags,
			messageTypeFlags,
			&debugCallback
		);

		m_debug_messenger = m_instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfo);
	}

	vk::Bool32 VulkanInstance::debugCallback(
		const vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
		const vk::DebugUtilsMessageTypeFlagsEXT type,
		const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void* data
	)
	{
		// FIXME: Implement Logger and put this there
		std::cerr <<
			"[validation layer] type: "
			<< to_string(type)
			<< " | message: {"
			<< pCallbackData->pMessage
			<< ""
			<< std::endl;


		return vk::False;
	}
}
