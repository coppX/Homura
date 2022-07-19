//
// Created by 最上川 on 2022/2/1.
//
#include <vulkanSampler.h>
#include <vulkanDevice.h>
#include <debugUtils.h>

namespace Homura
{
    VulkanSampler::VulkanSampler(VulkanDevicePtr device)
        : mDevice{device}
    {
        create();
    }

    VulkanSampler::~VulkanSampler()
    {
        destroy();
    }

    void VulkanSampler::create()
    {
        VkSamplerCreateInfo createInfo{};
        createInfo.sType                    = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        createInfo.magFilter                = VK_FILTER_LINEAR;
        createInfo.minFilter                = VK_FILTER_LINEAR;
        createInfo.addressModeU             = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        createInfo.addressModeV             = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        createInfo.addressModeW             = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        createInfo.anisotropyEnable         = VK_TRUE;
        createInfo.maxAnisotropy            = 16;

        createInfo.borderColor              = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        createInfo.unnormalizedCoordinates  = VK_FALSE;

        createInfo.compareEnable            = VK_FALSE;
        createInfo.compareOp                = VK_COMPARE_OP_ALWAYS;

        createInfo.mipmapMode               = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        createInfo.mipLodBias               = 0.0f;
        createInfo.minLod                   = 0.0f;
        createInfo.maxLod                   = 0.0f;

        VERIFYVULKANRESULT(vkCreateSampler(mDevice->getHandle(), &createInfo, nullptr, &mSampler));
    }

    void VulkanSampler::destroy()
    {
        if (mSampler != VK_NULL_HANDLE)
        {
            vkDestroySampler(mDevice->getHandle(), mSampler, nullptr);
            mSampler = VK_NULL_HANDLE;
        }
    }
}
