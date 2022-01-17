//
// Created by FDC on 2022/1/14.
//

#ifndef HOMURA_VULKANTYPES_H
#define HOMURA_VULKANTYPES_H
#include <memory>

namespace Homura
{
    class VulkanDevice;
    class VulkanRenderPass;
    class VulkanBuffer;
    class VulkanQueue;
    class VulkanSwapChain;
    class VulkanDescriptorPool;
    class VulkanDescriptorSetLayout;
    class VulkanCommandBuffer;
    class VulkanTexture;
    class VulkanShader;

    using VulkanDevicePtr = std::shared_ptr<VulkanDevice>;
    using VulkanRenderPassPtr = std::shared_ptr<VulkanRenderPass>;
    using VulkanBufferPtr = std::shared_ptr<VulkanBuffer>;
    using VulkanQueuePtr = std::shared_ptr<VulkanQueue>;
    using VulkanSwapChainPtr = std::shared_ptr<VulkanSwapChain>;
    using VulkanDescriptorPoolPtr = std::shared_ptr<VulkanDescriptorPool>;
    using VulkanDescriptorSetLayoutPtr = std::shared_ptr<VulkanDescriptorSetLayout>;
    using VulkanCommandBufferPtr = std::shared_ptr<VulkanCommandBuffer>;
    using VulkanShaderPtr = std::shared_ptr<VulkanShader>;
    using VulkanTexturePtr = std::shared_ptr<VulkanTexture>;
}
#endif //HOMURA_VULKANTYPES_H
