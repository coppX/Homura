//
// Created by FDC on 2022/1/5.
//

#ifndef HOMURA_APPLICATION_H
#define HOMURA_APPLICATION_H

#include "input.h"
#include "vulkan/vulkan.h"

#include <string>
#include <vector>

#include "pixelFormat.h"

namespace Homura
{
    class Application
    {
    public:
        Application() = default;
        virtual ~Application() = default;
        virtual bool init() = 0;
        virtual void update() = 0;
        virtual void exit() = 0;

    protected:
        struct Texture
        {
            VkImage image;
            VkDeviceMemory mem;
            VkImageView view;
        };

        Texture mDepthStencil;
        VkSampleCountFlagBits mSampleCount;
        VkRenderPass mRenderPass;
        EPixelFormat mDepthFormat;

        std::vector<VkFramebuffer> mFrameBuffers;
    };

};
#endif //HOMURA_APPLICATION_H
