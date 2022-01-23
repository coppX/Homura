//
// Created by 最上川 on 2022/1/12.
//

#include <vulkanDescriptorSet.h>
#include <vulkanDevice.h>
#include <debugUtils.h>

namespace Homura
{
    VulkanDescriptorPool::VulkanDescriptorPool(VulkanDevicePtr device, uint32_t uniformCount, uint32_t imageCount, uint32_t frameCount)
        : mDevice{device}
        , mDescriptorPool{VK_NULL_HANDLE}
        , mUniformCount{uniformCount}
        , mImageCount{imageCount}
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
        uniformBufferSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformBufferSize.descriptorCount = mUniformCount;
        poolSize.push_back(uniformBufferSize);

        VkDescriptorPoolSize textureSize{};
        textureSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        textureSize.descriptorCount = mImageCount;
        poolSize.push_back(textureSize);

        VkDescriptorPoolCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        createInfo.poolSizeCount = static_cast<uint32_t>(poolSize.size());
        createInfo.pPoolSizes = poolSize.data();
        createInfo.maxSets = mFrameCount;

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
        std::vector<VkDescriptorSetLayout> layouts(mPool->getFrameCount(), mLayout->getHandle());
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = mPool->getHandle();
        allocInfo.descriptorSetCount = mPool->getFrameCount();
        allocInfo.pSetLayouts = layouts.data();

        mDescriptorSets.resize(mPool->getFrameCount());
        VERIFYVULKANRESULT(vkAllocateDescriptorSets(mDevice->getHandle(), &allocInfo, mDescriptorSets.data()));
    }

    VulkanDescriptorSet::~VulkanDescriptorSet()
    {
        if (!mDescriptorSets.empty())
        {
            vkFreeDescriptorSets(mDevice->getHandle(), mPool->getHandle(), mDescriptorSets.size(), mDescriptorSets.data());
        }
    }

    VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDevicePtr device)
        : mDevice{device}
        , mDescriptorSetLayout{VK_NULL_HANDLE}
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::vector<VkDescriptorSetLayoutBinding> bindings{uboLayoutBinding, samplerLayoutBinding};
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        VERIFYVULKANRESULT(vkCreateDescriptorSetLayout(mDevice->getHandle(), &layoutInfo, nullptr, &mDescriptorSetLayout));
    }

    VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
    {
        if (mDescriptorSetLayout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(mDevice->getHandle(), mDescriptorSetLayout, nullptr);
        }
    }
}