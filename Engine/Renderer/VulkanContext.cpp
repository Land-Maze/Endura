#include "VulkanContext.h"

#include <iostream>
#include <ostream>
#include <GLFW/glfw3.h>

namespace Renderer
{
    void VulkanContext::InitializeVulkan()
    {
        createInstance();
        setupDebugMessenger();
    }

    void VulkanContext::createInstance()
    {
        constexpr vk::ApplicationInfo application_info(vk::StructureType::eApplicationInfo, nullptr, "Endura",
                                                       VK_MAKE_VERSION(0, 0, 1), "No Engine",
                                                       VK_MAKE_API_VERSION(0, 0, 0, 1), vk::ApiVersion13);

        std::vector<char const*> requiredLayers;
        if (enableValidationLayers)
        {
            requiredLayers.assign(validationLayers.begin(), validationLayers.end());
        }

        const auto layerProperties = _context.enumerateInstanceLayerProperties();
        bool areLayersSupported = true;
        for (const auto layer : requiredLayers)
        {
            bool isLayerSupported = false;
            for (auto supportedLayer : layerProperties)
            {
                if (strcmp(supportedLayer.layerName, layer) == 0)
                {
                    isLayerSupported = true;
                    break;
                }
            }
            if (!isLayerSupported)
            {
                areLayersSupported = false;
                break;
            }
        }
        if (!areLayersSupported) throw std::runtime_error("One or more required layers are not supported!");

        const auto extension = getGLFWRequiredExtension();

        const vk::InstanceCreateInfo create_info(vk::StructureType::eInstanceCreateInfo, nullptr, {}, &application_info,
                                                 requiredLayers.size(), requiredLayers.data(), extension.size(),
                                                 extension.data());

        _instance = vk::raii::Instance(_context, create_info);
    }

    std::vector<const char*> VulkanContext::getGLFWRequiredExtension() const
    {
        u_int32_t glfwExtensionCount = 0;
        auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        auto extensionProperties = _context.enumerateInstanceExtensionProperties();
        bool areExtensionsSupported = true;
        for (uint32_t i = 0; i < glfwExtensionCount; i++)
        {
            bool isExtensionSupported = false;
            for (auto supportedExtension : extensionProperties)
            {
                if (strcmp(supportedExtension.extensionName, glfwExtensions[i]) == 0)
                {
                    isExtensionSupported = true;
                    break;
                }
            }
            if (!isExtensionSupported)
            {
                areExtensionsSupported = false;
                break;
            }
        }
        if (!areExtensionsSupported) throw std::runtime_error("One or more required extensions are not supported!");

        std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) extensions.push_back(vk::EXTDebugUtilsExtensionName);

        return extensions;
    }

    void VulkanContext::setupDebugMessenger()
    {
        if constexpr (!enableValidationLayers) return;

        vk::DebugUtilsMessageSeverityFlagsEXT severity_flags(
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning);
        vk::DebugUtilsMessageTypeFlagsEXT message_type_flags(
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
        vk::DebugUtilsMessengerCreateInfoEXT debug_utils_messenger_create_info(
            vk::StructureType::eDebugUtilsMessengerCreateInfoEXT, nullptr, {}, severity_flags, message_type_flags,
            &debugCallback,
            nullptr);

        debugMessenger = _instance.createDebugUtilsMessengerEXT(debug_utils_messenger_create_info);
    }

    vk::Bool32 VulkanContext::debugCallback(const vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                                            const vk::DebugUtilsMessageTypeFlagsEXT type,
                                            const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*)
    {
        // FIXME: Implement Logger and put this there
        std::cerr << "validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage <<
            std::endl;

        return vk::False;
    }
}
