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

    enum class TextureType
    {
        TEXTURE_1D,
        TEXTURE_2D,
        TEXTURE_2D_ARRAY,
        TEXTURE_3D,
        TEXTURE_CUBE
    };

    class VulkanTexture
    {
    public:
        VulkanTexture(std::shared_ptr<VulkanDevice> device, int width, int height, TextureType type, VkImageTiling tiling);
        ~VulkanTexture();

    private:
        std::shared_ptr<VulkanDevice>   mDevice;

        VkImage                         mImage;
        VkImageView                     mImageView;
        VkSampler                       mSampler;

        VkFormat                        mFormat;
        VkImageLayout                   mImageLayout;
    };
}
#endif //HOMURA_VULKANTEXTURE_H
