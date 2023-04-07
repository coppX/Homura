//
// Created by FDC on 2022/1/7.
//

#ifndef HOMURA_VULKANSWAPCHAIN_H
#define HOMURA_VULKANSWAPCHAIN_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <vector>

#include <vulkanRHI.h>
#include <pixelFormat.h>
#include <vulkanTypes.h>

namespace Homura
{
    struct ENGINE_API SwapChainSupportInfo {
        VkSurfaceCapabilitiesKHR mCapabilities;
        std::vector<VkSurfaceFormatKHR> mFormats;
        std::vector<VkPresentModeKHR> mPresentModes;
    };

    class ENGINE_API VulkanSwapChain
    {
    public:
        VulkanSwapChain(VulkanDevicePtr device, ApplicationWindowPtr window, VulkanSurfacePtr surface);
        ~VulkanSwapChain() = default;

        void create();
        void destroy();
        void destroyImageView();
        void destroySwapChain();

        VkSwapchainKHR getHandle()
        {
            return mSwapChain;
        }

        VkFormat& getFormat()
        {
            return mSwapChainFormat;
        }

        uint32_t getImageCount()
        {
            return mImageCount;
        }

        VkExtent2D& getExtent()
        {
            return mSwapChainExtent;
        }

        SwapChainSupportInfo querySwapChainSupportInfo();
        VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        VkPresentModeKHR chooseSurfacePresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR &capabilities);

        void createSwapChainImageViews();
        VkImageView getImageView(uint32_t index);
    private:
        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageViewType viewType, uint32_t layerCount, uint32_t mipLevels = 1);
    private:
        VulkanDevicePtr                 mDevice;
        VulkanSurfacePtr                mSurface;
        ApplicationWindowPtr            mWindow;
        VkSwapchainKHR                  mSwapChain;

        VkFormat                        mSwapChainFormat;
        VkExtent2D                      mSwapChainExtent;
        uint32_t                        mImageCount;

        std::vector<VkImage>            mSwapChainImages;
        std::vector<VkImageView>        mSwapChainImageViews;
    };
}
#endif //HOMURA_VULKANSWAPCHAIN_H
