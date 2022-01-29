//
// Created by FDC on 2022/1/29.
//

#ifndef HOMURA_VULKANLAYOUT_H
#define HOMURA_VULKANLAYOUT_H

#include <vulkan/vulkan.h>
#include <vulkanTypes.h>
#include <vector>

namespace Homura
{
    class VulkanDescriptorSetLayout
    {
    public:
        explicit VulkanDescriptorSetLayout(VulkanDevicePtr device);
        ~VulkanDescriptorSetLayout();

        void create(std::vector<VkDescriptorSetLayoutBinding> bindings);
        void destroy();
        VkDescriptorSetLayout& getHandle()
        {
            return mSetLayout;
        }

    private:
        VulkanDevicePtr         mDevice;

        VkDescriptorSetLayout   mSetLayout;
    };

    class VulkanPipelineLayout
    {
    public:
        explicit VulkanPipelineLayout(VulkanDevicePtr device);
        ~VulkanPipelineLayout();

        void create(VulkanDescriptorSetLayoutPtr descriptorSetLayout);
        void destroy();

        VkPipelineLayout& getHandle()
        {
            return mPipelineLayout;
        }
    private:
        VulkanDevicePtr     mDevice;

        VkPipelineLayout    mPipelineLayout;
    };
}

#endif //HOMURA_VULKANLAYOUT_H
