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
    class VulkanSubPass
    {
    public:
        VulkanSubPass(std::vector<VkAttachmentReference> &colorAttachmentRef,
                      std::vector<VkAttachmentReference> &inputAttachmentRef,
                      const VkAttachmentReference &depthStencilAttachmentRef,
                      const VkAttachmentReference &resolveAttachmentRef);
        ~VulkanSubPass();

        VkSubpassDescription& getSubPassHandle()
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

        void create();
        void destroy();

        void addSubPass(const VulkanSubPass &subPass);

        void addDependency(const VkSubpassDependency &dependency);

        void addAttachment(const VkAttachmentDescription &attachment);

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
