//
// Created by 最上川 on 2022/1/22.
//

#ifndef HOMURA_VULKANINSTANCE_H
#define HOMURA_VULKANINSTANCE_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vulkanTypes.h>
#include <vector>

namespace Homura
{
    class ENGINE_API VulkanInstance
    {
    public:
        VulkanInstance();
        ~VulkanInstance();

        void create();
        void destroy();
        std::vector<const char*> getRequiredExtensions();
        bool checkValidationLayerSupport();
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

        VkInstance& getHandle()
        {
            return mInstance;
        }
    private:
        VkInstance                      mInstance;
        bool                            mEnableValidationLayer = true;
        const std::vector<const char*>  mValidationLayers = {
                "VK_LAYER_KHRONOS_validation"
        };
    };
}
#endif //HOMURA_VULKANINSTANCE_H
