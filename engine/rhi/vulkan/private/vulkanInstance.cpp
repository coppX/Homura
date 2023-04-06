//
// Created by 最上川 on 2022/1/22.
//
#include <vulkanInstance.h>
#include <debugUtils.h>

namespace Homura
{
    VulkanInstance::VulkanInstance()
        : mInstance{VK_NULL_HANDLE}
    {
        create();
    }

    VulkanInstance::~VulkanInstance()
    {
        destroy();
    }

    void VulkanInstance::create()
    {
        if (enableValidationLayers && !checkValidationLayerSupport())
        {
            throw std::runtime_error("validation layers requested, but not available!");
        }
        VkApplicationInfo appInfo{};
        appInfo.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName    = "Homura";
        appInfo.applicationVersion  = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName         = "Homura";
        appInfo.engineVersion       = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion          = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        std::vector<const char*> extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount    = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames  = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount    = static_cast<uint32_t>(mValidationLayers.size());
            createInfo.ppEnabledLayerNames  = mValidationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext                = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
        }
        else
        {
            createInfo.enabledLayerCount    = 0;
            createInfo.pNext                = nullptr;
        }

        VERIFYVULKANRESULT(vkCreateInstance(&createInfo, nullptr, &mInstance));
    }

    void VulkanInstance::destroy()
    {
        if (mInstance != VK_NULL_HANDLE)
        {
            vkDestroyInstance(mInstance, nullptr);
            mInstance = VK_NULL_HANDLE;
        }
    }

    std::vector<const char*> VulkanInstance::getRequiredExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (mEnableValidationLayer)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    bool VulkanInstance::checkValidationLayerSupport()
    {
        uint32_t  layerCount = 0;
        VERIFYVULKANRESULT(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));

        std::vector<VkLayerProperties> availableLayers(layerCount);
        VERIFYVULKANRESULT(vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()));

        for (const auto& layerName : mValidationLayers) {
            bool layerFound = false;

            for (const auto& layerProp : availableLayers) {
                if (std::strcmp(layerName, layerProp.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    void VulkanInstance::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
    {
        createInfo = {};
        createInfo.sType            = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity  = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                     | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                     | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType      = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                                     | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                                     | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback  = debugCallback;
    }
}