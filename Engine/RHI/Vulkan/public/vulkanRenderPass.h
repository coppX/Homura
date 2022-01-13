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
        VulkanSubPass();
        ~VulkanSubPass();

        void addColorAttachmentReference(const VkAttachmentReference &ref);

        void addInputAttachmentReference(const VkAttachmentReference &ref);

        void setDepthStencilAttachmentReference(const VkAttachmentReference &ref);

        void setResolveAttachmentReference(const VkAttachmentReference &ref);

        void buildSubPassDescription();

        VkSubpassDescription& getSubPassHandle()
        {
            return mSubPassDescription;
        }
    private:
        VkSubpassDescription                mSubPassDescription{};
        std::vector<VkAttachmentReference>  mColorAttachmentReferences{};
        std::vector<VkAttachmentReference>  mInputAttachmentReferences{};
        VkAttachmentReference               mDepthStencilAttachmentReference{};
        VkAttachmentReference               mResolvedAttachmentReference{};
    };

    class VulkanRenderPass
    {
    public:
        VulkanRenderPass(std::shared_ptr<VulkanDevice> device);
        ~VulkanRenderPass();

        VkRenderPass& getHandle()
        {
            return mRenderPass;
        }
    private:
        std::shared_ptr<VulkanDevice>   mDevice;
        VkRenderPass                    mRenderPass;
    };
}
#endif //HOMURA_VULKANRENDERPASS_H
