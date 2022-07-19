//
// Created by FDC on 2022/1/6.
//

#include <vulkanShader.h>
#include <vulkanDevice.h>
#include <debugUtils.h>
#include <fstream>

namespace Homura
{
    VulkanShaderEntity::VulkanShaderEntity(VulkanDevicePtr device, VkShaderStageFlagBits stage, std::string entryPoint)
        : mDevice{device}
        , mStage{stage}
        , mEntryPoint{entryPoint}
        , mModule{VK_NULL_HANDLE}
        , mVertexInputAttributeDes{}
        , mVertexInputBindingDes{}
    {

    }

    VulkanShaderEntity::~VulkanShaderEntity()
    {

    }

    void VulkanShaderEntity::create(std::vector<char> shaderCode)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = shaderCode.size();
        createInfo.pCode    = reinterpret_cast<const uint32_t*>(shaderCode.data());
        VERIFYVULKANRESULT(vkCreateShaderModule(mDevice->getHandle(), &createInfo, nullptr, &mModule));
    }

    void VulkanShaderEntity::destroy()
    {
        if (mModule != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(mDevice->getHandle(), mModule, nullptr);
            mModule = VK_NULL_HANDLE;
        }
    }

    void VulkanShaderEntity::setVertexAttributeDescription(std::vector<VkVertexInputAttributeDescription>& attributeDescriptions)
    {
        for (const VkVertexInputAttributeDescription& attribute : attributeDescriptions)
        {
            mVertexInputAttributeDes.push_back(attribute);
        }
    }

    void VulkanShaderEntity::setVertexInputBindingDescription(VkVertexInputBindingDescription inputBindingDescription)
    {
        mVertexInputBindingDes.push_back(inputBindingDescription);
    }

    uint32_t VulkanShaderEntity::getVertexAttributeDesriptionCount() const
    {
        return mVertexInputAttributeDes.size();
    }

    uint32_t VulkanShaderEntity::getVertexInputBindingDescriptionCount() const
    {
        return mVertexInputBindingDes.size();
    }

    const VkVertexInputAttributeDescription* VulkanShaderEntity::getVertexAttributeDesriptionData() const
    {
        return mVertexInputAttributeDes.data();
    }

    const VkVertexInputBindingDescription* VulkanShaderEntity::getVertexBindingDesriptionData() const
    {
        return mVertexInputBindingDes.data();
    }

    VulkanShader::VulkanShader(VulkanDevicePtr device)
        : mDevice{device}
    {

    }

    VulkanShader::~VulkanShader()
    {

    }

    std::vector<char> VulkanShader::readFile(const std::string &filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error("failed to open file!");
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();
        return buffer;
    }

    VulkanShaderEntityPtr VulkanShader::setupShader(std::string filename, ShaderType type)
    {
        VkShaderStageFlagBits stage;
        if (type == ShaderType::VERTEX)
            stage = VK_SHADER_STAGE_VERTEX_BIT;
        else if (type == ShaderType::TESSELLATION_CONTROL)
            stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        else if (type == ShaderType::TESSELLATION_EVALUATION)
            stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        else if (type == ShaderType::GEOMETRY)
            stage = VK_SHADER_STAGE_GEOMETRY_BIT;
        else if (type == ShaderType::FRAGMENT)
            stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        else if (type == ShaderType::COMPUTE)
            stage = VK_SHADER_STAGE_COMPUTE_BIT;
        else
            stage = VK_SHADER_STAGE_ALL_GRAPHICS;

        VulkanShaderEntityPtr shader = std::make_shared<VulkanShaderEntity>(mDevice, stage, std::string("main"));
        shader->create(readFile(filename));
        mShaders.push_back(shader);
        return shader;
    }

    uint32_t VulkanShader::getVertexAttributeDesriptionCount() const
    {
        for (const VulkanShaderEntityPtr& shader : mShaders)
        {
            if (shader->getStage() == VK_SHADER_STAGE_VERTEX_BIT)
            {
                return shader->getVertexAttributeDesriptionCount();
            }
        }
    }

    uint32_t VulkanShader::getVertexInputBindingDescriptionCount() const
    {
        for (const VulkanShaderEntityPtr& shader : mShaders)
        {
            if (shader->getStage() == VK_SHADER_STAGE_VERTEX_BIT)
            {
                return shader->getVertexInputBindingDescriptionCount();
            }
        }
    }

    const VkVertexInputAttributeDescription* VulkanShader::getVertexAttributeDesriptionData() const
    {
        for (const VulkanShaderEntityPtr& shader : mShaders)
        {
            if (shader->getStage() == VK_SHADER_STAGE_VERTEX_BIT)
            {
                return shader->getVertexAttributeDesriptionData();
            }
        }
    }

    const VkVertexInputBindingDescription* VulkanShader::getVertexBindingDesriptionData() const
    {
        for (const VulkanShaderEntityPtr& shader : mShaders)
        {
            if (shader->getStage() == VK_SHADER_STAGE_VERTEX_BIT)
            {
                return shader->getVertexBindingDesriptionData();
            }
        }
    }

    void VulkanShader::destroy()
    {
        for (auto& shader : mShaders)
        {
            shader->destroy();
        }
        mShaders.clear();
    }
}