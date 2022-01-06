//
// Created by FDC on 2022/1/5.
//

#ifndef HOMURA_APPLICATION_H
#define HOMURA_APPLICATION_H

#include "GLFW/glfw3.h"
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

        void resize(const uint32_t width, const uint32_t height)
        {
            mWidth = width;
            mHeight = height;
        }

        uint32_t getWidth()
        {
            return mWidth;
        }

        uint32_t getHeight()
        {
            return mHeight;
        }

        const std::string &getTitle()
        {
            return mTitle;
        }

        virtual void initWindow()
        {
            glfwInit();
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            mWindow = glfwCreateWindow(getWidth(), getHeight(), getTitle().c_str(), nullptr, nullptr);
        }

        virtual bool init() = 0;

        virtual void update() = 0;

        virtual void exit() = 0;

    public:
//        virtual bool OnKeyDown(const KeyCode key);
//
//        virtual bool OnKeyUp(const KeyCode key);
//
//        virtual bool OnMouseDown(const MouseButton button);
//
//        virtual bool OnMouseUp(const MouseButton button);

    protected:
        uint32_t mWidth = 960;
        uint32_t mHeight = 540;
        std::string mTitle = "madoka";

        GLFWwindow *mWindow = nullptr;

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
