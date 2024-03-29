//
// Created by FDC on 2022/1/13.
//

#ifndef HOMURA_VULKANRENDERPASS_H
#define HOMURA_VULKANRENDERPASS_H
#include <vulkan/vulkan.h>
#include <vulkanTypes.h>
#include <vector>
#include <rhiResources.h>

namespace Homura
{
    struct ENGINE_API RHIRenderPassInfo;

    class ENGINE_API VulkanSubPass
    {
    public:
        explicit VulkanSubPass(AttachmentReference& references);
        ~VulkanSubPass() = default;

        VkSubpassDescription& getHandle()
        {
            return mSubPassDescription;
        }
    private:
        VkSubpassDescription                mSubPassDescription;
    };

    class ENGINE_API VulkanRenderPass
    {
    public:
        explicit VulkanRenderPass(VulkanDevicePtr device);
        ~VulkanRenderPass();

        void set(RHIRenderPassInfo info);
        void build();
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
        RHIRenderPassInfo                       mInfo;
    };
}
#endif //HOMURA_VULKANRENDERPASS_H
