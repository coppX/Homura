//
// Created by FDC on 2022/1/12.
//

#ifndef HOMURA_VULKANDESCRIPTOR_H
#define HOMURA_VULKANDESCRIPTOR_H
#include <vulkan/vulkan.h>
#include <memory>

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
    private:
        std::shared_ptr<VulkanDevice>   mDevice;
        VkDescriptorPool                mDescriptorPool;
        uint32_t                        mUniformCount;
        uint32_t                        mImageCount;
        uint32_t                        mFrameCount;
    };

    class VulkanDescriptorSet
    {

    };

    class VulkanDescriptorSetLayout
    {

    };
}
#endif //HOMURA_VULKANDESCRIPTOR_H
