//
// Created by 最上川 on 2022/1/10.
//

#ifndef HOMURA_VULKANTEXTURE_H
#define HOMURA_VULKANTEXTURE_H
#include <vulkan/vulkan.h>
#include <memory>

namespace Homura
{
    class VulkanDevice;
    class VulkanBuffer;

    enum TextureType
    {
        TEXTURE_1D = 0,
        TEXTURE_1D_ARRAy,
        TEXTURE_2D,
        TEXTURE_2D_ARRAY,
        TEXTURE_3D,
        TEXTURE_CUBE,
        TEXTURE_CUBE_ARRAY
    };

    class VulkanTexture
    {
    public:
        VulkanTexture(std::shared_ptr<VulkanDevice> device, uint32_t width, uint32_t height, TextureType type, VkImageTiling tiling,
                      uint32_t mipLevels, uint32_t layerCount, uint32_t numSamples, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
        ~VulkanTexture();

        void fromBuffer(std::shared_ptr<VulkanBuffer> buffer, uint32_t width, uint32_t height);

    private:
        void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                         VkMemoryPropertyFlags properties);

        void createImageView(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    private:
        std::shared_ptr<VulkanDevice>   mDevice;

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
