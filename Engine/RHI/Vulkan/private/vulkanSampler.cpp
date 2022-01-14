//
// Created by FDC on 2022/1/11.
//
#include <vulkanSampler.h>
#include <vulkanDevice.h>
#include <debugUtils.h>

namespace Homura
{
    VulkanSampler::VulkanSampler(VulkanDevicePtr device)
        : mDevice{device}
        , mSampler{VK_NULL_HANDLE}
    {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(mDevice->getPhysicalHandle(), &properties);

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        VERIFYVULKANRESULT(vkCreateSampler(mDevice->getHandle(), &samplerInfo, nullptr, &mSampler));
    }

    VulkanSampler::~VulkanSampler()
    {
        if (mSampler != VK_NULL_HANDLE)
        {
            vkDestroySampler(mDevice->getHandle(), mSampler, nullptr);
            mSampler = VK_NULL_HANDLE;
        }
    }

}
