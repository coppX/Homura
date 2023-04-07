//
// Created by FDC on 2022/1/6.
//

#ifndef HOMURA_VULKANSHADER_H
#define HOMURA_VULKANSHADER_H
#include <vulkan/vulkan.h>
#include <vulkanTypes.h>
#include <vector>
#include <string>
#include <rhiResources.h>

namespace Homura
{
    class ENGINE_API VulkanShaderEntity
    {
    public:
        VulkanShaderEntity(VulkanDevicePtr device, VkShaderStageFlagBits stage, std::string entryPoint);
        ~VulkanShaderEntity() = default;

        void create(std::vector<char> shaderCode);
        void destroy();

        VkShaderStageFlagBits getStage()
        {
            return mStage;
        }

        VkShaderModule getHandle()
        {
            return mModule;
        }

        std::string& getName()
        {
            return mEntryPoint;
        }

        void setVertexAttributeDescription(std::vector<VkVertexInputAttributeDescription>& attributeDescriptions);
        void setVertexInputBindingDescription(VkVertexInputBindingDescription inputBindingDescription);
        uint32_t getVertexAttributeDesriptionCount() const;
        uint32_t getVertexInputBindingDescriptionCount() const;
        const VkVertexInputAttributeDescription* getVertexAttributeDesriptionData() const;
        const VkVertexInputBindingDescription* getVertexBindingDesriptionData() const;
    private:
        VulkanDevicePtr             mDevice;
        VkShaderModule              mModule;
        VkShaderStageFlagBits       mStage;
        std::string                 mEntryPoint;
        std::vector<VkVertexInputAttributeDescription>  mVertexInputAttributeDes;
        std::vector<VkVertexInputBindingDescription>    mVertexInputBindingDes;
    };

    class ENGINE_API VulkanShader
    {
    public:
        explicit VulkanShader(VulkanDevicePtr device);
        ~VulkanShader() = default;

        VulkanShaderEntityPtr setupShader(std::string filename, ShaderType type);
        void destroy();

        std::vector<VulkanShaderEntityPtr>& getShaders()
        {
            return mShaders;
        }

        uint32_t getVertexAttributeDesriptionCount() const;
        uint32_t getVertexInputBindingDescriptionCount() const;
        const VkVertexInputAttributeDescription* getVertexAttributeDesriptionData() const;
        const VkVertexInputBindingDescription* getVertexBindingDesriptionData() const;

    private:
        std::vector<char> readFile(const std::string &filename);
    private:
        VulkanDevicePtr                     mDevice;
        std::vector<VulkanShaderEntityPtr>  mShaders;
    };
}

#endif //HOMURA_VULKANSHADER_H
