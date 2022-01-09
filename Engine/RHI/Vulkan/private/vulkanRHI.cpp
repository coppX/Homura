//
// Created by FDC on 2022/1/6.
//

#include <vulkanRHI.h>
#include <debugUtils.h>
#include <GLFW/glfw3.h>
#include <vulkanDevice.h>
#include <vulkanSwapChain.h>

namespace Homura
{
    VulkanRHI::VulkanRHI()
        : mInstance{VK_NULL_HANDLE}
        , mDevice{nullptr}
        , mSwapChain{nullptr}
        , mValidationLayers{"VK_LAYER_KHRONOS_validation"}
        , mDeviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME}
        , mPixelFormat{PF_R8G8B8A8}
        , mWidth(960)
        , mHeight(540)
    {

    }

    VulkanRHI::~VulkanRHI()
    {
        destroySwapChain();
        destroyDevice();
        destroyInstance();
    }

    void VulkanRHI::createInstance()
    {
        if (enableValidationLayers && !checkValidationLayerSupport())
        {
            throw std::runtime_error("validation layers requested, but not available!");
        }
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "madoka";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();
        extensions.push_back("VK_KHR_get_physical_device_properties2");
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
            createInfo.ppEnabledLayerNames = mValidationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
        }
        else
        {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        VERIFYVULKANRESULT(vkCreateInstance(&createInfo, nullptr, &mInstance));
    }

    void VulkanRHI::selectAndInitDevice()
    {
        uint32_t gpuCount = 0;
        VkResult result = vkEnumeratePhysicalDevices(getInstance(), &gpuCount, nullptr);
        if (result == VK_ERROR_INITIALIZATION_FAILED) {
            printf("%s\n", "Cannot find a compatible Vulkan device or driver. Try updating your video driver to a more recent version and make sure your video card supports Vulkan.");
            return;
        }

        if (gpuCount == 0) {
            printf("%s\n", "Couldn't enumerate physical devices! Make sure your drivers are up to date and that you are not pending a reboot.");
            return;
        }

        std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
        VERIFYVULKANRESULT_EXPANDED(vkEnumeratePhysicalDevices(getInstance(), &gpuCount, physicalDevices.data()));

        struct DeviceInfo
        {
            std::shared_ptr<VulkanDevice> device;
            uint32_t deviceIndex;
        };

        std::vector<DeviceInfo> discreteDevices;
        std::vector<DeviceInfo> integratedDevices;

        for (uint32_t index = 0; index < gpuCount; index++)
        {
            std::shared_ptr<VulkanDevice> newDevice = std::make_shared<VulkanDevice>(physicalDevices[index]);
            bool isDiscrete = newDevice->queryDevice(index);
            if (isDiscrete)
            {
                discreteDevices.push_back({newDevice, index});
            }
            else
            {
                integratedDevices.push_back({newDevice, index});
            }
        }

        for (uint32_t index = 0; index < integratedDevices.size(); index++)
        {
            discreteDevices.push_back(integratedDevices[index]);
        }

        int32_t deviceIndex = -1;
        if (discreteDevices.size() > 0)
        {
            int32_t preferredVendor = -1;
            if (discreteDevices.size() > 1 && preferredVendor != -1)
            {
                for (int32_t index = 0; index < discreteDevices.size(); ++index)
                {
                    if (discreteDevices[index].device->getDeviceProperties().vendorID == preferredVendor)
                    {
                        mDevice = discreteDevices[index].device;
                        deviceIndex = discreteDevices[index].deviceIndex;
                        break;
                    }
                }
            }

            if (deviceIndex == -1)
            {
                mDevice = discreteDevices[0].device;
                deviceIndex = discreteDevices[0].deviceIndex;
            }
        }
        else
        {
            printf("%s", "No devices found!");
            deviceIndex = -1;
            return;
        }

        mDevice->setPhysicalDeviceFeatures(mPhysicalDeviceFeatures2);
        mDevice->initGPU(deviceIndex);
    }

    void VulkanRHI::createSwapChain(GLFWwindow *window)
    {
        destroySwapChain();

        uint32_t desiredNumBackBuffers = 3;
        mSwapChain = std::make_shared<VulkanSwapChain>(mInstance, mDevice, window, mPixelFormat, mWidth, mHeight, &desiredNumBackBuffers, mBackBufferImages, 1);

        mBackBufferViews.resize(mBackBufferImages.size());
        for (int32_t i = 0; i < mBackBufferViews.size(); ++i)
        {
            VkImageViewCreateInfo imageViewCreateInfo{};
            imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewCreateInfo.format = VK_FORMAT_B8G8R8A8_SRGB;//PixelFormatToVkFormat(mPixelFormat, false);
            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageViewCreateInfo.image = mBackBufferImages[i];
            imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
            imageViewCreateInfo.subresourceRange.levelCount = 1;
            imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            imageViewCreateInfo.subresourceRange.layerCount = 1;
            VERIFYVULKANRESULT(vkCreateImageView(mDevice->getHandle(), &imageViewCreateInfo, nullptr, &(mBackBufferViews[i])));
        }
    }

    void VulkanRHI::destroyInstance()
    {
        vkDestroyInstance(mInstance, nullptr);
    }

    void VulkanRHI::destroyDevice()
    {
        mDevice->destroyDevice();
        mDevice = nullptr;
    }

    void VulkanRHI::destroySwapChain()
    {
        if (mSwapChain)
        {
            for (int32_t i = 0; i < mBackBufferViews.size(); ++i)
            {
                vkDestroyImageView(mDevice->getHandle(), mBackBufferViews[i], nullptr);
            }
            vkDestroySwapchainKHR(mDevice->getHandle(), mSwapChain->getHandle(), nullptr);
            mSwapChain = nullptr;
        }
    }

    bool VulkanRHI::checkValidationLayerSupport()
    {
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : mValidationLayers)
        {

            bool layerFound = false;
            for (const auto &layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }
        return true;
    }

    std::vector<const char*> VulkanRHI::getRequiredExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        return extensions;
    }

    void VulkanRHI::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
    {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                     | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                     | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                                 | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                                 | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }



    void VulkanRHI::createGraphicsPipeline()
    {}


    void VulkanRHI::createCommandPool()
    {

    }

    void VulkanRHI::createColorResources()
    {

    }

    void VulkanRHI::createDepthResources()
    {

    }

    void VulkanRHI::createFrameBuffers()
    {

    }

    void VulkanRHI::createVertexBuffer()
    {

    }

    void VulkanRHI::createIndexBuffer()
    {

    }

    void VulkanRHI::createUniformBuffer()
    {

    }

    void VulkanRHI::createCommandBuffers()
    {

    }

    void VulkanRHI::createSyncObjects()
    {

    }

    void VulkanRHI::createSampler()
    {

    }

    void VulkanRHI::createRenderPass()
    {

    }

    void VulkanRHI::createDescriptorPool()
    {

    }
}