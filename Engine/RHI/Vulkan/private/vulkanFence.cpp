//
// Created by FDC on 2022/1/14.
//

#include <vulkanFence.h>
#include <vulkanDevice.h>
#include <debugUtils.h>

namespace Homura
{
    VulkanFence::VulkanFence(VulkanDevicePtr device, bool signaled)
        : mDevice{device}
    {
        VkFenceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        createInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

        VERIFYVULKANRESULT(vkCreateFence(mDevice->getHandle(), &createInfo, nullptr, &mFence));
    }

    VulkanFence::~VulkanFence()
    {
        if (mFence != VK_NULL_HANDLE)
        {
            vkDestroyFence(mDevice->getHandle(), mFence, nullptr);
        }
    }

    void VulkanFence::reset()
    {
        if (mFence != VK_NULL_HANDLE)
        {
            vkResetFences(mDevice->getHandle(), 1, &mFence);
        }
    }

    VkResult VulkanFence::getResult()
    {
        return vkGetFenceStatus(mDevice->getHandle(), mFence);
    }
}