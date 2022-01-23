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
    class VulkanSurface
    {
    public:
        VulkanSurface(VulkanInstancePtr instance, GLFWwindow* window);
        ~VulkanSurface();

        void create();
        void destroy();

    private:
        VulkanInstancePtr   mInstance;
        GLFWwindow*         mWindow;
        VkSurfaceKHR        mSurface;
    };
}

#endif //HOMURA_VULKANSURFACE_H
