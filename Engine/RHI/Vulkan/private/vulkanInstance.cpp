//
// Created by 最上川 on 2022/1/22.
//
#include <vulkanInstance.h>
#include <debugUtils.h>

namespace Homura
{
    VulkanInstance::VulkanInstance()
    {

    }

    VulkanInstance::~VulkanInstance()
    {

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
}