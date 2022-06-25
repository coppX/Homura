//
// Created by FDC on 2022/1/6.
//

#ifndef HOMURA_PIXELFORMAT_H
#define HOMURA_PIXELFORMAT_H

#include <cstdint>
#include <vulkan/vulkan.h>
#include <vulkanTypes.h>
#include <vector>

namespace Homura
{
    enum EPixelFormat
    {
        PF_Unknown                  = 0,
        PF_A32B32G32R32F            = 1,
        PF_B8G8R8A8                 = 2,
        PF_G8                       = 3,
        PF_G16                      = 4,
        PF_DXT1                     = 5,
        PF_DXT3                     = 6,
        PF_DXT5                     = 7,
        PF_UYVY                     = 8,
        PF_FloatRGB                 = 9,
        PF_FloatRGBA                = 10,
        PF_DepthStencil             = 11,
        PF_ShadowDepth              = 12,
        PF_R32_FLOAT                = 13,
        PF_G16R16                   = 14,
        PF_G16R16F                  = 15,
        PF_G16R16F_FILTER           = 16,
        PF_G32R32F                  = 17,
        PF_A2B10G10R10              = 18,
        PF_A16B16G16R16             = 19,
        PF_D24                      = 20,
        PF_R16F                     = 21,
        PF_R16F_FILTER              = 22,
        PF_BC5                      = 23,
        PF_V8U8                     = 24,
        PF_A1                       = 25,
        PF_FloatR11G11B10           = 26,
        PF_A8                       = 27,
        PF_R32_UINT                 = 28,
        PF_R32_SINT                 = 29,
        PF_PVRTC2                   = 30,
        PF_PVRTC4                   = 31,
        PF_R16_UINT                 = 32,
        PF_R16_SINT                 = 33,
        PF_R16G16B16A16_UINT        = 34,
        PF_R16G16B16A16_SINT        = 35,
        PF_R5G6B5_UNORM             = 36,
        PF_R8G8B8A8                 = 37,
        PF_A8R8G8B8                 = 38,
        PF_BC4                      = 39,
        PF_R8G8                     = 40,
        PF_ATC_RGB                  = 41,
        PF_ATC_RGBA_E               = 42,
        PF_ATC_RGBA_I               = 43,
        PF_X24_G8                   = 44,
        PF_ETC1                     = 45,
        PF_ETC2_RGB                 = 46,
        PF_ETC2_RGBA                = 47,
        PF_R32G32B32A32_UINT        = 48,
        PF_R16G16_UINT              = 49,
        PF_ASTC_4x4                 = 50,
        PF_ASTC_6x6                 = 51,
        PF_ASTC_8x8                 = 52,
        PF_ASTC_10x10               = 53,
        PF_ASTC_12x12               = 54,
        PF_BC6H                     = 55,
        PF_BC7                      = 56,
        PF_R8_UINT                  = 57,
        PF_L8                       = 58,
        PF_XGXR8                    = 59,
        PF_R8G8B8A8_UINT            = 60,
        PF_R8G8B8A8_SNORM           = 61,
        PF_R16G16B16A16_UNORM       = 62,
        PF_R16G16B16A16_SNORM       = 63,
        PF_PLATFORM_HDR_0           = 64,
        PF_PLATFORM_HDR_1           = 65,
        PF_PLATFORM_HDR_2           = 66,
        PF_MAX = PF_PLATFORM_HDR_2 + 1,
    };

    struct PixelFormatInfo
    {
        const char* name;
        int32_t blockSizeX,
                blockSizeY,
                blockSizeZ,
                blockBytes,
                numComponents;
        uint32_t platformFormat;
        bool supported;
        EPixelFormat pixelFormat;

    };

    extern PixelFormatInfo PixelFormats[PF_MAX];

    inline VkFormat PixelFormatToVkFormat(EPixelFormat format, const bool bIsSRGB)
    {
        VkFormat result = (VkFormat)PixelFormats[format].pixelFormat;
        if (bIsSRGB)
        {
            switch (result)
            {
                case VK_FORMAT_B8G8R8A8_UNORM:				result = VK_FORMAT_B8G8R8A8_SRGB; break;
                case VK_FORMAT_A8B8G8R8_UNORM_PACK32:		result = VK_FORMAT_A8B8G8R8_SRGB_PACK32; break;
                case VK_FORMAT_R8_UNORM:					result = VK_FORMAT_R8_SRGB; break;
                case VK_FORMAT_R8G8_UNORM:					result = VK_FORMAT_R8G8_SRGB; break;
                case VK_FORMAT_R8G8B8_UNORM:				result = VK_FORMAT_R8G8B8_SRGB; break;
                case VK_FORMAT_R8G8B8A8_UNORM:				result = VK_FORMAT_R8G8B8A8_SRGB; break;
                case VK_FORMAT_BC1_RGB_UNORM_BLOCK:			result = VK_FORMAT_BC1_RGB_SRGB_BLOCK; break;
                case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:		result = VK_FORMAT_BC1_RGBA_SRGB_BLOCK; break;
                case VK_FORMAT_BC2_UNORM_BLOCK:				result = VK_FORMAT_BC2_SRGB_BLOCK; break;
                case VK_FORMAT_BC3_UNORM_BLOCK:				result = VK_FORMAT_BC3_SRGB_BLOCK; break;
                case VK_FORMAT_BC7_UNORM_BLOCK:				result = VK_FORMAT_BC7_SRGB_BLOCK; break;
                case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:		result = VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK; break;
                case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:	result = VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK; break;
                case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:	result = VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK; break;
                case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:		result = VK_FORMAT_ASTC_4x4_SRGB_BLOCK; break;
                case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:		result = VK_FORMAT_ASTC_5x4_SRGB_BLOCK; break;
                case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:		result = VK_FORMAT_ASTC_5x5_SRGB_BLOCK; break;
                case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:		result = VK_FORMAT_ASTC_6x5_SRGB_BLOCK; break;
                case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:		result = VK_FORMAT_ASTC_6x6_SRGB_BLOCK; break;
                case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:		result = VK_FORMAT_ASTC_8x5_SRGB_BLOCK; break;
                case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:		result = VK_FORMAT_ASTC_8x6_SRGB_BLOCK; break;
                case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:		result = VK_FORMAT_ASTC_8x8_SRGB_BLOCK; break;
                case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:		result = VK_FORMAT_ASTC_10x5_SRGB_BLOCK; break;
                case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:		result = VK_FORMAT_ASTC_10x6_SRGB_BLOCK; break;
                case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:		result = VK_FORMAT_ASTC_10x8_SRGB_BLOCK; break;
                case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:		result = VK_FORMAT_ASTC_10x10_SRGB_BLOCK; break;
                case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:		result = VK_FORMAT_ASTC_12x10_SRGB_BLOCK; break;
                case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:		result = VK_FORMAT_ASTC_12x12_SRGB_BLOCK; break;
                case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG:	result = VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG; break;
                case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG:	result = VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG; break;
                case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG:	result = VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG; break;
                case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG:	result = VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG; break;
                default:	break;
            }
        }
        return result;
    }


    VkFormat findSupportedFormat(VulkanDevicePtr device, const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    VkFormat findDepthFormat(VulkanDevicePtr device);
}

#endif //HOMURA_PIXELFORMAT_H
