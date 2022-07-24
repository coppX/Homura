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
                                 uint32_t mipLevels, uint32_t arraySize, VkSampleCountFlagBits numSamples, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
        : mDevice{device}
        , mWidth{width}
        , mHeight{height}
        , mType{type}
        , mImage{VK_NULL_HANDLE}
        , mImageView{VK_NULL_HANDLE}
        , mImageMemory{VK_NULL_HANDLE}
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

    }

    void VulkanTexture::destroy()
    {
        if (mImageView != VK_NULL_HANDLE)
        {
            vkDestroyImageView(mDevice->getHandle(), mImageView, nullptr);
            mImageView = VK_NULL_HANDLE;
        }
        if (mImage != VK_NULL_HANDLE)
        {
            vkDestroyImage(mDevice->getHandle(), mImage, nullptr);
            mImage = VK_NULL_HANDLE;
        }
        if (mImageMemory != VK_NULL_HANDLE)
        {
            vkFreeMemory(mDevice->getHandle(), mImageMemory, nullptr);
            mImageMemory = VK_NULL_HANDLE;
        }
    }

    void VulkanTexture::createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                                    VkMemoryPropertyFlags properties)
    {
        VkImageCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        switch (mType)
        {
            case TextureType::TEXTURE_1D:
            case TextureType::TEXTURE_1D_ARRAY:
                createInfo.imageType = VK_IMAGE_TYPE_1D;
                break;
            case TextureType::TEXTURE_2D:
            case TextureType::TEXTURE_2D_ARRAY:
                createInfo.imageType = VK_IMAGE_TYPE_2D;
                break;
            case TextureType::TEXTURE_3D:
                createInfo.imageType = VK_IMAGE_TYPE_3D;
                break;
            case TextureType::TEXTURE_CUBE:
            case TextureType::TEXTURE_CUBE_ARRAY:
                createInfo.imageType = VK_IMAGE_TYPE_2D;
                break;
            default:
                std::cerr<< "Unknown Texture Type!";
                break;
        }

        createInfo.extent.width  = width;
        createInfo.extent.height = height;
        createInfo.extent.depth  = 1;
        createInfo.mipLevels     = mipLevels;
        createInfo.arrayLayers   = mLayerCount;
        createInfo.format        = format;
        createInfo.tiling        = tiling;
        createInfo.initialLayout = mImageLayout;
        createInfo.usage         = usage;
        createInfo.samples       = numSamples;
        createInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;


        VERIFYVULKANRESULT(vkCreateImage(mDevice->getHandle(), &createInfo, nullptr, &mImage));

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
        viewInfo.format                             = format;
        viewInfo.subresourceRange.aspectMask        = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel      = 0;
        viewInfo.subresourceRange.levelCount        = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer    = 0;
        viewInfo.subresourceRange.layerCount        = 1;
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

    void VulkanTexture::fromBuffer(VulkanCommandBufferPtr command, VulkanBufferPtr buffer)
    {
        VkCommandBuffer commandBuffer = command->beginSingleTimeCommands();
        VkBufferImageCopy region{};
        region.bufferOffset                     = 0;
        region.bufferRowLength                  = 0;
        region.bufferImageHeight                = 0;
        region.imageSubresource.aspectMask      = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel        = 0;
        region.imageSubresource.baseArrayLayer  = 0;
        region.imageSubresource.layerCount      = 1;
        region.imageOffset                      = {0, 0, 0};
        region.imageExtent                      = {mWidth, mHeight, 1};

        vkCmdCopyBufferToImage(commandBuffer, buffer->getHandle(), mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
        command->endSingleTimeCommands(commandBuffer);
    }

    void VulkanTexture::setImageLayout(VkImageLayout newLayout, VulkanCommandBufferPtr command)
    {
        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType                            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.oldLayout                        = mImageLayout;
        imageMemoryBarrier.newLayout                        = newLayout;
        imageMemoryBarrier.srcQueueFamilyIndex              = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex              = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.image                            = mImage;
        imageMemoryBarrier.subresourceRange.aspectMask      = VK_IMAGE_ASPECT_COLOR_BIT;
        imageMemoryBarrier.subresourceRange.baseMipLevel    = 0;
        imageMemoryBarrier.subresourceRange.levelCount      = mMipLevels;
        imageMemoryBarrier.subresourceRange.baseArrayLayer  = 0;
        imageMemoryBarrier.subresourceRange.layerCount      = 1;

        VkPipelineStageFlags srcStageMask;
        VkPipelineStageFlags dstStageMask;

        if (mImageLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            imageMemoryBarrier.srcAccessMask = 0;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (mImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else
        {
            throw std::invalid_argument("unsupported layout transition!");
        }

        VkCommandBuffer commandBuffer = command->beginSingleTimeCommands();
        command->transferImageLayout(commandBuffer, imageMemoryBarrier, srcStageMask, dstStageMask);
        command->endSingleTimeCommands(commandBuffer);
        mImageLayout = newLayout;
    }

    void VulkanTexture::generateMipmaps(VulkanCommandBufferPtr command)
    {
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(mDevice->getPhysicalHandle(), mFormat, &formatProperties);

        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
        {
            throw std::runtime_error("texture image format does not support linear blitting!");
        }

        VkCommandBuffer commandBuffer = command->beginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image                           = mImage;
        barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = 1;
        barrier.subresourceRange.levelCount     = 1;


        int32_t mipWidth = mWidth;
        int32_t mipHeight = mHeight;

        for (uint32_t i = 1; i < mMipLevels; i++)
        {
            barrier.subresourceRange.baseArrayLayer = i - 1;
            barrier.oldLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask                   = VK_ACCESS_TRANSFER_READ_BIT;

            command->transferImageLayout(commandBuffer, barrier, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

            VkImageBlit blit{};
            blit.srcOffsets[0]                      = {0, 0, 0};
            blit.srcOffsets[1]                      = {mipWidth, mipHeight, 1};
            blit.srcSubresource.aspectMask          = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel            = i - 1;
            blit.srcSubresource.baseArrayLayer      = 0;
            blit.srcSubresource.layerCount          = 1;
            blit.dstOffsets[0]                      = {0, 0, 0};
            blit.dstOffsets[1]                      = {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
            blit.dstSubresource.aspectMask          = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel            = i;
            blit.dstSubresource.baseArrayLayer      = 0;
            blit.dstSubresource.layerCount          = 1;

            command->blitImage(commandBuffer, mImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1, &blit,
                           VK_FILTER_LINEAR);

            barrier.oldLayout       = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask   = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask   = VK_ACCESS_SHADER_READ_BIT;

            command->transferImageLayout(commandBuffer, barrier, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }

        barrier.subresourceRange.baseMipLevel   = mMipLevels - 1;
        barrier.oldLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout                       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask                   = VK_ACCESS_SHADER_READ_BIT;

        command->transferImageLayout(commandBuffer, barrier, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
        command->endSingleTimeCommands(commandBuffer);
    }

    VkWriteDescriptorSet VulkanTexture2D::createWriteDescriptorSet(VkDescriptorSet descriptorSet)
    {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = mImageView;
        imageInfo.sampler = mSampler->getHandle();

        VkWriteDescriptorSet descriptorWrite;
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSet;
        descriptorWrite.dstBinding = mBinding;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pImageInfo = &imageInfo;
        return descriptorWrite;
    }
}