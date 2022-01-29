//
// Created by FDC on 2022/1/14.
//

#ifndef HOMURA_VULKANTYPES_H
#define HOMURA_VULKANTYPES_H
#include <memory>
#include <cassert>

namespace Homura
{
    class VulkanDevice;
    class VulkanInstance;
    class VulkanRenderPass;
    class VulkanVertexBuffer;
    class VulkanIndexBuffer;
    class VulkanUniformBuffer;
    class VulkanStagingBuffer;
    class VulkanQueue;
    class VulkanSwapChain;
    class VulkanDescriptorPool;
    class VulkanDescriptorSetLayout;
    class VulkanDescriptorSet;
    class VulkanCommandPool;
    class VulkanCommandBuffer;
    class VulkanTexture;
    class VulkanShader;
    class VulkanSurface;
    class VulkanFence;
    class VulkanPipeline;
    class VulkanPipelineLayout;

    using VulkanDevicePtr = std::shared_ptr<VulkanDevice>;
    using VulkanInstancePtr = std::shared_ptr<VulkanInstance>;
    using VulkanRenderPassPtr = std::shared_ptr<VulkanRenderPass>;
    using VulkanVertexBufferPtr = std::shared_ptr<VulkanVertexBuffer>;
    using VulkanIndexBufferPtr = std::shared_ptr<VulkanIndexBuffer>;
    using VulkanUniformBufferPtr = std::shared_ptr<VulkanUniformBuffer>;
    using VulkanStagingBufferPtr = std::shared_ptr<VulkanStagingBuffer>;
    using VulkanQueuePtr = std::shared_ptr<VulkanQueue>;
    using VulkanSwapChainPtr = std::shared_ptr<VulkanSwapChain>;
    using VulkanDescriptorPoolPtr = std::shared_ptr<VulkanDescriptorPool>;
    using VulkanDescriptorSetPtr = std::shared_ptr<VulkanDescriptorSet>;
    using VulkanDescriptorSetLayoutPtr = std::shared_ptr<VulkanDescriptorSetLayout>;
    using VulkanCommandPoolPtr = std::shared_ptr<VulkanCommandPool>;
    using VulkanCommandBufferPtr = std::shared_ptr<VulkanCommandBuffer>;
    using VulkanShaderPtr = std::shared_ptr<VulkanShader>;
    using VulkanTexturePtr = std::shared_ptr<VulkanTexture>;
    using VulkanSurfacePtr = std::shared_ptr<VulkanSurface>;
    using VulkanFencePtr = std::shared_ptr<VulkanFence>;
    using VulkanPipelinePtr = std::shared_ptr<VulkanPipeline>;
    using VulkanPipelineLayoutPtr = std::shared_ptr<VulkanPipelineLayout>;
}
#endif //HOMURA_VULKANTYPES_H
