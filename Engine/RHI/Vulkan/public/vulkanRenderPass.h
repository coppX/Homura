//
// Created by FDC on 2022/1/13.
//

#ifndef HOMURA_VULKANRENDERPASS_H
#define HOMURA_VULKANRENDERPASS_H
#include <vulkan/vulkan.h>
#include <vulkanTypes.h>
#include <vector>

namespace Homura
{
    struct RHIRenderPassInfo;

    class VulkanSubPass
    {
    public:
        VulkanSubPass(std::vector<VkAttachmentReference> &colorAttachmentRef,
                      std::vector<VkAttachmentReference> &inputAttachmentRef,
                      const VkAttachmentReference &depthStencilAttachmentRef,
                      const VkAttachmentReference &resolveAttachmentRef);
        ~VulkanSubPass();

        VkSubpassDescription& getHandle()
        {
            return mSubPassDescription;
        }
    private:
        VkSubpassDescription                mSubPassDescription;
        std::vector<VkAttachmentReference>  mColorAttachmentReferences;
        std::vector<VkAttachmentReference>  mInputAttachmentReferences;
        VkAttachmentReference               mDepthStencilAttachmentReference;
        VkAttachmentReference               mResolvedAttachmentReference;
    };

    class VulkanRenderPass
    {
    public:
        VulkanRenderPass(VulkanDevicePtr device);
        ~VulkanRenderPass();

        void create(RHIRenderPassInfo& info);
        void destroy();

        VkRenderPass& getHandle()
        {
            return mRenderPass;
        }

    private:
        VulkanDevicePtr                         mDevice;
        VkRenderPass                            mRenderPass;

        std::vector<VulkanSubPass>              mSubPasses;
        std::vector<VkSubpassDependency>        mDependencies;
        std::vector<VkAttachmentDescription>    mAttachmentDescriptions;
    };
}
#endif //HOMURA_VULKANRENDERPASS_H
