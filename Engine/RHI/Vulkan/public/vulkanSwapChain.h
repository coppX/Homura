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
    struct SwapChainSupportInfo {
        VkSurfaceCapabilitiesKHR mCapabilities;
        std::vector<VkSurfaceFormatKHR> mFormats;
        std::vector<VkPresentModeKHR> mPresentModes;
    };

    class VulkanSwapChain
    {
    public:
        VulkanSwapChain(VulkanDevicePtr device, GLFWwindow* window, VkSurfaceKHR surface, VkCommandPool commandPool);
        ~VulkanSwapChain();


        void createSurface(GLFWwindow *window);

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

        VkFramebuffer& getFrameBuffer(const int index)
        {
            return mSwapChainFrameBuffers[index];
        }

        VkExtent2D& getExtent()
        {
            return mSwapChainExtent;
        }

        SwapChainSupportInfo querySwapChainSupportInfo();

        VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

        VkPresentModeKHR chooseSurfacePresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

        VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR &capabilities);

        void createFrameBuffers(const VulkanRenderPassPtr renderPass);

    private:
        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels = 1);

    private:
        VulkanDevicePtr                 mDevice;
        VkInstance                      mInstance;
        VkSurfaceKHR                    mSurface;
        GLFWwindow*                     mWindow;
        VkSwapchainKHR                  mSwapChain;

        VkFormat                        mSwapChainFormat;
        VkExtent2D                      mSwapChainExtent;
        uint32_t                        mImageCount;

        std::vector<VkImage>            mSwapChainImages;
        std::vector<VkImageView>        mSwapChainImageViews;
        std::vector<VkFramebuffer>      mSwapChainFrameBuffers;

        std::vector<VulkanTexturePtr>   mDepthImages;
        std::vector<VulkanTexturePtr>   mMultiSampleImages;
    };
}
#endif //HOMURA_VULKANSWAPCHAIN_H
