//
// Created by 最上川 on 2022/1/22.
//

#ifndef HOMURA_VULKANSURFACE_H
#define HOMURA_VULKANSURFACE_H
#include <vulkan/vulkan.h>
#include <vulkanTypes.h>
#include <GLFW/glfw3.h>

namespace Homura
{
    class ENGINE_API VulkanSurface
    {
    public:
        VulkanSurface(VulkanInstancePtr instance, ApplicationWindowPtr window);
        ~VulkanSurface() = default;

        void create();
        void destroy();

        VkSurfaceKHR& getHandle()
        {
            return mSurface;
        }
    private:
        VulkanInstancePtr       mInstance;
        ApplicationWindowPtr    mWindow;
        VkSurfaceKHR            mSurface;
    };
}

#endif //HOMURA_VULKANSURFACE_H
