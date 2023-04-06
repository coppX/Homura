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
        , mPresent{nullptr}
        , mInstance{instance}
        , mSurface{surface}
        , mMsaaSamples{VK_SAMPLE_COUNT_1_BIT}
    {
        create();
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

    void VulkanDevice::idle()
    {
        vkDeviceWaitIdle(mDevice);
    }

    void VulkanDevice::pickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(mInstance->getHandle(), &deviceCount, nullptr);

        if (deviceCount == 0)
        {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(mInstance->getHandle(), &deviceCount, devices.data());

        for (const auto &device : devices)
        {
            if (isDeviceSuitable(device))
            {
                mPhysicalDevice = device;
                mMsaaSamples = getMaxUsableSampleCount();
                break;
            }
        }

        if (mPhysicalDevice == VK_NULL_HANDLE)
        {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    void VulkanDevice::createLogicalDevice()
    {
        QueueFamilyIndices indices = findQueueFamilies(mPhysicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::vector<uint32_t> queueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : queueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType               = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex    = queueFamily;
            queueCreateInfo.queueCount          = 1;
            queueCreateInfo.pQueuePriorities    = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.samplerAnisotropy        = VK_TRUE;
        deviceFeatures.sampleRateShading        = VK_TRUE;
        deviceFeatures.fragmentStoresAndAtomics = VK_TRUE;
        deviceFeatures.independentBlend         = VK_TRUE;
        deviceFeatures.geometryShader           = VK_TRUE;

        VkDeviceCreateInfo createInfo{};
        createInfo.sType                    = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount     = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos        = queueCreateInfos.data();
        createInfo.pEnabledFeatures         = &deviceFeatures;
        createInfo.enabledExtensionCount    = static_cast<uint32_t>(deviceRequiredExtensions.size());
        createInfo.ppEnabledExtensionNames  = deviceRequiredExtensions.data();

        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount    = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames  = validationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        VERIFYVULKANRESULT(vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice));
    }

    bool VulkanDevice::isDeviceSuitable(VkPhysicalDevice device)
    {
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        return deviceFeatures.samplerAnisotropy;
    }

    VkSampleCountFlagBits VulkanDevice::getMaxUsableSampleCount()
    {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(mPhysicalDevice, &physicalDeviceProperties);

        VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT)    { return VK_SAMPLE_COUNT_64_BIT; }
        if (counts & VK_SAMPLE_COUNT_32_BIT)    { return VK_SAMPLE_COUNT_32_BIT; }
        if (counts & VK_SAMPLE_COUNT_16_BIT)    { return VK_SAMPLE_COUNT_16_BIT; }
        if (counts & VK_SAMPLE_COUNT_8_BIT)     { return VK_SAMPLE_COUNT_8_BIT; }
        if (counts & VK_SAMPLE_COUNT_4_BIT)     { return VK_SAMPLE_COUNT_4_BIT; }
        if (counts & VK_SAMPLE_COUNT_2_BIT)     { return VK_SAMPLE_COUNT_2_BIT; }

        return VK_SAMPLE_COUNT_1_BIT;
    }

    QueueFamilyIndices VulkanDevice::findQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndices  indices;
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto &queueFamily : queueFamilies)
        {
            if (!indices.graphicsFamily.has_value() && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
                i++;
                continue;
            }

            VkBool32 isPresentSupport = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface->getHandle(), &isPresentSupport);
            if (isPresentSupport)
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

    void VulkanDevice::initializeQueue()
    {
        QueueFamilyIndices indices = findQueueFamilies(mPhysicalDevice);
        mGfxQueue   = std::make_shared<VulkanQueue>(shared_from_this(), indices.graphicsFamily.value());
        mPresent    = std::make_shared<VulkanQueue>(shared_from_this(), indices.presentFamily.value());
    }
}