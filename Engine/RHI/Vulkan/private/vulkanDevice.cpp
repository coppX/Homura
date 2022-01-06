//
// Created by FDC on 2022/1/6.
//

#include "vulkanDevice.h"
#include <string>

namespace Homura
{
    VulkanDevice::VulkanDevice(VkPhysicalDevice physicalDevice)
    {

    }

    VulkanDevice::~VulkanDevice()
    {

    }

    bool VulkanDevice::queryDevice(uint32_t index)
    {
        bool discrete = false;
        vkGetPhysicalDeviceProperties(mPhysicalDevice, &mPhysicalDeviceProperties);

        auto GetDeviceTypeString = [&]()->std::string
        {
            std::string info;
            switch (mPhysicalDeviceProperties.deviceType)
            {
                case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                    info = "Other";
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    info = "Discrete GPU";
                    discrete = true;
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    info = "Integrated GPU";
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_CPU:
                    info = "CPU";
                    break;
                default:
                    info = "Unknown";
                    break;
            }
            return info;
        };
        printf("DeviceID 0x%x Type %s", mPhysicalDeviceProperties.deviceID, GetDeviceTypeString().c_str());

        uint32_t queueCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueCount, nullptr);
        mQueueFamilyProperties.resize(queueCount);
        vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueCount, mQueueFamilyProperties.data());

        return discrete;
    }

    void VulkanDevice::initGPU(uint32_t index)
    {

    }
}