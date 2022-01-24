//
// Created by FDC on 2022/1/6.
//

#include <vulkanDevice.h>
#include <vulkanQueue.h>
#include <vulkanInstance.h>
#include <vulkanSurface.h>
#include <string>
#include <debugUtils.h>

namespace Homura
{
    VulkanDevice::VulkanDevice(VulkanInstancePtr instance, VulkanSurfacePtr surface)
        : mDevice{VK_NULL_HANDLE}
        , mPhysicalDevice{VK_NULL_HANDLE}
        , mGfxQueue{nullptr}
        , mPresentQueue{nullptr}
        , mInstance{instance}
        , mSurface{surface}
        , mMsaaSamples{VK_SAMPLE_COUNT_1_BIT}
    {

    }

    VulkanDevice::~VulkanDevice()
    {
        destroy();
    }

    void VulkanDevice::create()
    {
        pickPhysicalDevice();
        createLogicalDevice();
    }

    void VulkanDevice::destroy()
    {
        if (mDevice != VK_NULL_HANDLE)
        {
            vkDestroyDevice(mDevice, nullptr);
            mDevice = VK_NULL_HANDLE;
        }
    }

    void VulkanDevice::pickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        VERIFYVULKANRESULT(vkEnumeratePhysicalDevices(mInstance->getHandle(), &deviceCount, nullptr));
        if (deviceCount != 0)
        {
            std::vector<VkPhysicalDevice> devices(deviceCount);
            VERIFYVULKANRESULT(vkEnumeratePhysicalDevices(mInstance->getHandle(), &deviceCount, devices.data()));

            for (const auto& device : devices)
            {
                if (isDeviceSuitable(device))
                {
                    mPhysicalDevice = device;
                    mMsaaSamples = getMaxUsableSampleCount();
                    break;
                }
            }
        }
        assert(mPhysicalDevice != VK_NULL_HANDLE);
    }

    void VulkanDevice::createLogicalDevice()
    {
        QueueFamilyIndices indices = findQueueFamilies(mPhysicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::vector<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.samplerAnisotropy = VK_TRUE;
        deviceFeatures.sampleRateShading = VK_TRUE;

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceRequiredExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceRequiredExtensions.data();

        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        VERIFYVULKANRESULT(vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice));

        mGfxQueue = std::make_shared<VulkanQueue>(std::make_shared<VkDevice>(mDevice), indices.graphicsFamily.value());
        mPresentQueue = std::make_shared<VulkanQueue>(std::make_shared<VkDevice>(mDevice), indices.presentFamily.value());
    }

    VkSampleCountFlagBits VulkanDevice::getMaxUsableSampleCount()
    {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(mPhysicalDevice, &physicalDeviceProperties);

        VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
        if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
        if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
        if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
        if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
        if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

        return VK_SAMPLE_COUNT_1_BIT;
    }

    QueueFamilyIndices VulkanDevice::findQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            if (queueFamily.queueFlags == VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            VERIFYVULKANRESULT(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface->getHandle(), &presentSupport));

            if (presentSupport)
            {
                indices.presentFamily = i;
            }

            if (indices.isComplete())
            {
                break;
            }
            i++;
        }
        return indices;
    }
}