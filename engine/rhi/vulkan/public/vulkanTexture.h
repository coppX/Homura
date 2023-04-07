//
// Created by 最上川 on 2022/1/10.
//

#ifndef HOMURA_VULKANTEXTURE_H
#define HOMURA_VULKANTEXTURE_H
#include <vulkan/vulkan.h>
#include <vulkanTypes.h>
#include <vulkanSampler.h>

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

    class ENGINE_API VulkanTexture
    {
    public:
        VulkanTexture(VulkanDevicePtr device, uint32_t width, uint32_t height, TextureType type, VkImageTiling tiling, VkImageAspectFlags aspectFlags,
                      uint32_t mipLevels, uint32_t arraySize, VkSampleCountFlagBits numSamples, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
        ~VulkanTexture() = default;

        void destroy();
        void fromBuffer(VulkanCommandBufferPtr commandBuffer, VulkanBufferPtr buffer);

        VkImage& getImage()
        {
            return mImage;
        }

        VkImageView& getImageView()
        {
            return mImageView;
        }

        virtual VkFormat& getFormat()
        {
            return mFormat;
        }

        void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                         VkMemoryPropertyFlags properties);

        void createImageView(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        void setImageLayout(VkImageLayout newLayout, VulkanCommandBufferPtr command);

        void generateMipmaps(VulkanCommandBufferPtr command);
    private:
        VulkanDevicePtr                 mDevice;

        VkImage                         mImage;
        VkDeviceMemory                  mImageMemory;
        VkFormat                        mFormat;
        VkImageLayout                   mImageLayout;
        TextureType                     mType;

        uint32_t                        mWidth, mHeight;
        uint32_t                        mMipLevels, mLayerCount;
        VkSampleCountFlagBits           mNumSamples;
        VkMemoryPropertyFlags           mProperties;
    public:
        VkImageView                     mImageView;
    };

    class ENGINE_API VulkanTexture1D : public VulkanTexture
    {
    public:
        VulkanTexture1D(VulkanDevicePtr device, uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t layerCount, VkSampleCountFlagBits numSamples,
                VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
        : VulkanTexture(device, width, height, TEXTURE_1D, VulkanViewTypeTilingMode[TEXTURE_1D], VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, layerCount, numSamples, format, usage, properties)
        {

        }
    };

    class ENGINE_API VulkanTexture2D : public VulkanTexture
    {
    public:
        VulkanTexture2D(VulkanDevicePtr device, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
                VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
        : VulkanTexture(device, width, height, TEXTURE_2D, VulkanViewTypeTilingMode[TEXTURE_2D], VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 1, numSamples, format, usage, properties)
        , mSampler{}
        , mBinding{0}
        , mImageInfo{}
        {

        }

        VkWriteDescriptorSet createWriteDescriptorSet(VkDescriptorSet descriptorSet);
        void setSampler(VulkanSamplerPtr sampler, uint32_t binding)
        {
            mSampler = sampler;
            mBinding = binding;
        }

        int getBinding()
        {
            return mBinding;
        }
    public:
        VulkanSamplerPtr        mSampler;
        uint32_t                mBinding;
        VkDescriptorImageInfo   mImageInfo;
    };

    class ENGINE_API VulkanTexture2DArray : public VulkanTexture
    {
    public:
        VulkanTexture2DArray(VulkanDevicePtr device, uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t ArraySize, VkSampleCountFlagBits numSamples,
                VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
        : VulkanTexture(device, width, height, TEXTURE_2D_ARRAY, VulkanViewTypeTilingMode[TEXTURE_2D_ARRAY], VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, ArraySize, numSamples, format, usage, properties)
        {

        }
    };

    class ENGINE_API VulkanTexture3D : public VulkanTexture
    {
    public:
        VulkanTexture3D(VulkanDevicePtr device, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
                VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
        : VulkanTexture(device, width, height, TEXTURE_3D, VulkanViewTypeTilingMode[TEXTURE_3D], VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 1, numSamples, format, usage, properties)
        {

        }
    };

    class ENGINE_API VulkanTextureCube : public VulkanTexture
    {
    public:
        VulkanTextureCube(VulkanDevicePtr device, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
                VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
        : VulkanTexture(device, width, height, TEXTURE_CUBE, VulkanViewTypeTilingMode[TEXTURE_CUBE], VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 1, numSamples, format, usage, properties)
        {

        }
    };

    class ENGINE_API VulkanTextureCubeArray : public VulkanTexture
    {
    public:
        VulkanTextureCubeArray(VulkanDevicePtr device, uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t arraySize, VkSampleCountFlagBits numSamples,
                VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
        : VulkanTexture(device, width, height, TEXTURE_CUBE_ARRAY, VulkanViewTypeTilingMode[TEXTURE_CUBE_ARRAY], VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, arraySize, numSamples, format, usage, properties)
        {

        }
    };

    class ENGINE_API VulkanTextureDepth : public VulkanTexture
    {
    public:
        VulkanTextureDepth(VulkanDevicePtr device, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
                VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
        : VulkanTexture(device, width, height, TEXTURE_2D, VulkanViewTypeTilingMode[TEXTURE_2D], VK_IMAGE_ASPECT_DEPTH_BIT, mipLevels, 1, numSamples, format, usage, properties)
        {

        }
    };
}
#endif //HOMURA_VULKANTEXTURE_H
