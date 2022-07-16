//
// Created by FDC on 2022/1/14.
//

#ifndef HOMURA_VULKANTYPES_H
#define HOMURA_VULKANTYPES_H
#include <memory>
#include <cassert>
#include <functional>

namespace Homura
{
    class ApplicationWindow;
    class VulkanRHI;
    class VulkanDevice;
    class VulkanInstance;
    class VulkanRenderPass;
    class VulkanSubPass;
    class VulkanBuffer;
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
    class VulkanTexture1D;
    class VulkanTexture2D;
    class VulkanTexture3D;
    class VulkanTexture2DArray;
    class VulkanTextureDepth;
    class VulkanTextureCube;
    class VulkanShader;
    class VulkanShaderEntity;
    class VulkanSurface;
    class VulkanFences;
    class VulkanSemaphores;
    class VulkanPipeline;
    class VulkanPipelineLayout;
    class VulkanSampler;
    class VulkanFramebuffer;

    using ApplicationWindowPtr          = std::shared_ptr<ApplicationWindow>;
    using VulkanRHIPtr                  = std::shared_ptr<VulkanRHI>;
    using VulkanDevicePtr               = std::shared_ptr<VulkanDevice>;
    using VulkanInstancePtr             = std::shared_ptr<VulkanInstance>;
    using VulkanRenderPassPtr           = std::shared_ptr<VulkanRenderPass>;
    using VulkanSubPassPtr              = std::shared_ptr<VulkanSubPass>;
    using VulkanBufferPtr               = std::shared_ptr<VulkanBuffer>;
    using VulkanVertexBufferPtr         = std::shared_ptr<VulkanVertexBuffer>;
    using VulkanIndexBufferPtr          = std::shared_ptr<VulkanIndexBuffer>;
    using VulkanUniformBufferPtr        = std::shared_ptr<VulkanUniformBuffer>;
    using VulkanStagingBufferPtr        = std::shared_ptr<VulkanStagingBuffer>;
    using VulkanQueuePtr                = std::shared_ptr<VulkanQueue>;
    using VulkanSwapChainPtr            = std::shared_ptr<VulkanSwapChain>;
    using VulkanDescriptorPoolPtr       = std::shared_ptr<VulkanDescriptorPool>;
    using VulkanDescriptorSetPtr        = std::shared_ptr<VulkanDescriptorSet>;
    using VulkanDescriptorSetLayoutPtr  = std::shared_ptr<VulkanDescriptorSetLayout>;
    using VulkanCommandPoolPtr          = std::shared_ptr<VulkanCommandPool>;
    using VulkanCommandBufferPtr        = std::shared_ptr<VulkanCommandBuffer>;
    using VulkanShaderPtr               = std::shared_ptr<VulkanShader>;
    using VulkanShaderEntityPtr         = std::shared_ptr<VulkanShaderEntity>;
    using VulkanTexture1DPtr            = std::shared_ptr<VulkanTexture1D>;
    using VulkanTexture2DPtr            = std::shared_ptr<VulkanTexture2D>;
    using VulkanTexture3DPtr            = std::shared_ptr<VulkanTexture3D>;
    using VulkanTexture2DArrayPtr       = std::shared_ptr<VulkanTexture2DArray>;
    using VulkanTextureDepthPtr         = std::shared_ptr<VulkanTextureDepth>;
    using VulkanSurfacePtr              = std::shared_ptr<VulkanSurface>;
    using VulkanFencesPtr               = std::shared_ptr<VulkanFences>;
    using VulkanSemaphoresPtr           = std::shared_ptr<VulkanSemaphores>;
    using VulkanPipelinePtr             = std::shared_ptr<VulkanPipeline>;
    using VulkanPipelineLayoutPtr       = std::shared_ptr<VulkanPipelineLayout>;
    using VulkanSamplerPtr              = std::shared_ptr<VulkanSampler>;
    using VulkanFramebufferPtr          = std::shared_ptr<VulkanFramebuffer>;

    using MouseCallback                 = std::function<void(int, int, int)>;
    using FramebufferResizeCallback     = std::function<void(int, int)>;
}
#endif //HOMURA_VULKANTYPES_H
