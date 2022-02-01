//
// Created by FDC on 2022/1/6.
//

#include <pixelFormat.h>
#include <vulkanDevice.h>

namespace Homura
{
    PixelFormatInfo PixelFormats[PF_MAX] =
            {
                    // Name					BlockSizeX	BlockSizeY	BlockSizeZ	BlockBytes	NumComponents	PlatformFormat	Supported		Format
                    {("unknown"),              0,  0,  0, 0,  0, 0, 0, PF_Unknown},
                    {("A32B32G32R32F"),        1,  1,  1, 16, 4, 0, 1, PF_A32B32G32R32F},
                    {("B8G8R8A8"),             1,  1,  1, 4,  4, 0, 1, PF_B8G8R8A8},
                    {("G8"),                   1,  1,  1, 1,  1, 0, 1, PF_G8},
                    {("G16"),                  1,  1,  1, 2,  1, 0, 1, PF_G16},
                    {("DXT1"),                 4,  4,  1, 8,  3, 0, 1, PF_DXT1},
                    {("DXT3"),                 4,  4,  1, 16, 4, 0, 1, PF_DXT3},
                    {("DXT5"),                 4,  4,  1, 16, 4, 0, 1, PF_DXT5},
                    {("UYVY"),                 2,  1,  1, 4,  4, 0, 0, PF_UYVY},
                    {("FloatRGB"),             1,  1,  1, 4,  3, 0, 1, PF_FloatRGB},
                    {("FloatRGBA"),            1,  1,  1, 8,  4, 0, 1, PF_FloatRGBA},
                    {("DepthStencil"),         1,  1,  1, 4,  1, 0, 0, PF_DepthStencil},
                    {("ShadowDepth"),          1,  1,  1, 4,  1, 0, 0, PF_ShadowDepth},
                    {("R32_FLOAT"),            1,  1,  1, 4,  1, 0, 1, PF_R32_FLOAT},
                    {("G16R16"),               1,  1,  1, 4,  2, 0, 1, PF_G16R16},
                    {("G16R16F"),              1,  1,  1, 4,  2, 0, 1, PF_G16R16F},
                    {("G16R16F_FILTER"),       1,  1,  1, 4,  2, 0, 1, PF_G16R16F_FILTER},
                    {("G32R32F"),              1,  1,  1, 8,  2, 0, 1, PF_G32R32F},
                    {("A2B10G10R10"),          1,  1,  1, 4,  4, 0, 1, PF_A2B10G10R10},
                    {("A16B16G16R16"),         1,  1,  1, 8,  4, 0, 1, PF_A16B16G16R16},
                    {("D24"),                  1,  1,  1, 4,  1, 0, 1, PF_D24},
                    {("PF_R16F"),              1,  1,  1, 2,  1, 0, 1, PF_R16F},
                    {("PF_R16F_FILTER"),       1,  1,  1, 2,  1, 0, 1, PF_R16F_FILTER},
                    {("BC5"),                  4,  4,  1, 16, 2, 0, 1, PF_BC5},
                    {("V8U8"),                 1,  1,  1, 2,  2, 0, 1, PF_V8U8},
                    {("A1"),                   1,  1,  1, 1,  1, 0, 0, PF_A1},
                    {("FloatR11G11B10"),       1,  1,  1, 4,  3, 0, 0, PF_FloatR11G11B10},
                    {("A8"),                   1,  1,  1, 1,  1, 0, 1, PF_A8},
                    {("R32_UINT"),             1,  1,  1, 4,  1, 0, 1, PF_R32_UINT},
                    {("R32_SINT"),             1,  1,  1, 4,  1, 0, 1, PF_R32_SINT},

                    // IOS Support
                    {("PVRTC2"),               8,  4,  1, 8,  4, 0, 0, PF_PVRTC2},
                    {("PVRTC4"),               4,  4,  1, 8,  4, 0, 0, PF_PVRTC4},

                    {("R16_UINT"),             1,  1,  1, 2,  1, 0, 1, PF_R16_UINT},
                    {("R16_SINT"),             1,  1,  1, 2,  1, 0, 1, PF_R16_SINT},
                    {("R16G16B16A16_UINT"),    1,  1,  1, 8,  4, 0, 1, PF_R16G16B16A16_UINT},
                    {("R16G16B16A16_SINT"),    1,  1,  1, 8,  4, 0, 1, PF_R16G16B16A16_SINT},
                    {("R5G6B5_UNORM"),         1,  1,  1, 2,  3, 0, 1, PF_R5G6B5_UNORM},
                    {("R8G8B8A8"),             1,  1,  1, 4,  4, 0, 1, PF_R8G8B8A8},
                    {("A8R8G8B8"),             1,  1,  1, 4,  4, 0, 1, PF_A8R8G8B8},
                    {("BC4"),                  4,  4,  1, 8,  1, 0, 1, PF_BC4},
                    {("R8G8"),                 1,  1,  1, 2,  2, 0, 1, PF_R8G8},

                    {("ATC_RGB"),              4,  4,  1, 8,  3, 0, 0, PF_ATC_RGB},
                    {("ATC_RGBA_E"),           4,  4,  1, 16, 4, 0, 0, PF_ATC_RGBA_E},
                    {("ATC_RGBA_I"),           4,  4,  1, 16, 4, 0, 0, PF_ATC_RGBA_I},
                    {("X24_G8"),               1,  1,  1, 1,  1, 0, 0, PF_X24_G8},
                    {("ETC1"),                 4,  4,  1, 8,  3, 0, 0, PF_ETC1},
                    {("ETC2_RGB"),             4,  4,  1, 8,  3, 0, 0, PF_ETC2_RGB},
                    {("ETC2_RGBA"),            4,  4,  1, 16, 4, 0, 0, PF_ETC2_RGBA},
                    {("PF_R32G32B32A32_UINT"), 1,  1,  1, 16, 4, 0, 1, PF_R32G32B32A32_UINT},
                    {("PF_R16G16_UINT"),       1,  1,  1, 4,  4, 0, 1, PF_R16G16_UINT},

                    // ASTC support
                    {("ASTC_4x4"),             4,  4,  1, 16, 4, 0, 0, PF_ASTC_4x4},
                    {("ASTC_6x6"),             6,  6,  1, 16, 4, 0, 0, PF_ASTC_6x6},
                    {("ASTC_8x8"),             8,  8,  1, 16, 4, 0, 0, PF_ASTC_8x8},
                    {("ASTC_10x10"),           10, 10, 1, 16, 4, 0, 0, PF_ASTC_10x10},
                    {("ASTC_12x12"),           12, 12, 1, 16, 4, 0, 0, PF_ASTC_12x12},

                    {("BC6H"),                 4,  4,  1, 16, 3, 0, 1, PF_BC6H},
                    {("BC7"),                  4,  4,  1, 16, 4, 0, 1, PF_BC7},
                    {("R8_UINT"),              1,  1,  1, 1,  1, 0, 1, PF_R8_UINT},
                    {("L8"),                   1,  1,  1, 1,  1, 0, 0, PF_L8},
                    {("XGXR8"),                1,  1,  1, 4,  4, 0, 1, PF_XGXR8},
                    {("R8G8B8A8_UINT"),        1,  1,  1, 4,  4, 0, 1, PF_R8G8B8A8_UINT},
                    {("R8G8B8A8_SNORM"),       1,  1,  1, 4,  4, 0, 1, PF_R8G8B8A8_SNORM},

                    {("R16G16B16A16_UINT"),    1,  1,  1, 8,  4, 0, 1, PF_R16G16B16A16_UNORM},
                    {("R16G16B16A16_SINT"),    1,  1,  1, 8,  4, 0, 1, PF_R16G16B16A16_SNORM},
                    {("PLATFORM_HDR_0"),       0,  0,  0, 0,  0, 0, 0, PF_PLATFORM_HDR_0},
                    {("PLATFORM_HDR_1"),       0,  0,  0, 0,  0, 0, 0, PF_PLATFORM_HDR_1},
                    {("PLATFORM_HDR_2"),       0,  0,  0, 0,  0, 0, 0, PF_PLATFORM_HDR_2},
            };

    VkFormat findSupportedFormat(VulkanDevicePtr device, const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
    {
        for (VkFormat format : candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(device->getPhysicalHandle(), format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL & (props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }
        throw std::runtime_error("failed to find supported format!");
    }

    VkFormat findDepthFormat(VulkanDevicePtr device)
    {
        return findSupportedFormat(device, {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                                   VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }
}