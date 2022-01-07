//
// Created by 最上川 on 2022/1/7.
//
#include "vulkanQueue.h"
#include "vulkanDevice.h"

namespace Homura
{
    VulkanQueue::VulkanQueue(VulkanDevice* device, uint32_t familyIndex)
        : mQueue{VK_NULL_HANDLE}, mFamilyIndex{familyIndex}, mDevice{device}
    {
        vkGetDeviceQueue(mDevice->getHandle(), mFamilyIndex, 0, &mQueue);
    }
}
