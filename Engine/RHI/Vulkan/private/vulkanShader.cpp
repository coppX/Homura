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

    void VulkanShader::setupShader(std::string filename, ShaderType type)
    {
        VkShaderStageFlagBits stage;
        if (type == VERTEX)
            stage = VK_SHADER_STAGE_VERTEX_BIT;
        else if (type == TESSELLATION_CONTROL)
            stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        else if (type == TESSELLATION_EVALUATION)
            stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        else if (type == GEOMETRY)
            stage = VK_SHADER_STAGE_GEOMETRY_BIT;
        else if (type == FRAGMENT)
            stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        else if (type == COMPUTE)
            stage = VK_SHADER_STAGE_COMPUTE_BIT;
        else
            stage = VK_SHADER_STAGE_ALL_GRAPHICS;

        VulkanShaderEntity shader(mDevice, stage, std::string("main"));
        shader.create(readFile(filename));
        mShaders.push_back(shader);
    }

    void VulkanShader::destroy()
    {
        for (auto& shader : mShaders)
        {
            shader.destroy();
        }
        mShaders.clear();
    }
}