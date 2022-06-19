//
// Created by coppx on 2022/6/16/016.
//

#ifndef HOMURA_RHIRESOURCES_H
#define HOMURA_RHIRESOURCES_H

#include <vulkan/vulkan.h>
#include <vulkanTypes.h>
#include <vector>

namespace Homura {
    struct RHISetRenderTargetsInfo
    {
        int numColorRenderTargets;
        bool bClearColor;
        bool bClearDepth;
        bool bClearStencil;
        bool bHasResolveAttachments;
    };

    struct RHIRenderPassInfo
    {
        std::vector<VulkanSubPassPtr> mSubPasses;
        std::vector<VkSubpassDependency> mDependencies;
        std::vector<VkAttachmentDescription> mAttachmentDescriptions;// color, depth

        RHIRenderPassInfo()
            : mSubPasses{}, mDependencies{}, mAttachmentDescriptions{} {}

        void addSubPass(const VulkanSubPassPtr subPass)
        {
            mSubPasses.push_back(subPass);
        }

        void addDependency(const VkSubpassDependency &dependency)
        {
            mDependencies.push_back(dependency);
        }

        void addAttachment(const VkAttachmentDescription &attachment)
        {
            mAttachmentDescriptions.push_back(attachment);
        }
    };

    struct ColorAttachmentReference
    {
        ColorAttachmentReference(uint32_t attachmentIndex)
            : mColorReference{}
        {
            if (attachmentIndex != -1)
            {
                mColorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                mColorReference.attachment = attachmentIndex;
            }
        }

        VkAttachmentReference& getHandle()
        {
            return mColorReference;
        }

        VkAttachmentReference mColorReference;
    };

    struct DepthAttachmentReference
    {
        DepthAttachmentReference(uint32_t attachmentIndex)
            : mDepthReference{}
        {
            mDepthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            mDepthReference.attachment = attachmentIndex;
        }

        VkAttachmentReference& getHandle()
        {
            return mDepthReference;
        }

        VkAttachmentReference mDepthReference;
    };
}

#endif //HOMURA_RHIRESOURCES_H
