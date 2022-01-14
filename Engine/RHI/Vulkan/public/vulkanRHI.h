//
// Created by FDC on 2022/1/6.
//

#ifndef HOMURA_VULKANRHI_H
#define HOMURA_VULKANRHI_H

#include <vulkan/vulkan.h>
#include <vulkanTypes.h>
#include <pixelFormat.h>
#include <GLFW/glfw3.h>

#include <vector>

namespace Homura
{
    class VulkanRHI
    {
    public:
        VulkanRHI();

        virtual ~VulkanRHI();

        void init();

        const VkInstance& getInstance() const
        {
            return mInstance;
        }

        const VulkanDevicePtr getDevice() const
        {
            return mDevice;
        }

        const VulkanSwapChainPtr getSwapChain() const
        {
            return mSwapChain;
        }

        void createInstance();
        void selectAndInitDevice();
        void createSwapChain(GLFWwindow *window);

        void destroyInstance();
        void destroyDevice();
        void destroySwapChain();

        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

        void createGraphicsPipeline();

        void createCommandPool();

        void createColorResources();

        void createDepthResources();

        void createFrameBuffers();

        void createVertexBuffer();

        void createIndexBuffer();

        void createUniformBuffer();

        void createCommandBuffers();

        void createSyncObjects();

        void createSampler();

        void createRenderPass();

        void createDescriptorPool();

    private:
        bool checkValidationLayerSupport();
        std::vector<const char*> getRequiredExtensions();

    private:
        VkInstance                          mInstance;
        std::vector<const char*>            mValidationLayers;
        std::vector<const char*>            mDeviceExtensions;
        VkPhysicalDeviceFeatures2*          mPhysicalDeviceFeatures2 = nullptr;

        VulkanDevicePtr                     mDevice;

        VulkanSwapChainPtr                  mSwapChain;
        EPixelFormat                        mPixelFormat;
        std::vector<VkImage>                mBackBufferImages;
        std::vector<VkImageView>            mBackBufferViews;

        uint32_t                            mWidth;
        uint32_t                            mHeight;
    };

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
}
#endif //HOMURA_VULKANRHI_H
