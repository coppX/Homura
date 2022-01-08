//
// Created by FDC on 2022/1/7.
//

#ifndef HOMURA_VULKANSWAPCHAIN_H
#define HOMURA_VULKANSWAPCHAIN_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <vector>

#include <vulkanRHI.h>
#include <pixelFormat.h>

namespace Homura
{
    class VulkanDevice;
    class VulkanQueue;

    class VulkanSwapChain
    {
    public:
        enum class SwapStatus
        {
            Healthy     = 0,
            OutOfData   = -1,
            SurfaceLost = -2,
        };

        VulkanSwapChain(VkInstance instance, std::shared_ptr<VulkanDevice> device, GLFWwindow* window, EPixelFormat& outPixelFormat, uint32_t width, uint32_t height,
                        uint32_t* outDesiredNumBackBuffers, std::vector<VkImage>& outImages, int8_t lockToVsync);
        ~VulkanSwapChain();

        SwapStatus present(std::shared_ptr<VulkanQueue> gfxQueue, std::shared_ptr<VulkanQueue> presentQueue, VkSemaphore* complete);

        void createSurface(GLFWwindow *window);

        int32_t getBackBufferCount() const
        {
            return mBackBufferCount;
        }

        VkSwapchainKHR getHandle()
        {
            return mSwapChain;
        }

        int32_t getWidth() const
        {
            return mSwapChainInfo.imageExtent.width;
        }

        int32_t getHeight() const
        {
            return mSwapChainInfo.imageExtent.height;
        }

        int8_t doesLockToVsync()
        {
            return mLockToVsync;
        }

        const VkSwapchainCreateInfoKHR& getInfo() const
        {
            return mSwapChainInfo;
        }

        VkFormat getColorFormat() const
        {
            return mColorFormat;
        }

        int32_t acquireImageIndex(VkSemaphore *outSemaphore);
    private:
        VkInstance                      mInstance;
        VkSwapchainKHR                  mSwapChain;
        VkSwapchainCreateInfoKHR        mSwapChainInfo;
        VkSurfaceKHR                    mSurface;
        VkFormat                        mColorFormat;
        int32_t                         mBackBufferCount;

        std::shared_ptr<VulkanDevice>   mDevice;
        std::vector<VkSemaphore>        mImageAcquiredSemaphore;

        int32_t                         mCurrentImageIndex;
        int32_t                         mSemaphoreIndex;
        int32_t                         mNumPresentCalls;
        int32_t                         mNumAcquireCalls;
        int8_t                          mLockToVsync;
        uint32_t                        mPresentID;
    };
}
#endif //HOMURA_VULKANSWAPCHAIN_H
