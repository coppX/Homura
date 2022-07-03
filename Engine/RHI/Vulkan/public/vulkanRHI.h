//
// Created by FDC on 2022/1/6.
//

#ifndef HOMURA_VULKANRHI_H
#define HOMURA_VULKANRHI_H

#include <vulkan/vulkan.h>
#include <vulkanTypes.h>
#include <rhiResources.h>
#include <pixelFormat.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <string>

namespace Homura
{
    class VulkanRHI
    {
    public:
        VulkanRHI(GLFWwindow* window, uint32_t width, uint32_t height);
        virtual ~VulkanRHI();

        void init();
        void exit();

        VulkanInstancePtr getInstance();
        VulkanDevicePtr getDevice();
        VulkanSwapChainPtr getSwapChain();
        VulkanFramebufferPtr getFrameBuffer();

        VulkanInstancePtr createInstance();
        VulkanDevicePtr createDevice();
        VulkanSurfacePtr createSurface();
        VulkanSwapChainPtr createSwapChain();
        VulkanRenderPassPtr createRenderPass();
        VulkanDescriptorPoolPtr createDescriptorPool();
        VulkanDescriptorSetPtr createDescriptorSet(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
        VulkanCommandPoolPtr createCommandPool();
        VulkanFramebufferPtr createFrameBuffer();
        VulkanCommandBufferPtr createCommandBuffer();
        VulkanShaderPtr createShader();
        VulkanPipelinePtr createPipeline();

        void destroyInstance();
        void destroyDevice();
        void destroySurface();
        void destroySwapChain();
        void destroyRenderPass();
        void destroyDescriptorSet();
        void destroyDescriptorPool();
        void destroyFrameBuffer();
        void destroyCommandBuffer();
        void destroyCommandPool();
        void destroyShader();
        void destroyPipeline();

        VulkanTexture2DPtr createColorResources();
        VulkanTextureDepthPtr createDepthResources();
        void destroyColorResources();
        void destroyDepthResources();

        VulkanVertexBufferPtr createVertexBuffer(uint32_t size, void* pData);
        VulkanIndexBufferPtr createIndexBuffer(uint32_t size, void* pData);
        VulkanUniformBufferPtr createUniformBuffer(uint32_t size, void* pData);
        VulkanStagingBufferPtr createStagingBuffer(uint32_t size, void* pData);

        void idle();
//        void addPushConstant(const VkPushConstantRange& constantRange, const char* data);

        void setupAttachments();
        void setupRenderPass(RHIRenderPassInfo& info);
        void setupFramebuffer(std::vector<VulkanTexture2DPtr>& colorImages, std::vector<VulkanTextureDepthPtr>& depthStencilImages);
        void setupShaders(std::string filename, ShaderType type);
        void setupPipeline(VulkanDescriptorSetPtr descriptorSet);

    private:
        VulkanInstancePtr                   mInstance;;
        VulkanDevicePtr                     mDevice;
        VulkanSurfacePtr                    mSurface;
        VulkanSwapChainPtr                  mSwapChain;
        VulkanRenderPassPtr                 mRenderPass;
        VulkanDescriptorPoolPtr             mDescriptorPool;
        VulkanDescriptorSetPtr              mDescriptorSet;
        VulkanCommandPoolPtr                mCommandPool;
        VulkanCommandBufferPtr              mCommandBuffer;
        VulkanFramebufferPtr                mFramebuffer;
        VulkanPipelinePtr                   mPipeline;
        VulkanTexture2DPtr                  mRenderTarget;
        VulkanTextureDepthPtr               mRenderTargetDepth;
        VulkanShaderPtr                     mShader;

        GLFWwindow*                         mWindow;
        uint32_t                            mWidth;
        uint32_t                            mHeight;

        VulkanTexture2DPtr                  mDepthStencil;
    };
}
#endif //HOMURA_VULKANRHI_H
