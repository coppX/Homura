//
// Created by FDC on 2022/1/6.
//

#include <vulkanDevice.h>
#include <vulkanQueue.h>
#include <string>
#include <debugUtils.h>

namespace Homura
{
    VulkanDevice::VulkanDevice(VkPhysicalDevice physicalDevice)
        : mDevice{VK_NULL_HANDLE}
        , mPhysicalDevice{physicalDevice}
        , mGfxQueue{nullptr}
        , mComputeQueue{nullptr}
        , mTransferQueue{nullptr}
        , mPresentQueue{nullptr}
    {

    }

    VulkanDevice::~VulkanDevice()
    {
        if (mDevice != VK_NULL_HANDLE)
        {
            destroyDevice();
        }
    }

    bool VulkanDevice::queryDevice(uint32_t index)
    {
        bool discrete = false;
        vkGetPhysicalDeviceProperties(mPhysicalDevice, &mPhysicalDeviceProperties);

        auto GetDeviceTypeString = [&]() -> std::string
        {
            std::string info;
            switch (mPhysicalDeviceProperties.deviceType)
            {
                case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                    info = "Other";
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    info = "Discrete GPU";
                    discrete = true;
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    info = "Integrated GPU";
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_CPU:
                    info = "CPU";
                    break;
                default:
                    info = "Unknown";
                    break;
            }
            return info;
        };
        printf("DeviceID 0x%x Type %s", mPhysicalDeviceProperties.deviceID, GetDeviceTypeString().c_str());

        uint32_t queueCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueCount, nullptr);
        mQueueFamilyProperties.resize(queueCount);
        vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueCount, mQueueFamilyProperties.data());

        return discrete;
    }

    void VulkanDevice::initGPU(uint32_t index)
    {
        //vkGetPhysicalDeviceFeatures(mPhysicalDevice, &mPhysicalDeviceFeatures);
        createDevice();
    }

    void VulkanDevice::createDevice()
    {
        std::vector<const char*> deviceExtensions = {"VK_KHR_portability_subset"};
        std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

        //getDeviceExtensionsAndLayers(deviceExtensions, validationLayers);

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        // device extensions
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        // device validationLayers
        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }
        // device features
        mPhysicalDeviceFeatures = {};
        mPhysicalDeviceFeatures.samplerAnisotropy = VK_TRUE;
        mPhysicalDeviceFeatures.sampleRateShading = VK_TRUE;

        if (mPhysicalDeviceFeatures2)
        {
            createInfo.pNext = mPhysicalDeviceFeatures2;
            createInfo.pEnabledFeatures = nullptr;
            mPhysicalDeviceFeatures2->features = mPhysicalDeviceFeatures;
        }
        else
        {
            createInfo.pEnabledFeatures = &mPhysicalDeviceFeatures;
        }
        std::vector<VkDeviceQueueCreateInfo> queueFamilyInfos;

        // queue
        uint32_t numProperties              = 0;
        int32_t gfxQueueFamilyIndex         = -1;
        int32_t computeQueueFamilyIndex     = -1;
        int32_t transferQueueFamilyIndex    = -1;

        for (int32_t familyIndex = 0; familyIndex < mQueueFamilyProperties.size(); familyIndex++)
        {
            const VkQueueFamilyProperties& currProps = mQueueFamilyProperties[familyIndex];
            bool isValidQueue = false;
            // graphics queue
            if ((currProps.queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT)
            {
                if (gfxQueueFamilyIndex == -1)
                {
                    gfxQueueFamilyIndex = familyIndex;
                    isValidQueue = true;
                }
            }
            // compute queue
            if ((currProps.queueFlags & VK_QUEUE_COMPUTE_BIT) == VK_QUEUE_COMPUTE_BIT)
            {
                if (computeQueueFamilyIndex == -1)
                {
                    computeQueueFamilyIndex = familyIndex;
                    isValidQueue = true;
                }
            }
            // transfer queue
            if ((currProps.queueFlags & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT)
            {
                if (transferQueueFamilyIndex == -1)
                {
                    transferQueueFamilyIndex = familyIndex;
                    isValidQueue = true;
                }
            }

            auto GetQueueInfoString = [](const VkQueueFamilyProperties& Props) -> std::string
            {
                std::string info;
                if ((Props.queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT) {
                    info += " Gfx";
                }
                if ((Props.queueFlags & VK_QUEUE_COMPUTE_BIT) == VK_QUEUE_COMPUTE_BIT) {
                    info += " Compute";
                }
                if ((Props.queueFlags & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT) {
                    info += " Xfer";
                }
                if ((Props.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) == VK_QUEUE_SPARSE_BINDING_BIT) {
                    info += " Sparse";
                }
                return info;
            };

            if (!isValidQueue)
            {
                printf("Skipping unnecessary Queue Family %d: %d queues%s\n", familyIndex, currProps.queueCount, GetQueueInfoString(currProps).c_str());
                continue;
            }

            VkDeviceQueueCreateInfo currQueue{};
            currQueue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            currQueue.queueFamilyIndex  = familyIndex;
            currQueue.queueCount        = currProps.queueCount;
            numProperties              += currProps.queueCount;
            queueFamilyInfos.push_back(currQueue);
        }

        std::vector<float> queuePriorities(numProperties);
        float* currentPriority = queuePriorities.data();
        for (int32_t index = 0; index < queueFamilyInfos.size(); index++)
        {
            VkDeviceQueueCreateInfo &currQueue = queueFamilyInfos[index];
            currQueue.pQueuePriorities = currentPriority;
            const VkQueueFamilyProperties &currProps = mQueueFamilyProperties[currQueue.queueFamilyIndex];
            for (int32_t queueIndex = 0; queueIndex < (int32_t)currProps.queueCount; queueIndex++)
            {
                *currentPriority++ = 1.0f;
            }
        }

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueFamilyInfos.size());
        createInfo.pQueueCreateInfos = queueFamilyInfos.data();

        VERIFYVULKANRESULT_EXPANDED(vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice));

        mGfxQueue = std::make_shared<VulkanQueue>(this, gfxQueueFamilyIndex);

        if (computeQueueFamilyIndex == -1)
        {
            computeQueueFamilyIndex = gfxQueueFamilyIndex;
        }

        mComputeQueue = std::make_shared<VulkanQueue>(this, computeQueueFamilyIndex);

        if (transferQueueFamilyIndex == -1)
        {
            transferQueueFamilyIndex = gfxQueueFamilyIndex;
        }

        mTransferQueue = std::make_shared<VulkanQueue>(this, transferQueueFamilyIndex);
    }

    void VulkanDevice::destroyDevice()
    {
        if (mDevice != VK_NULL_HANDLE)
        {
            vkDestroyDevice(mDevice, nullptr);
            mDevice = VK_NULL_HANDLE;
        }
    }

    void VulkanDevice::setPresentQueue(VkSurfaceKHR surface)
    {
        if (mPresentQueue)
        {
            return;
        }

        const auto supportsPresent = [surface](VkPhysicalDevice physicalDevice, VulkanQueuePtr queue)
        {
            VkBool32 support = VK_FALSE;
            const uint32_t familyIndex = queue->getFamilyIndex();
            VERIFYVULKANRESULT(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, familyIndex, surface, &support));
            if (support)
                printf("Queue Family %d: Supports Present", familyIndex);
            return support == VK_TRUE;
        };

        bool compute = supportsPresent(mPhysicalDevice, mComputeQueue);
        if (mTransferQueue->getFamilyIndex() != mGfxQueue->getFamilyIndex() && mTransferQueue->getFamilyIndex() != mComputeQueue->getFamilyIndex())
        {
            supportsPresent(mPhysicalDevice, mTransferQueue);
        }
        if (mComputeQueue->getFamilyIndex() != mGfxQueue->getFamilyIndex() && compute)
        {
            mPresentQueue = mComputeQueue;
        }
        else
        {
            mPresentQueue = mGfxQueue;
        }
    }

    void VulkanDevice::createCommandPool()
    {
        VkCommandPoolCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.queueFamilyIndex = mGfxQueue->getFamilyIndex();

        VERIFYVULKANRESULT(vkCreateCommandPool(getHandle(), &createInfo, nullptr, mCommandPool.get()));
    }
}