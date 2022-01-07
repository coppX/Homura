//
// Created by FDC on 2022/1/7.
//

#include "vulkanSwapChain.h"
#include "vulkanDevice.h"
#include <debugUtils.h>
#include <algorithm>

namespace Homura
{
    VulkanSwapChain::VulkanSwapChain(VkInstance instance, std::shared_ptr<VulkanDevice> device, GLFWwindow* window, EPixelFormat& outPixelFormat, uint32_t width, uint32_t height,
                                    uint32_t* outDesiredNumBackBuffers, std::vector<VkImage>& outImages, int8_t lockToVsync)
        : mInstance{instance}
        , mSwapChain{VK_NULL_HANDLE}
        , mSurface{VK_NULL_HANDLE}
        , mColorFormat{VK_FORMAT_R8G8B8A8_UNORM}
        , mBackBufferCount{3}
        , mDevice{device}
        , mCurrentImageIndex{-1}
        , mSemaphoreIndex{0}
        , mNumPresentCalls{0}
        , mNumAcquireCalls{0}
        , mLockToVsync{lockToVsync}
        , mPresentID{0}
    {
        createSurface(window);
        mDevice->setPresentQueue(mSurface);

        uint32_t numFormats;
        VERIFYVULKANRESULT_EXPANDED(vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->getPhysicalHandle(), mSurface, &numFormats, nullptr));

        std::vector<VkSurfaceFormatKHR> formats(numFormats);
        VERIFYVULKANRESULT_EXPANDED(vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->getPhysicalHandle(), mSurface, &numFormats, formats.data()));

        VkSurfaceFormatKHR currFormat = {};
        if (outPixelFormat != PF_Unknown)
        {
            bool found = false;
            if (PixelFormats[outPixelFormat].supported)
            {
                VkFormat requested = (VkFormat)PixelFormats[outPixelFormat].platformFormat;
                for (int32_t index = 0; index < formats.size(); index++)
                {
                    if (formats[index].format == requested)
                    {
                        found = true;
                        currFormat = formats[index];
                        break;
                    }
                }

                if (!found)
                {
                    printf("Requested PixelFormat %d not supported by this swapchain! Falling back to supported swapchain format...", (int32_t)outPixelFormat);
                    outPixelFormat = PF_Unknown;
                }
            }
            else
            {
                printf("Requested PixelFormat %d not supported by this Vulkan implementation!", (int32_t)outPixelFormat);
                outPixelFormat = PF_Unknown;
            }
        }

        if (outPixelFormat == PF_Unknown)
        {
            for (int32_t index = 0; index < formats.size(); ++index)
            {
                for (int32_t pfIndex = 0; pfIndex < PF_MAX; ++pfIndex)
                {
                    if (formats[index].format == PixelFormats[pfIndex].platformFormat)
                    {
                        outPixelFormat = (EPixelFormat)pfIndex;
                        currFormat     = formats[index];
                        printf("No swapchain format requested, picking up VulkanFormat %d", (uint32_t)currFormat.format);
                        break;
                    }
                }

                if (outPixelFormat != PF_Unknown)
                {
                    break;
                }
            }
        }

        if (outPixelFormat == PF_Unknown)
        {
            printf("Can't find a proper pixel format for the swapchain, trying to pick up the first available");
            VkFormat platformFormat = PixelFormatToVkFormat(outPixelFormat, false);
            bool supported = false;
            for (int32_t index = 0; index < formats.size(); ++index)
            {
                if (formats[index].format == platformFormat)
                {
                    supported  = true;
                    currFormat = formats[index];
                }
            }
        }

        // 检测Present Model
        uint32_t numFoundPresentModes = 0;
        VERIFYVULKANRESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->getPhysicalHandle(), mSurface, &numFoundPresentModes, nullptr));

        std::vector<VkPresentModeKHR> foundPresentModes(numFoundPresentModes);
        VERIFYVULKANRESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->getPhysicalHandle(), mSurface, &numFoundPresentModes, foundPresentModes.data()));

        bool foundPresentModeMailbox    = false;
        bool foundPresentModeImmediate  = false;
        bool foundPresentModeFIFO       = false;

        printf("Found %d present mode.", (int32_t)numFoundPresentModes);
        for (int32_t index = 0; index < (int32_t)numFoundPresentModes; ++index)
        {
            switch (foundPresentModes[index])
            {
                case VK_PRESENT_MODE_MAILBOX_KHR:
                    foundPresentModeMailbox = true;
                    printf("- VK_PRESENT_MODE_MAILBOX_KHR (%d)", (int32_t)VK_PRESENT_MODE_MAILBOX_KHR);
                    break;
                case VK_PRESENT_MODE_IMMEDIATE_KHR:
                    foundPresentModeImmediate = true;
                    printf("- VK_PRESENT_MODE_IMMEDIATE_KHR (%d)", (int32_t)VK_PRESENT_MODE_IMMEDIATE_KHR);
                    break;
                case VK_PRESENT_MODE_FIFO_KHR:
                    foundPresentModeFIFO = true;
                    printf("- VK_PRESENT_MODE_FIFO_KHR (%d)", (int32_t)VK_PRESENT_MODE_FIFO_KHR);
                    break;
                default:
                    printf("- VkPresentModeKHR (%d)", (int32_t)foundPresentModes[index]);
                    break;
            }
        }

        VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
        if (foundPresentModeImmediate && !mLockToVsync)
        {
            presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
        else if (foundPresentModeMailbox)
        {
            presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
        }
        else if (foundPresentModeFIFO)
        {
            presentMode = VK_PRESENT_MODE_FIFO_KHR;
        }
        else
        {
            printf("Couldn't find desired PresentMode! Using %d", (int32_t)foundPresentModes[0]);
            presentMode = foundPresentModes[0];
        }

        printf("Selected VkPresentModeKHR mode %d", presentMode);

        VkSurfaceCapabilitiesKHR surfProperties;
        VERIFYVULKANRESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice->getPhysicalHandle(), mSurface, &surfProperties));

        printf("Surface minSize:%dx%d maxSize:%dx%d",
             (int32_t)surfProperties.minImageExtent.width, (int32_t)surfProperties.minImageExtent.height,
             (int32_t)surfProperties.maxImageExtent.width, (int32_t)surfProperties.maxImageExtent.height
        );

        VkSurfaceTransformFlagBitsKHR preTransform;
        if (surfProperties.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
        {
            preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        }
        else
        {
            preTransform = surfProperties.currentTransform;
        }

        VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
        if (surfProperties.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR)
        {
            compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        }

        uint32_t desiredNumBuffers = surfProperties.maxImageCount > 0 ? std::clamp(*outDesiredNumBackBuffers, surfProperties.minImageCount, surfProperties.maxImageCount) : *outDesiredNumBackBuffers;
        uint32_t sizeX = surfProperties.currentExtent.width == 0xFFFFFFFF ? width : surfProperties.currentExtent.width;
        uint32_t sizeY = surfProperties.currentExtent.height == 0xFFFFFFFF ? height : surfProperties.currentExtent.height;

        mSwapChainInfo = {};
        mSwapChainInfo.sType                = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        mSwapChainInfo.surface              = mSurface;
        mSwapChainInfo.minImageCount        = desiredNumBuffers;
        mSwapChainInfo.imageFormat          = currFormat.format;
        mSwapChainInfo.imageColorSpace      = currFormat.colorSpace;
        mSwapChainInfo.imageExtent.width    = sizeX;
        mSwapChainInfo.imageExtent.height   = sizeY;
        mSwapChainInfo.imageUsage           = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        mSwapChainInfo.preTransform         = preTransform;
        mSwapChainInfo.imageArrayLayers     = 1;
        mSwapChainInfo.imageSharingMode     = VK_SHARING_MODE_EXCLUSIVE;
        mSwapChainInfo.presentMode          = presentMode;
        mSwapChainInfo.oldSwapchain         = VK_NULL_HANDLE;
        mSwapChainInfo.clipped              = VK_TRUE;
        mSwapChainInfo.compositeAlpha       = compositeAlpha;

        if (mSwapChainInfo.imageExtent.width == 0)
        {
            mSwapChainInfo.imageExtent.width = width;
        }

        if (mSwapChainInfo.imageExtent.height == 0)
        {
            mSwapChainInfo.imageExtent.height = height;
        }

        // 检测是否支持present
        VkBool32 supportsPresent;
        VERIFYVULKANRESULT(vkGetPhysicalDeviceSurfaceSupportKHR(mDevice->getPhysicalHandle(), mDevice->getPresentQueue()->getFamilyIndex(), mSurface, &supportsPresent));
        if (!supportsPresent)
        {
            printf("Present queue not support.");
        }

        // 创建SwapChain
        VERIFYVULKANRESULT(vkCreateSwapchainKHR(mDevice->getHandle(), &mSwapChainInfo, nullptr, &mSwapChain));

        // 获取Backbuffer数量
        uint32_t numSwapChainImages;
        VERIFYVULKANRESULT(vkGetSwapchainImagesKHR(mDevice->getHandle(), mSwapChain, &numSwapChainImages, nullptr));

        outImages.resize(numSwapChainImages);
        VERIFYVULKANRESULT(vkGetSwapchainImagesKHR(mDevice->getHandle(), mSwapChain, &numSwapChainImages, outImages.data()));

        *outDesiredNumBackBuffers = numSwapChainImages;
        mBackBufferCount = numSwapChainImages;
        for (uint32_t index = 0; index < (uint32_t)numSwapChainImages; ++index)
        {
            VkSemaphoreCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            VERIFYVULKANRESULT(vkCreateSemaphore(mDevice->getHandle(), &createInfo, nullptr, &mImageAcquiredSemaphore[index]));
        }

        mPresentID = 0;
        mColorFormat = currFormat.format;
        printf("SwapChain: Backbuffer:%d Format:%d ColorSpace:%d Size:%dx%d Present:%d", mSwapChainInfo.minImageCount, mSwapChainInfo.imageFormat, mSwapChainInfo.imageColorSpace, mSwapChainInfo.imageExtent.width, mSwapChainInfo.imageExtent.height, mSwapChainInfo.presentMode);
    }

    VulkanSwapChain::~VulkanSwapChain()
    {
        VkDevice device = mDevice->getHandle();

        for (int32_t index = 0; index < mImageAcquiredSemaphore.size(); ++index)
        {
            vkDestroySemaphore(mDevice->getHandle(), mImageAcquiredSemaphore[index], nullptr);
        }

        vkDestroySwapchainKHR(device, mSwapChain, nullptr);
        vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
    }

    void VulkanSwapChain::createSurface(GLFWwindow *window)
    {
        VERIFYVULKANRESULT(glfwCreateWindowSurface(mInstance, window, nullptr, &mSurface));
    }

    int32_t VulkanSwapChain::acquireImageIndex(VkSemaphore *outSemaphore)
    {
        uint32_t imageIndex = 0;
        VkDevice device = mDevice->getHandle();
        const int32_t prev = mSemaphoreIndex;

        mSemaphoreIndex = (mSemaphoreIndex + 1) % mImageAcquiredSemaphore.size();
        VkResult result = vkAcquireNextImageKHR(device, mSwapChain, UINT64_MAX, mImageAcquiredSemaphore[mSemaphoreIndex], nullptr, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            mSemaphoreIndex = prev;
            return (int32_t)SwapStatus::OutOfData;
        }

        if (result == VK_ERROR_SURFACE_LOST_KHR)
        {
            mSemaphoreIndex = prev;
            return (int32_t)SwapStatus::SurfaceLost;
        }

        mNumAcquireCalls += 1;
        *outSemaphore = mImageAcquiredSemaphore[mSemaphoreIndex];
        mCurrentImageIndex = imageIndex;

        return mCurrentImageIndex;
    }

    VulkanSwapChain::SwapStatus VulkanSwapChain::present(std::shared_ptr<VulkanQueue> gfxQueue, std::shared_ptr<VulkanQueue> presentQueue, VkSemaphore* complete)
    {
        return SwapStatus::Healthy;
    }

}