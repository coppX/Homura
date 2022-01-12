//
// Created by 最上川 on 2022/1/12.
//

#include <vulkanDescriptorSet.h>
#include <vulkanDevice.h>
#include <debugUtils.h>

#include <vector>
namespace Homura
{
    VulkanDescriptorPool::VulkanDescriptorPool(std::shared_ptr<VulkanDevice> device, uint32_t uniformCount, uint32_t imageCount, uint32_t frameCount)
        : mDevice{device}
        , mDescriptorPool{VK_NULL_HANDLE}
        , mUniformCount{uniformCount}
        , mImageCount{imageCount}
        , mFrameCount{frameCount}
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

    VulkanDescriptorPool::~VulkanDescriptorPool()
    {
        if (mDescriptorPool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(mDevice->getHandle(), mDescriptorPool, nullptr);
        }
    }
    
}