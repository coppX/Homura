//
// Created by FDC on 2022/1/11.
//

#include <vulkanTexture.h>
#include <vulkanDevice.h>
#include <vulkanBuffer.h>
#include <debugUtils.h>

namespace Homura
{
    VulkanTexture::VulkanTexture(std::shared_ptr<VulkanDevice> device, uint32_t width, uint32_t height, TextureType type, VkImageTiling tiling,
                                 uint32_t mipLevels, uint32_t layerCount, uint32_t numSamples, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
        : mDevice{device}
        , mWidth{width}
        , mHeight{height}
        , mType{type}
        , mImage{VK_NULL_HANDLE}
        , mImageView{VK_NULL_HANDLE}
        , mImageMemory{VK_NULL_HANDLE}
        , mSampler{VK_NULL_HANDLE}
        , mMipLevels{mipLevels}
        , mLayerCount{layerCount}
        , mFormat{format}
        , mNumSamples{numSamples}
        , mProperties{properties}
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        switch (mType)
        {
            case TextureType::TEXTURE_1D:
                imageInfo.imageType = VK_IMAGE_TYPE_1D;
                break;
            case TextureType::TEXTURE_2D:
            case TextureType::TEXTURE_2D_ARRAY:
                imageInfo.imageType = VK_IMAGE_TYPE_2D;
                break;
            case TextureType::TEXTURE_3D:
                imageInfo.imageType = VK_IMAGE_TYPE_3D;
                break;
            case TextureType::TEXTURE_CUBE:
            case TextureType::TEXTURE_CUBE_ARRAY:
                imageInfo.imageType = VK_IMAGE_TYPE_2D;
                break;
        }

        imageInfo.extent.width = mWidth;
        imageInfo.extent.height = mHeight;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mMipLevels;
        imageInfo.arrayLayers = mLayerCount;
        imageInfo.format = mFormat;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = mImageLayout;
        imageInfo.usage = usage;
        switch (mNumSamples)
        {
            case 1:
                imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
                break;
            case 2:
                imageInfo.samples = VK_SAMPLE_COUNT_2_BIT;
                break;
            case 4:
                imageInfo.samples = VK_SAMPLE_COUNT_4_BIT;
                break;
            case 8:
                imageInfo.samples = VK_SAMPLE_COUNT_8_BIT;
                break;
            case 16:
                imageInfo.samples = VK_SAMPLE_COUNT_16_BIT;
                break;
            case 32:
                imageInfo.samples = VK_SAMPLE_COUNT_32_BIT;
                break;
            case 64:
                imageInfo.samples = VK_SAMPLE_COUNT_64_BIT;
                break;
        }
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VERIFYVULKANRESULT(vkCreateImage(mDevice->getHandle(), &imageInfo, nullptr, &mImage));

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(mDevice->getHandle(), mImage, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, mProperties);

        VERIFYVULKANRESULT(vkAllocateMemory(mDevice->getHandle(), &allocInfo, nullptr, &mImageMemory));

        vkBindImageMemory(mDevice->getHandle(), mImage, mImageMemory, 0);
    }

    uint32_t VulkanTexture::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(mDevice->getPhysicalHandle(), &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
        {
            if ((typeFilter & 1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }
        return -1;
    }
}