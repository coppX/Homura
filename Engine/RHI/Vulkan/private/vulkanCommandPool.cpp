//
// Created by 最上川 on 2022/1/24.
//
#include <vulkanCommandPool.h>
#include <vulkanDevice.h>
#include <vulkanQueue.h>
#include <debugUtils.h>

namespace Homura
{
    VulkanCommandPool::VulkanCommandPool(VulkanDevicePtr device)
        : mDevice{device}
        , mCommandPool{VK_NULL_HANDLE}
    {
        create();
    }

    VulkanCommandPool::~VulkanCommandPool()
    {
        destroy();
    }

    void VulkanCommandPool::create()
    {
        VkCommandPoolCreateInfo createInfo{};
        createInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.queueFamilyIndex = mDevice->getGraphicsQueue()->getFamilyIndex();
        VERIFYVULKANRESULT(vkCreateCommandPool(mDevice->getHandle(), &createInfo, nullptr, &mCommandPool));
    }

    void VulkanCommandPool::reset()
    {
        VERIFYVULKANRESULT(vkResetCommandPool(mDevice->getHandle(), mCommandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT));
    }

    void VulkanCommandPool::destroy()
    {
        if (mCommandPool != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(mDevice->getHandle(), mCommandPool, nullptr);
            mCommandPool = VK_NULL_HANDLE;
        }
    }
}

