//
// Created by FDC on 2022/1/7.
//

#include <vulkanSwapChain.h>
#include <vulkanDevice.h>
#include <vulkanQueue.h>
#include <debugUtils.h>
#include <vulkanTexture.h>
#include <vulkanSurface.h>
#include <algorithm>
#include <array>
#include <vulkanFramebuffer.h>
#include <applicationWindow.h>

namespace Homura
{
    VulkanSwapChain::VulkanSwapChain(VulkanDevicePtr device, ApplicationWindowPtr window, VulkanSurfacePtr surface)
        : mDevice{device}
        , mSurface{surface}
        , mWindow{window}
    {
        create();
    }

    VulkanSwapChain::~VulkanSwapChain()
    {

    }

    void VulkanSwapChain::create()
    {
        auto swapChainSupportInfo = querySwapChainSupportInfo();
        VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(swapChainSupportInfo.mFormats);
        VkPresentModeKHR presentMode = chooseSurfacePresentMode(swapChainSupportInfo.mPresentModes);
        VkExtent2D extent = chooseExtent(swapChainSupportInfo.mCapabilities);
        mImageCount = swapChainSupportInfo.mCapabilities.minImageCount + 1;

        if (swapChainSupportInfo.mCapabilities.maxImageCount > 0 && mImageCount > swapChainSupportInfo.mCapabilities.maxImageCount)
        {
            mImageCount = swapChainSupportInfo.mCapabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = mSurface->getHandle();
        createInfo.minImageCount = mImageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

        std::vector<uint32_t> queueFamilies = { mDevice->getGraphicsQueue()->getFamilyIndex(), mDevice->getPresentQueue()->getFamilyIndex() };

        if (mDevice->getGraphicsQueue()->getFamilyIndex() != mDevice->getPresentQueue()->getFamilyIndex())
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilies.data();
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform             = swapChainSupportInfo.mCapabilities.currentTransform;
        createInfo.compositeAlpha           = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode              = presentMode;
        createInfo.clipped                  = VK_TRUE;
        createInfo.oldSwapchain             = VK_NULL_HANDLE;

        VERIFYVULKANRESULT(vkCreateSwapchainKHR(mDevice->getHandle(), &createInfo, nullptr, &mSwapChain));
        mSwapChainFormat = surfaceFormat.format;
        mSwapChainExtent = extent;

        vkGetSwapchainImagesKHR(mDevice->getHandle(), mSwapChain, &mImageCount, nullptr);
        mSwapChainImages.resize(mImageCount);
        vkGetSwapchainImagesKHR(mDevice->getHandle(), mSwapChain, &mImageCount, mSwapChainImages.data());

        createSwapChainImageViews();
    }

    void VulkanSwapChain::destroy()
    {
        destroyImageView();
        destroySwapChain();
    }

    void VulkanSwapChain::destroyImageView()
    {
        for (auto &imageView : mSwapChainImageViews)
        {
            vkDestroyImageView(mDevice->getHandle(), imageView, nullptr);
        }
        mSwapChainImageViews.clear();
    }

    void VulkanSwapChain::destroySwapChain()
    {
        if (mSwapChain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(mDevice->getHandle(), mSwapChain, nullptr);
            mSwapChain = VK_NULL_HANDLE;
        }
    }

    void VulkanSwapChain::createSwapChainImageViews()
    {
        mSwapChainImageViews.resize(mImageCount);
        for (uint32_t i = 0; i < mImageCount; ++i)
        {
            mSwapChainImageViews[i] = createImageView(mSwapChainImages[i], mSwapChainFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);
        }
    }

    VkImageView VulkanSwapChain::getImageView(uint32_t index)
    {
        assert(index < mImageCount);
        return mSwapChainImageViews[index];
    }

    SwapChainSupportInfo VulkanSwapChain::querySwapChainSupportInfo()
    {
        SwapChainSupportInfo info;
        VERIFYVULKANRESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice->getPhysicalHandle(), mSurface->getHandle(), &info.mCapabilities));

        uint32_t formatCount = 0;
        VERIFYVULKANRESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->getPhysicalHandle(), mSurface->getHandle(), &formatCount, nullptr));
        if (formatCount != 0)
        {
            info.mFormats.resize(formatCount);
            VERIFYVULKANRESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->getPhysicalHandle(), mSurface->getHandle(), &formatCount, info.mFormats.data()));
        }

        uint32_t presentModeCount = 0;
        VERIFYVULKANRESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->getPhysicalHandle(), mSurface->getHandle(), &presentModeCount, nullptr));
        if (presentModeCount != 0)
        {
            info.mPresentModes.resize(presentModeCount);
            VERIFYVULKANRESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->getPhysicalHandle(), mSurface->getHandle(), &presentModeCount, info.mPresentModes.data()));
        }

        return info;
    }

    VkSurfaceFormatKHR VulkanSwapChain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
    {
        if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
        {
            return {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
        }

        for (const auto& availableFormat : availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR VulkanSwapChain::chooseSurfacePresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
    {
        for (const auto& availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return availablePresentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanSwapChain::chooseExtent(const VkSurfaceCapabilitiesKHR &capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }

        int width = mWindow->getWidth();
        int height = mWindow->getHeight();

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width  = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }

    VkImageView VulkanSwapChain::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageViewType viewType, uint32_t layerCount, uint32_t mipLevels)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType                              = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image                              = image;
        viewInfo.viewType                           = viewType;
        viewInfo.format                             = format;

        viewInfo.subresourceRange.aspectMask        = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel      = 0;
        viewInfo.subresourceRange.levelCount        = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer    = 0;
        viewInfo.subresourceRange.layerCount        = layerCount;

        VkImageView imageView{};
        VERIFYVULKANRESULT(vkCreateImageView(mDevice->getHandle(), &viewInfo, nullptr, &imageView));

        return imageView;
    }

    void VulkanSwapChain::recreateSwapChain()
    {

    }
}