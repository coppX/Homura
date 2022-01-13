//
// Created by FDC on 2022/1/13.
//

#ifndef HOMURA_VULKANRENDERPASS_H
#define HOMURA_VULKANRENDERPASS_H
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

namespace Homura
{
    class VulkanDevice;

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
        VulkanRenderPass(std::shared_ptr<VulkanDevice> device);
        ~VulkanRenderPass();

        void addSubPass(const VulkanSubPass &subPass);

        void addDependency(const VkSubpassDependency &dependency);

        void addAttachment(const VkAttachmentDescription &attachment);

        VkRenderPass& getHandle()
        {
            return mRenderPass;
        }

        void buildRenderPass();
    private:
        std::shared_ptr<VulkanDevice>           mDevice;
        VkRenderPass                            mRenderPass;

        std::vector<VulkanSubPass>              mSubPasses;
        std::vector<VkSubpassDependency>        mDependencies{};
        std::vector<VkAttachmentDescription>    mAttachmentDescriptions;
    };
}
#endif //HOMURA_VULKANRENDERPASS_H
