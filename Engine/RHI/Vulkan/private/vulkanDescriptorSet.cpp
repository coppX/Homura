//
// Created by 最上川 on 2022/1/12.
//

#include <vulkanDescriptorSet.h>
#include <vulkanLayout.h>
#include <vulkanDevice.h>
#include <vulkanSwapChain.h>
#include <debugUtils.h>

namespace Homura
{
    VulkanDescriptorPool::VulkanDescriptorPool(VulkanDevicePtr device, uint32_t frameCount)
        : mDevice{device}
        , mDescriptorPool{VK_NULL_HANDLE}
        , mFrameCount{frameCount}
    {
        create();
    }

    VulkanDescriptorPool::~VulkanDescriptorPool()
    {
        destroy();
    }

    void VulkanDescriptorPool::create()
    {
        std::vector<VkDescriptorPoolSize> poolSize{};

        VkDescriptorPoolSize uniformBufferSize{};
        uniformBufferSize.type              = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformBufferSize.descriptorCount   = mFrameCount;
        poolSize.push_back(uniformBufferSize);

        VkDescriptorPoolSize textureSize{};
        textureSize.type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        textureSize.descriptorCount = mFrameCount;
        poolSize.push_back(textureSize);

        VkDescriptorPoolCreateInfo createInfo{};
        createInfo.sType            = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        createInfo.poolSizeCount    = static_cast<uint32_t>(poolSize.size());
        createInfo.pPoolSizes       = poolSize.data();
        createInfo.maxSets          = mFrameCount;
        createInfo.flags            = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

        VERIFYVULKANRESULT(vkCreateDescriptorPool(mDevice->getHandle(), &createInfo, nullptr, &mDescriptorPool));
    }

    void VulkanDescriptorPool::destroy()
    {
        if (mDescriptorPool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(mDevice->getHandle(), mDescriptorPool, nullptr);
            mDescriptorPool = VK_NULL_HANDLE;
        }
    }

    VulkanDescriptorSet::VulkanDescriptorSet(VulkanDevicePtr device, VulkanDescriptorPoolPtr pool, VulkanDescriptorSetLayoutPtr layout)
        : mDevice{device}
        , mPool{pool}
        , mLayout{layout}
        , mDescriptorSets{0}
    {
        create();
    }

    VulkanDescriptorSet::~VulkanDescriptorSet()
    {
        destroy();
    }

    void VulkanDescriptorSet::create()
    {
        std::vector<VkDescriptorSetLayout> layouts(mPool->getFrameCount(), mLayout->getHandle());
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType                 = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool        = mPool->getHandle();
        allocInfo.descriptorSetCount    = mPool->getFrameCount();
        allocInfo.pSetLayouts           = layouts.data();

        mDescriptorSets.resize(mPool->getFrameCount());
        VERIFYVULKANRESULT(vkAllocateDescriptorSets(mDevice->getHandle(), &allocInfo, mDescriptorSets.data()));
    }

    void VulkanDescriptorSet::destroy()
    {
        mLayout->destroy();
        if (!mDescriptorSets.empty())
        {
            vkFreeDescriptorSets(mDevice->getHandle(), mPool->getHandle(), static_cast<uint32_t >(mDescriptorSets.size()), mDescriptorSets.data());
            mDescriptorSets.clear();
        }
    }
}