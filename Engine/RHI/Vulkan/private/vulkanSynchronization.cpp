//
// Created by FDC on 2022/1/14.
//

#include <vulkanSynchronization.h>
#include <vulkanDevice.h>
#include <debugUtils.h>

namespace Homura
{
    VulkanFenceEntity::VulkanFenceEntity(VulkanDevicePtr device)
        : mDevice{device}
    {

    }

    VulkanFenceEntity::~VulkanFenceEntity()
    {
        destroy();
    }

    void VulkanFenceEntity::create(bool signaled)
    {
        VkFenceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        createInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

        VERIFYVULKANRESULT(vkCreateFence(mDevice->getHandle(), &createInfo, nullptr, &mFence));
    }

    void VulkanFenceEntity::destroy()
    {
        if (mFence != VK_NULL_HANDLE)
        {
            vkDestroyFence(mDevice->getHandle(), mFence, nullptr);
            mFence = VK_NULL_HANDLE;
        }
    }

    void VulkanFenceEntity::reset()
    {
        if (mFence != VK_NULL_HANDLE)
        {
            VERIFYVULKANRESULT(vkResetFences(mDevice->getHandle(), 1, &mFence));
        }
    }

    void VulkanFenceEntity::wait()
    {
        VERIFYVULKANRESULT(vkWaitForFences(mDevice->getHandle(), 1, &mFence, VK_TRUE, UINT64_MAX));
    }

    VkResult VulkanFenceEntity::getResult()
    {
        return vkGetFenceStatus(mDevice->getHandle(), mFence);
    }

    VulkanFenceEntity& VulkanFenceEntity::operator=(VulkanFenceEntity &r)
    {
        mDevice = r.mDevice;
        mFence = r.mFence;
        return *this;
    }

    VulkanFences::VulkanFences(VulkanDevicePtr device)
        : mDevice{device}
        , mFences{}
    {

    }

    VulkanFences::~VulkanFences()
    {

    }

    void VulkanFences::create(uint32_t num)
    {
        for (uint32_t i = 0; i < num; i++)
        {
            VulkanFenceEntity entity(mDevice);
            entity.create(false);
            mFences.push_back(entity);
        }
    }

    void VulkanFences::destroy()
    {
        for (auto& entity : mFences)
        {
            entity.destroy();
        }
        mFences.clear();
    }

    void VulkanFences::wait(uint32_t index)
    {
        assert(index < mFences.size());
        VulkanFenceEntity entity = mFences[index];
        entity.wait();
    }

    void VulkanFences::waitAll()
    {
        std::vector<VkFence> fences;
        for (auto& entity : mFences)
        {
            fences.push_back(entity.getHandle());
        }
        VERIFYVULKANRESULT(vkWaitForFences(mDevice->getHandle(), fences.size(), fences.data(), VK_TRUE, UINT64_MAX));
    }

    VkFence VulkanFences::getFence(uint32_t index)
    {
        assert(index < mFences.size());
        return mFences[index].getHandle();
    }

    void VulkanFences::setValue(VulkanFenceEntity &value, uint32_t index)
    {
        assert(index < mFences.size());
        mFences[index] = value;
    }

    VulkanFenceEntity& VulkanFences::getEntity(uint32_t index)
    {
        assert(index < mFences.size());
        return mFences[index];
    }
}