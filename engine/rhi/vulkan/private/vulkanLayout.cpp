//
// Created by FDC on 2022/1/29.
//

#include <vulkanLayout.h>
#include <vulkanDevice.h>
#include <debugUtils.h>

namespace Homura
{
    VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDevicePtr device)
        : mDevice{device}
        , mSetLayout{VK_NULL_HANDLE}
    {

    }

    void VulkanDescriptorSetLayout::create(const std::vector<VkDescriptorSetLayoutBinding>& bindings)
    {
        VkDescriptorSetLayoutCreateInfo createInfo{};
        createInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        createInfo.pBindings    = bindings.data();
        createInfo.pNext        = nullptr;

        VERIFYVULKANRESULT(vkCreateDescriptorSetLayout(mDevice->getHandle(), &createInfo, nullptr, &mSetLayout));
    }

    void VulkanDescriptorSetLayout::destroy()
    {
        if (mSetLayout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(mDevice->getHandle(), mSetLayout, nullptr);
            mSetLayout = VK_NULL_HANDLE;
        }
    }

    VulkanPipelineLayout::VulkanPipelineLayout(VulkanDevicePtr device)
        : mDevice{device}
        , mPipelineLayout{VK_NULL_HANDLE}
    {

    }

    void VulkanPipelineLayout::create(VulkanDescriptorSetLayoutPtr descriptorSetLayout)
    {
        VkPipelineLayoutCreateInfo createInfo{};
        createInfo.sType            = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        createInfo.setLayoutCount   = 1;
        createInfo.pSetLayouts      = &descriptorSetLayout->getHandle();

        VERIFYVULKANRESULT(vkCreatePipelineLayout(mDevice->getHandle(), &createInfo, nullptr, &mPipelineLayout));
    }

    void VulkanPipelineLayout::destroy()
    {
        if (mPipelineLayout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(mDevice->getHandle(), mPipelineLayout, nullptr);
            mPipelineLayout = VK_NULL_HANDLE;
        }
    }
}