//
// Created by FDC on 2022/1/12.
//

#ifndef HOMURA_VULKANDESCRIPTOR_H
#define HOMURA_VULKANDESCRIPTOR_H
#include <vulkan/vulkan.h>
#include <vulkanTypes.h>
#include <vector>

namespace Homura
{
    class VulkanDevice;

    class VulkanDescriptorPool
    {
    public:
        VulkanDescriptorPool(VulkanDevicePtr device, uint32_t uniformCount, uint32_t imageCount, uint32_t frameCount);
        ~VulkanDescriptorPool();

        VkDescriptorPool& getHandle()
        {
            return mDescriptorPool;
        }

        uint32_t& getFrameCount()
        {
            return mFrameCount;
        }
    private:
        VulkanDevicePtr     mDevice;
        VkDescriptorPool    mDescriptorPool;
        uint32_t            mUniformCount;
        uint32_t            mImageCount;
        uint32_t            mFrameCount;
    };

    class VulkanDescriptorSetLayout
    {
    public:
        VulkanDescriptorSetLayout(VulkanDevicePtr device);
        ~VulkanDescriptorSetLayout();

        VkDescriptorSetLayout& getHandle()
        {
            return mDescriptorSetLayout;
        }

    private:
        VulkanDevicePtr         mDevice;
        VkDescriptorSetLayout   mDescriptorSetLayout;
    };

    class VulkanDescriptorSet
    {
    public:
        VulkanDescriptorSet(VulkanDevicePtr device, VulkanDescriptorPoolPtr pool, VulkanDescriptorSetLayoutPtr layout);
        ~VulkanDescriptorSet();

        const uint32_t getCount()
        {
            return mDescriptorSets.size();
        }

        VkDescriptorSet* getData()
        {
            return mDescriptorSets.data();
        }

    private:
        VulkanDevicePtr                 mDevice;
        VulkanDescriptorPoolPtr         mPool;
        VulkanDescriptorSetLayoutPtr    mLayout;
        std::vector<VkDescriptorSet>    mDescriptorSets;
    };


}
#endif //HOMURA_VULKANDESCRIPTOR_H
