#include "VulkanContext.h"

#include <iostream>
#include <ostream>

namespace Renderer
{
    void VulkanContext::InitializeVulkan(GLFWwindow* window)
    {
        createInstance();
        setupDebugMessenger();

        pickPhysicalDevice();

        createSurface(window);
        findBestQueueFamilyIndexes();

        createLogicalDevice();
        createQueues();
    }

    void VulkanContext::createInstance()
    {
        constexpr vk::ApplicationInfo application_info(
            "Endura",
            VK_MAKE_VERSION(0, 0, 1), "No Engine",
            VK_MAKE_API_VERSION(0, 0, 0, 1), vk::ApiVersion13
        );

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
        if (!areLayersSupported)
            throw std::runtime_error(
                "One or more required layers are not supported: areLayersSupported is false.");

        const auto extension = getGLFWRequiredExtension();

        const vk::InstanceCreateInfo create_info(
            {
            },
            &application_info,
            requiredLayers.size(),
            requiredLayers.data(),
            extension.size()
            , extension.data(),
            nullptr
        );

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
        if (!areExtensionsSupported)
            throw std::runtime_error(
                "One or more required extensions are not supported: areExtensionsSupported is false.");

        std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) extensions.push_back(vk::EXTDebugUtilsExtensionName);

        return extensions;
    }

    void VulkanContext::setupDebugMessenger()
    {
        if constexpr (!enableValidationLayers) return;

        constexpr vk::DebugUtilsMessageSeverityFlagsEXT severity_flags(
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning);
        constexpr vk::DebugUtilsMessageTypeFlagsEXT message_type_flags(
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
        constexpr vk::DebugUtilsMessengerCreateInfoEXT debug_utils_messenger_create_info(
            {},
            severity_flags,
            message_type_flags,
            &debugCallback
        );

        debug_messenger = _instance.createDebugUtilsMessengerEXT(debug_utils_messenger_create_info);
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

    void VulkanContext::pickPhysicalDevice()
    {
        const auto devices = _instance.enumeratePhysicalDevices();

        if (devices.empty())
            throw std::runtime_error(
                "No suitable device with Vulkan support was found: enumeratePhysicalDevices() returned empty vector. Is the GPU enabled?");

        uint64_t deviceScore = 0;

        for (const auto device : devices)
        {
            const auto deviceProperties = device.getProperties();
            const auto deviceFeatures = device.getFeatures();
            uint64_t score = 0;

            if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
            {
                score += 1000;
            }

            score += deviceProperties.limits.maxImageDimension2D;


            if (score > deviceScore)
            {
                _device_features = deviceFeatures;
                _physical_device = std::move(device);
                deviceScore = score;
            }
        }

        if (_physical_device == VK_NULL_HANDLE)
            throw std::runtime_error(
                "No suitable device with required features was found. Is the GPU enabled?");

        std::printf("Device -> Name: %s, API v%u\n", _physical_device.getProperties().deviceName.data(),
                    _physical_device.getProperties().apiVersion);
    }

    void VulkanContext::findBestQueueFamilyIndexes()
    {
        const std::vector<vk::QueueFamilyProperties> queue_families = _physical_device.getQueueFamilyProperties();

        for (uint32_t i = 0; i < queue_families.size(); i++)
        {
            if (const auto family = queue_families[i]; family.queueFlags & vk::QueueFlagBits::eGraphics &&
                _physical_device.getSurfaceSupportKHR(i, _surface))
            {
                _graphics_family_index = i;
                _present_family_index = i;
                break;
            }
        }

        if (_graphics_family_index == UINT32_MAX)
            for (uint32_t i = 0; i < queue_families.size(); i++)
            {
                const auto family = queue_families[i];
                if ((family.queueFlags & vk::QueueFlagBits::eGraphics) && (_graphics_family_index != UINT32_MAX))
                {
                    _graphics_family_index = i;
                }

                if (_physical_device.getSurfaceSupportKHR(
                    static_cast<uint32_t>(family.queueFlags & vk::QueueFlagBits::eGraphics), _surface))
                {
                    _present_family_index = _graphics_family_index;
                    break;
                }
            }

        if ((_graphics_family_index == UINT32_MAX) || (_present_family_index == UINT32_MAX))
            throw std::runtime_error(
                "Could not find a queue for graphics or present: neither of _graphics_family_index nor _present_family_index is set.");
    }

    void VulkanContext::createSurface(GLFWwindow* window)
    {
        VkSurfaceKHR surface;

        if (glfwCreateWindowSurface(*_instance, window, nullptr, &surface) != VK_SUCCESS)
            throw std::runtime_error(
                "Failed to create window surface: glfwCreateWindowSurface returned non-zero value.");

        _surface = vk::raii::SurfaceKHR(_instance, surface);
    }

    void VulkanContext::createQueues()
    {
        _graphics_queue = vk::raii::Queue(_device, _graphics_family_index, 0);
        _present_queue = vk::raii::Queue(_device, _present_family_index, 0);
    }

    void VulkanContext::createLogicalDevice()
    {
        auto features = _physical_device.getFeatures2();

        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT extendedDynamicStateFeatures;
        extendedDynamicStateFeatures.extendedDynamicState = vk::True;

        vk::PhysicalDeviceVulkan11Features vulkan11Features;
        vulkan11Features.setShaderDrawParameters(vk::True);

        vk::PhysicalDeviceVulkan12Features vulkan12Features;

        vk::PhysicalDeviceVulkan13Features vulkan13Features;
        vulkan13Features.setDynamicRendering(vk::True);
        vulkan13Features.setSynchronization2(vk::True);

        vulkan11Features.setPNext(extendedDynamicStateFeatures);
        vulkan12Features.setPNext(vulkan11Features);
        vulkan13Features.setPNext(vulkan12Features);
        features.setPNext(vulkan13Features);

        constexpr float queuePriority = 0.0f;

        vk::DeviceQueueCreateInfo deviceQueueCreateInfo(
            {},
            _graphics_family_index, 1, &queuePriority
        );

        vk::DeviceCreateInfo deviceCreateInfo(
            {}, 1,
            &deviceQueueCreateInfo, static_cast<uint32_t>(validationLayers.size()),
            validationLayers.data(), static_cast<uint32_t>(deviceExtensions.size()),
            deviceExtensions.data()
        );

        _device = vk::raii::Device(_physical_device, deviceCreateInfo);
    }
}
