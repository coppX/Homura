//
// Created by 最上川 on 2022/1/10.
//

#ifndef HOMURA_VULKANTEXTURE_H
#define HOMURA_VULKANTEXTURE_H
#include <vulkan/vulkan.h>
#include <vulkanTypes.h>

namespace Homura
{
    enum TextureType
    {
        TEXTURE_1D = 0,
        TEXTURE_2D,
        TEXTURE_3D,
        TEXTURE_CUBE,
        TEXTURE_1D_ARRAY,
        TEXTURE_2D_ARRAY,
        TEXTURE_CUBE_ARRAY,
        TEXTURE_TYPE_SIZE
    };

    static const VkImageTiling VulkanViewTypeTilingMode[TEXTURE_TYPE_SIZE] =
    {
        VK_IMAGE_TILING_LINEAR,     // TEXTURE_1D
        VK_IMAGE_TILING_OPTIMAL,    // TEXTURE_2D
        VK_IMAGE_TILING_OPTIMAL,    // TEXTURE_3D
        VK_IMAGE_TILING_OPTIMAL,    // TEXTURE_CUBE
        VK_IMAGE_TILING_LINEAR,     // TEXTURE_1D_ARRAY
        VK_IMAGE_TILING_OPTIMAL,    // TEXTURE_2D_ARRAY
        VK_IMAGE_TILING_OPTIMAL,    // TEXTURE_CUBE_ARRAY
    };

    class VulkanTexture
    {
    public:
        VulkanTexture(VulkanDevicePtr device, uint32_t width, uint32_t height, TextureType type, VkImageTiling tiling, VkImageAspectFlags aspectFlags,
                      uint32_t mipLevels, uint32_t arraySize, uint32_t numSamples, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
        ~VulkanTexture();

//        void fromBuffer(VulkanBufferPtr buffer, uint32_t width, uint32_t height);

        VkImage& getImage()
        {
            return mImage;
        }

        VkImageView& getImageView()
        {
            return mImageView;
        }

        void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                         VkMemoryPropertyFlags properties);

        void createImageView(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        void setImageLayout(VkImageLayout newLayout,
                            VkPipelineStageFlags srcStageMask,
                            VkPipelineStageFlags dstStageMask,
                            VkImageSubresourceRange subresourceRange,
                            VulkanCommandBufferPtr command);

        void generateMipmaps(VulkanCommandBufferPtr commandBuffer, VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
    private:
        VulkanDevicePtr                 mDevice;

        VkImage                         mImage;
        VkImageView                     mImageView;
        VkDeviceMemory                  mImageMemory;
        VkFormat                        mFormat;
        VkImageLayout                   mImageLayout;
        TextureType                     mType;

        uint32_t                        mWidth, mHeight;
        uint32_t                        mMipLevels, mLayerCount;
        uint32_t                        mNumSamples;

        VkMemoryPropertyFlags           mProperties;
    };

    class VulkanTexture1D : public VulkanTexture
    {
    public:
        VulkanTexture1D(VulkanDevicePtr device, uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t layerCount, uint32_t numSamples,
                        VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
            : VulkanTexture(device, width, height, TEXTURE_1D, VulkanViewTypeTilingMode[TEXTURE_1D], VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, layerCount, numSamples, format, usage, properties)
        {

        }
    };

    class VulkanTexture2D : public VulkanTexture
    {
    public:
        VulkanTexture2D(VulkanDevicePtr device, uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t numSamples,
                VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
        : VulkanTexture(device, width, height, TEXTURE_2D, VulkanViewTypeTilingMode[TEXTURE_2D], VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 1, numSamples, format, usage, properties)
        {

        }
    };

    class VulkanTexture2DArray : public VulkanTexture
    {
    public:
        VulkanTexture2DArray(VulkanDevicePtr device, uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t ArraySize, uint32_t numSamples,
                VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
        : VulkanTexture(device, width, height, TEXTURE_2D_ARRAY, VulkanViewTypeTilingMode[TEXTURE_2D_ARRAY], VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, ArraySize, numSamples, format, usage, properties)
        {

        }
    };

    class VulkanTexture3D : public VulkanTexture
    {
    public:
        VulkanTexture3D(VulkanDevicePtr device, uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t numSamples,
                VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
        : VulkanTexture(device, width, height, TEXTURE_3D, VulkanViewTypeTilingMode[TEXTURE_3D], VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 1, numSamples, format, usage, properties)
        {

        }
    };

    class VulkanTextureCube : public VulkanTexture
    {
    public:
        VulkanTextureCube(VulkanDevicePtr device, uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t numSamples,
                VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
        : VulkanTexture(device, width, height, TEXTURE_CUBE, VulkanViewTypeTilingMode[TEXTURE_CUBE], VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 1, numSamples, format, usage, properties)
        {

        }
    };

    class VulkanTextureCubeArray : public VulkanTexture
    {
    public:
        VulkanTextureCubeArray(VulkanDevicePtr device, uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t arraySize, uint32_t numSamples,
                          VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
                : VulkanTexture(device, width, height, TEXTURE_CUBE_ARRAY, VulkanViewTypeTilingMode[TEXTURE_CUBE_ARRAY], VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, arraySize, numSamples, format, usage, properties)
        {

        }
    };

    class VulkanTextureDepth : public VulkanTexture
    {
    public:
        VulkanTextureDepth(VulkanDevicePtr device, uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t numSamples,
                VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
        : VulkanTexture(device, width, height, TEXTURE_2D, VulkanViewTypeTilingMode[TEXTURE_2D], VK_IMAGE_ASPECT_DEPTH_BIT, mipLevels, 1, numSamples, format, usage, properties)
        {

        }
    };
}
#endif //HOMURA_VULKANTEXTURE_H
