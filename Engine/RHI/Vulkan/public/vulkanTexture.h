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
        TEXTURE_1D_ARRAY,
        TEXTURE_2D,
        TEXTURE_2D_ARRAY,
        TEXTURE_3D,
        TEXTURE_CUBE,
        TEXTURE_CUBE_ARRAY
    };

    class VulkanTexture
    {
    public:
        VulkanTexture(VulkanDevicePtr device, uint32_t width, uint32_t height, TextureType type, VkImageTiling tiling,
                      uint32_t mipLevels, uint32_t layerCount, uint32_t numSamples, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
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

        VkSampler& getSampler()
        {
            return mSampler;
        }

        void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                         VkMemoryPropertyFlags properties);

        void createImageView(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

        void createSampler(int mipLevels);

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    private:
        VulkanDevicePtr                 mDevice;

        VkImage                         mImage;
        VkImageView                     mImageView;
        VkDeviceMemory                  mImageMemory;
        VkSampler                       mSampler;
        VkFormat                        mFormat;
        VkImageLayout                   mImageLayout;

        TextureType                     mType;

        uint32_t                        mWidth, mHeight;
        uint32_t                        mMipLevels, mLayerCount;
        uint32_t                        mNumSamples;
        VkDescriptorImageInfo           mDescriptor;

        VkMemoryPropertyFlags           mProperties;

    };

    class VulkanTexture1D : public VulkanTexture
    {

    };

    class VulkanTexture2D : public VulkanTexture
    {

    };

    class VulkanTexture2DArray : public VulkanTexture
    {

    };

    class VulkanTexture3D : public VulkanTexture
    {

    };

    class VulkanTextureCube : public VulkanTexture
    {

    };
}
#endif //HOMURA_VULKANTEXTURE_H
