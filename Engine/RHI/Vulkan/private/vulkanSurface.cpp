//
// Created by 最上川 on 2022/1/22.
//
#include <vulkanSurface.h>
#include <vulkanInstance.h>
#include <debugUtils.h>

namespace Homura
{
    VulkanSurface::VulkanSurface(VulkanInstancePtr instance, GLFWwindow* window)
        : mInstance{instance}
        , mWindow{window}
        , mSurface{VK_NULL_HANDLE}
    {

    }

    VulkanSurface::~VulkanSurface()
    {
        destroy();
    }

    void VulkanSurface::create()
    {
        VERIFYVULKANRESULT(glfwCreateWindowSurface(mInstance->getHandle(), mWindow, nullptr, &mSurface));
    }

    void VulkanSurface::destroy()
    {
        if (mSurface != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(mInstance->getHandle(), mSurface, nullptr);
        }
    }
}