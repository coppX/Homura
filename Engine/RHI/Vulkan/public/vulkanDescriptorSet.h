//
// Created by FDC on 2022/1/12.
//

#ifndef HOMURA_VULKANDESCRIPTOR_H
#define HOMURA_VULKANDESCRIPTOR_H
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

namespace Homura
{
    class VulkanDevice;

    class VulkanDescriptorPool
    {
    public:
        VulkanDescriptorPool(std::shared_ptr<VulkanDevice> device, uint32_t uniformCount, uint32_t imageCount, uint32_t frameCount);
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
        std::shared_ptr<VulkanDevice>   mDevice;
        VkDescriptorPool                mDescriptorPool;
        uint32_t                        mUniformCount;
        uint32_t                        mImageCount;
        uint32_t                        mFrameCount;
    };

    class VulkanDescriptorSetLayout
    {
    public:
        VulkanDescriptorSetLayout(std::shared_ptr<VulkanDevice> device);
        ~VulkanDescriptorSetLayout();

        VkDescriptorSetLayout& getHandle()
        {
            return mDescriptorSetLayout;
        }

    private:
        std::shared_ptr<VulkanDevice>   mDevice;
        VkDescriptorSetLayout           mDescriptorSetLayout;
    };

    class VulkanDescriptorSet
    {
    public:
        VulkanDescriptorSet(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanDescriptorPool> pool, std::shared_ptr<VulkanDescriptorSetLayout> layout);
        ~VulkanDescriptorSet();

        uint32_t getCount()
        {
            return mDescriptorSets.size();
        }

        VkDescriptorSet* getData()
        {
            return mDescriptorSets.data();
        }

    private:
        std::shared_ptr<VulkanDevice>               mDevice;
        std::shared_ptr<VulkanDescriptorPool>       mPool;
        std::shared_ptr<VulkanDescriptorSetLayout>  mLayout;
        std::vector<VkDescriptorSet>                mDescriptorSets;
    };


}
#endif //HOMURA_VULKANDESCRIPTOR_H
