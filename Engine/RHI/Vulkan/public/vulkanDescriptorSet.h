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
        VulkanDescriptorPool();
        ~VulkanDescriptorPool();
    private:
        std::shared_ptr<VulkanDevice> mDevice;
    };

    class VulkanDescriptorSet
    {

    };

    class VulkanDescriptorSetLayout
    {

    };
}
#endif //HOMURA_VULKANDESCRIPTOR_H
