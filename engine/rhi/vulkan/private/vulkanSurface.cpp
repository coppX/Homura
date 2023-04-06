//
// Created by 最上川 on 2022/1/22.
//
#include <vulkanSurface.h>
#include <vulkanInstance.h>
#include <applicationWindow.h>
#include <debugUtils.h>

namespace Homura
{
    VulkanSurface::VulkanSurface(VulkanInstancePtr instance, ApplicationWindowPtr window)
        : mInstance{instance}
        , mWindow{window}
        , mSurface{VK_NULL_HANDLE}
    {
        create();
    }

    void VulkanSurface::create()
    {
        VERIFYVULKANRESULT(glfwCreateWindowSurface(mInstance->getHandle(), mWindow->getHandle(), nullptr, &mSurface));
    }

    void VulkanSurface::destroy()
    {
        if (mSurface != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(mInstance->getHandle(), mSurface, nullptr);
            mSurface = VK_NULL_HANDLE;
        }
    }
}