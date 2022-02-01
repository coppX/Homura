//
// Created by FDC on 2022/1/11.
//

#include <vulkanTexture.h>
#include <vulkanDevice.h>
#include <vulkanBuffer.h>
#include <vulkanCommandBuffer.h>
#include <debugUtils.h>

namespace Homura
{
    VulkanTexture::VulkanTexture(VulkanDevicePtr device, uint32_t width, uint32_t height, TextureType type, VkImageTiling tiling, VkImageAspectFlags aspectFlags,
                                 uint32_t mipLevels, uint32_t arraySize, uint32_t numSamples, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
        : mDevice{device}
        , mWidth{width}
        , mHeight{height}
        , mType{type}
        , mImage{VK_NULL_HANDLE}
        , mImageView{VK_NULL_HANDLE}
        , mImageMemory{VK_NULL_HANDLE}
        , mSampler{VK_NULL_HANDLE}
        , mMipLevels{mipLevels}
        , mLayerCount{arraySize * ((mType == TEXTURE_CUBE || mType == TEXTURE_CUBE_ARRAY) ? 6 : 1)}
        , mFormat{format}
        , mNumSamples{numSamples}
        , mProperties{properties}
        , mImageLayout{VK_IMAGE_LAYOUT_UNDEFINED}
    {
        createImage(mWidth, mHeight, mMipLevels, mNumSamples, mFormat, tiling, usage, mProperties);
        createImageView(mFormat, aspectFlags, mMipLevels);
    }

    VulkanTexture::~VulkanTexture()
    {
        if (mImageView != VK_NULL_HANDLE)
        {
            vkDestroyImageView(mDevice->getHandle(), mImageView, nullptr);
        }
        if (mImage != VK_NULL_HANDLE)
        {
            vkDestroyImage(mDevice->getHandle(), mImage, nullptr);
        }
        if (mImageMemory != VK_NULL_HANDLE)
        {
            vkFreeMemory(mDevice->getHandle(), mImageMemory, nullptr);
        }
    }

    void VulkanTexture::createImage(uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                                    VkMemoryPropertyFlags properties)
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        switch (mType)
        {
            case TextureType::TEXTURE_1D:
            case TextureType::TEXTURE_1D_ARRAY:
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
            default:
                std::cerr<< "Unknown Texture Type!";
                break;
        }

        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = mLayerCount;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = mImageLayout;
        imageInfo.usage = usage;
        switch (numSamples)
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

    void VulkanTexture::createImageView(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = mImage;
        switch (mType)
        {
            case TextureType::TEXTURE_1D:
                viewInfo.viewType = VK_IMAGE_VIEW_TYPE_1D;
                break;
            case TextureType::TEXTURE_1D_ARRAY:
                viewInfo.viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
                break;
            case TextureType::TEXTURE_2D:
                viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                break;
            case TextureType::TEXTURE_2D_ARRAY:
                viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
                break;
            case TextureType::TEXTURE_CUBE:
                viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
                break;
            case TextureType::TEXTURE_CUBE_ARRAY:
                viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
                break;
            case TextureType::TEXTURE_3D:
                viewInfo.viewType = VK_IMAGE_VIEW_TYPE_3D;
                break;
            default:
                std::cerr<< "Unknown Texture Type!";
                break;
        }
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        VERIFYVULKANRESULT(vkCreateImageView(mDevice->getHandle(), &viewInfo, nullptr, &mImageView));
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

//    void VulkanTexture::fromBuffer(VulkanBufferPtr buffer, uint32_t width, uint32_t height)
//    {
//        VulkanCommandBuffer commandBuffer{mDevice};
//        commandBuffer.beginSingleTimeCommands();
//
//        VkBufferImageCopy region{};
//        region.bufferOffset = 0;
//        region.bufferRowLength = 0;
//        region.bufferImageHeight = 0;
//        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//        region.imageSubresource.mipLevel = 0;
//        region.imageSubresource.baseArrayLayer = 0;
//        region.imageSubresource.layerCount = 1;
//        region.imageOffset = {0, 0, 0};
//        region.imageExtent = {
//                width,
//                height,
//                1
//        };
//        vkCmdCopyBufferToImage(commandBuffer.getHandle(), buffer->getHandle(), mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
//
//        commandBuffer.endSingleTimeCommands();
//    }

    void VulkanTexture::createSampler(int mipLevels)
    {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(mDevice->getPhysicalHandle(), &properties);

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;;

        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.minLod = 0;
        samplerInfo.maxLod = static_cast<float>(mipLevels);
        samplerInfo.mipLodBias = 0;

        VERIFYVULKANRESULT(vkCreateSampler(mDevice->getHandle(), &samplerInfo, nullptr, &mSampler));
    }

    void VulkanTexture::setImageLayout(VkImageLayout newLayout,
                        VkPipelineStageFlags srcStageMask,
                        VkPipelineStageFlags dstStageMask,
                        VkImageSubresourceRange subresourceRange,
                        VulkanCommandBufferPtr command)
    {
        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.oldLayout = mImageLayout;
        imageMemoryBarrier.newLayout = newLayout;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.image = mImage;
        imageMemoryBarrier.subresourceRange = subresourceRange;

        switch (mImageLayout)
        {
            case VK_IMAGE_LAYOUT_UNDEFINED:
                imageMemoryBarrier.srcAccessMask = 0;
                break;
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                break;
            default:
                break;
        }

        switch (newLayout)
        {
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                break;
            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
                if (imageMemoryBarrier.srcAccessMask == 0)
                {
                    imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
                }
                imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                break;
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                break;
            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
                imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                break;
            default:
                break;
        }

        mImageLayout = newLayout;
        command->beginSingleTimeCommands();
        command->transferImageLayout(imageMemoryBarrier, srcStageMask, dstStageMask);
        command->endSingleTimeCommands();
    }

    void VulkanTexture::generateMipmaps(VulkanCommandBufferPtr commandBuffer, VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
    {
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(mDevice->getPhysicalHandle(), imageFormat, &formatProperties);

        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
        {
            throw std::runtime_error("texture image format does not support linear blitting!");
        }

        commandBuffer->beginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = texWidth;
        int32_t mipHeight = texHeight;

        for (uint32_t i = 1; i < mipLevels; i++)
        {
            barrier.subresourceRange.baseArrayLayer = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            commandBuffer->transferImageLayout(barrier, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

            VkImageBlit blit{};
            blit.srcOffsets[0] = {0, 0, 0};
            blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = {0, 0, 0};
            blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            commandBuffer->blitImage(image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1, &blit,
                           VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            commandBuffer->transferImageLayout(barrier, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }

        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        commandBuffer->transferImageLayout(barrier, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
        commandBuffer->endSingleTimeCommands();
    }
}