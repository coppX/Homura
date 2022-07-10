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
    class VulkanDescriptorPool
    {
    public:
        VulkanDescriptorPool(VulkanDevicePtr device, uint32_t frameCount);
        ~VulkanDescriptorPool();

        VkDescriptorPool& getHandle()
        {
            return mDescriptorPool;
        }

        void create();
        void destroy();

        uint32_t getFrameCount()
        {
            return mFrameCount;
        }
    private:
        VulkanDevicePtr     mDevice;
        VkDescriptorPool    mDescriptorPool;
        uint32_t            mFrameCount;
    };

    class VulkanDescriptorSet
    {
    public:
        VulkanDescriptorSet(VulkanDevicePtr device, VulkanDescriptorPoolPtr pool, VulkanDescriptorSetLayoutPtr layout);
        ~VulkanDescriptorSet();

        void create();
        void destroy();

        const uint32_t getCount()
        {
            return (uint32_t )mDescriptorSets.size();
        }

        std::vector<VkDescriptorSet>& getData()
        {
            return mDescriptorSets;
        }

        VulkanDescriptorSetLayoutPtr getLayout()
        {
            return mLayout;
        }
    private:
        VulkanDevicePtr                 mDevice;
        VulkanDescriptorPoolPtr         mPool;
        VulkanDescriptorSetLayoutPtr    mLayout;
        std::vector<VkDescriptorSet>    mDescriptorSets;
    };
}
#endif //HOMURA_VULKANDESCRIPTOR_H
