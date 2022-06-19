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
        std::vector<VulkanSubPassPtr>           mSubPasses;
        std::vector<VkSubpassDependency>        mDependencies;
        std::vector<VkAttachmentDescription>    mAttachmentDescriptions;

        RHIRenderPassInfo()
            : mSubPasses{}
            , mDependencies{}
            , mAttachmentDescriptions{}
        {}

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
}

#endif //HOMURA_RHIRESOURCES_H
