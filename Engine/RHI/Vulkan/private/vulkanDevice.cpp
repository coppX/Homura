//
// Created by FDC on 2022/1/6.
//

#include "vulkanDevice.h"

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
        vkGetPhysicalDeviceProperties(mPhysicalDevice, &mPhysicalDeviceProperties);
        return false;
    }
}