#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <iostream>
#include <fstream>
#include <exception>
#include <algorithm>
#include <vector>
#include <optional>
#include <set>
#include <array>
#include <chrono>
#include <string>
#include <unordered_map>
#include <memory>

#include <filesystem.h>
#include <application.h>
#include <debugUtils.h>
#include <vulkanShader.h>
#include <vulkanRHI.h>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }

    bool operator==(const Vertex &vertex) const
    {
        return pos == vertex.pos && color == vertex.color && texCoord == vertex.texCoord;
    }
};

template<>
struct std::hash<Vertex>
{
    size_t operator()(const Vertex &vertex) const
    {
        return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
    }
};

namespace Homura
{
    const std::string MODEL_PATH = FileSystem::getPath("resources/models/viking_room.obj");
    const std::string TEXTURE_PATH = FileSystem::getPath("resources/textures/viking_room.png");

    const int MAX_FRAMES_IN_FLIGHT = 2;

    const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct UniformBufferObject
    {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };

    class TriangleApplication : Application
    {
    public:
        TriangleApplication()
            : rhi{std::make_shared<VulkanRHI>()}
        {

        }

        ~TriangleApplication()
        {
            exit();
        }

        bool init()
        {
            initWindow();
            initVulkan();
            update();
            return true;
        }

        void update()
        {
            while (!glfwWindowShouldClose(mWindow))
            {
                processInput(mWindow);

                glfwPollEvents();
                drawFrame();
            }
            vkDeviceWaitIdle(rhi->getDevice()->getHandle());
        }

        void exit()
        {
            cleanupSwapChain();

            vkDestroySampler(rhi->getDevice()->getHandle(), textureSampler, nullptr);
            vkDestroyImageView(rhi->getDevice()->getHandle(), textureImageView, nullptr);

            vkDestroyImage(rhi->getDevice()->getHandle(), textureImage, nullptr);
            vkFreeMemory(rhi->getDevice()->getHandle(), textureImageMemory, nullptr);

            vkDestroyDescriptorSetLayout(rhi->getDevice()->getHandle(), descriptorSetLayout, nullptr);

            vkDestroyBuffer(rhi->getDevice()->getHandle(), indexBuffer, nullptr);
            vkFreeMemory(rhi->getDevice()->getHandle(), indexBufferMemory, nullptr);

            vkDestroyBuffer(rhi->getDevice()->getHandle(), vertexBuffer, nullptr);
            vkFreeMemory(rhi->getDevice()->getHandle(), vertexBufferMemory, nullptr);

            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                vkDestroySemaphore(rhi->getDevice()->getHandle(), renderFinishedSemaphores[i], nullptr);
                vkDestroySemaphore(rhi->getDevice()->getHandle(), imageAvailableSemaphores[i], nullptr);
                vkDestroyFence(rhi->getDevice()->getHandle(), inFlightFences[i], nullptr);
            }

            vkDestroyCommandPool(rhi->getDevice()->getHandle(), commandPool, nullptr);

            vkDestroyDevice(rhi->getDevice()->getHandle(), nullptr);

            if (enableValidationLayers)
            {
                DestroyDebugUtilsMessengerEXT(rhi->getInstance(), debugMessenger, nullptr);
            }

            vkDestroySurfaceKHR(rhi->getInstance(), surface, nullptr);
            rhi->destroyInstance();

            glfwDestroyWindow(mWindow);
            glfwTerminate();
        }

    private:
        void initWindow()
        {
            Application::initWindow();
            glfwSetWindowUserPointer(mWindow, this);
            glfwSetFramebufferSizeCallback(mWindow, framebufferResizeCallback);
        }

        static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
        {
            auto app = reinterpret_cast<TriangleApplication*>(glfwGetWindowUserPointer(window));
            app->framebufferResized = true;
        }

        void initVulkan()
        {
            rhi->createInstance();
            rhi->selectAndInitDevice();
            setupDebugMessenger();
            createSurface();
            createSwapChain();
            createImageViews();
            createRenderPass();
            createDescriptorSetLayout();
            createGraphicsPipeline();
            createCommandPool();
            createColorResources();
            createDepthResources();
            createFrameBuffers();
            createTextureImage();
            createTextureImageView();
            createTextureSampler();
            loadModel();
            createVertexBuffer();
            createIndexBuffer();
            createUniformBuffers();
            createDescriptorPool();
            createDescriptorSets();
            createCommandBuffers();
            createSyncObjects();
        }

        void recreateSwapChain()
        {
            int width = 0, height = 0;
            glfwGetWindowSize(mWindow, &width, &height);
            while (width == 0 || height == 0)
            {
                glfwGetFramebufferSize(mWindow, &width, &height);
                glfwWaitEvents();
            }

            vkDeviceWaitIdle(rhi->getDevice()->getHandle());
            cleanupSwapChain();

            createSwapChain();
            createImageViews();
            createRenderPass();
            createGraphicsPipeline();
            createColorResources();
            createDepthResources();
            createFrameBuffers();
            createUniformBuffers();
            createDescriptorPool();
            createDescriptorSets();
            createCommandBuffers();

            imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);
        }

        void cleanupSwapChain()
        {
            vkDestroyImageView(rhi->getDevice()->getHandle(), depthImageView, nullptr);
            vkDestroyImage(rhi->getDevice()->getHandle(), depthImage, nullptr);
            vkFreeMemory(rhi->getDevice()->getHandle(), depthImageMemory, nullptr);

            vkDestroyImageView(rhi->getDevice()->getHandle(), colorImageView, nullptr);
            vkDestroyImage(rhi->getDevice()->getHandle(), colorImage, nullptr);
            vkFreeMemory(rhi->getDevice()->getHandle(), colorImageMemory, nullptr);

            for (auto frameBuffer : swapChainFrameBuffers)
            {
                vkDestroyFramebuffer(rhi->getDevice()->getHandle(), frameBuffer, nullptr);
            }

            vkFreeCommandBuffers(rhi->getDevice()->getHandle(), commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

            vkDestroyPipeline(rhi->getDevice()->getHandle(), graphicsPipeline, nullptr);
            vkDestroyPipelineLayout(rhi->getDevice()->getHandle(), pipelineLayout, nullptr);
            vkDestroyRenderPass(rhi->getDevice()->getHandle(), renderPass, nullptr);

            for (auto imageView : swapChainImageViews)
            {
                vkDestroyImageView(rhi->getDevice()->getHandle(), imageView, nullptr);
            }

            vkDestroySwapchainKHR(rhi->getDevice()->getHandle(), swapChain, nullptr);

            for (size_t i = 0; i < swapChainImages.size(); i++)
            {
                vkDestroyBuffer(rhi->getDevice()->getHandle(), uniformBuffers[i], nullptr);
                vkFreeMemory(rhi->getDevice()->getHandle(), uniformBuffersMemory[i], nullptr);
            }

            vkDestroyDescriptorPool(rhi->getDevice()->getHandle(), descriptorPool, nullptr);
        }

        void processInput(GLFWwindow* window)
        {
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);
        }

        void setupDebugMessenger()
        {
            if (!enableValidationLayers) return;

            VkDebugUtilsMessengerCreateInfoEXT createInfo;
            rhi->populateDebugMessengerCreateInfo(createInfo);

            if (CreateDebugUtilsMessengerEXT(rhi->getInstance(), &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to set up debug messenger!");
            }
        }

        void createSurface()
        {
            if (glfwCreateWindowSurface(rhi->getInstance(), mWindow, nullptr, &surface) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create window surface!");
            }
        }

        void createSwapChain()
        {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(rhi->getDevice()->getPhysicalHandle());

            VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
            VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
            VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

            uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
            if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
            {
                imageCount = swapChainSupport.capabilities.maxImageCount;
            }

            VkSwapchainCreateInfoKHR createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            createInfo.surface = surface;

            createInfo.minImageCount = imageCount;
            createInfo.imageFormat = surfaceFormat.format;
            createInfo.imageColorSpace = surfaceFormat.colorSpace;
            createInfo.imageExtent = extent;
            createInfo.imageArrayLayers = 1;
            createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            QueueFamilyIndices indices = findQueueFamilies(rhi->getDevice()->getPhysicalHandle());
            uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

            if (indices.graphicsFamily != indices.presentFamily)
            {
                createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                createInfo.queueFamilyIndexCount = 2;
                createInfo.pQueueFamilyIndices = queueFamilyIndices;
            }
            else
            {
                createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            }

            createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
            createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            createInfo.presentMode = presentMode;
            createInfo.clipped = VK_TRUE;

            if (vkCreateSwapchainKHR(rhi->getDevice()->getHandle(), &createInfo, nullptr, &swapChain) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create swap chain!");
            }

            vkGetSwapchainImagesKHR(rhi->getDevice()->getHandle(), swapChain, &imageCount, nullptr);
            swapChainImages.resize(imageCount);
            vkGetSwapchainImagesKHR(rhi->getDevice()->getHandle(), swapChain, &imageCount, swapChainImages.data());

            swapChainImageFormat = surfaceFormat.format;
            swapChainExtent = extent;
        }

        void createImageViews()
        {
            swapChainImageViews.resize(swapChainImages.size());

            for (size_t i = 0; i < swapChainImages.size(); i++)
            {
                swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
            }
        }

        void createRenderPass()
        {
            VkAttachmentDescription colorAttachment{};
            colorAttachment.format = swapChainImageFormat;
            colorAttachment.samples = msaaSamples;
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkAttachmentDescription depthAttachment{};
            depthAttachment.format = findDepthFormat();
            depthAttachment.samples = msaaSamples;
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            VkAttachmentDescription colorAttachmentResolve{};
            colorAttachmentResolve.format = swapChainImageFormat;
            colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            VkAttachmentReference colorAttachmentRef{};
            colorAttachmentRef.attachment = 0;
            colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkAttachmentReference depthAttachmentRef{};
            depthAttachmentRef.attachment = 1;
            depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            VkAttachmentReference colorAttachmentResolveRef{};
            colorAttachmentResolveRef.attachment = 2;
            colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpass{};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorAttachmentRef;
            subpass.pDepthStencilAttachment = &depthAttachmentRef;
            subpass.pResolveAttachments = &colorAttachmentResolveRef;

            VkSubpassDependency dependency{};
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;
            dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            dependency.srcAccessMask = 0;
            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            std::array<VkAttachmentDescription, 3> attachments = {colorAttachment, depthAttachment, colorAttachmentResolve};
            VkRenderPassCreateInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            renderPassInfo.pAttachments = attachments.data();
            renderPassInfo.subpassCount = 1;
            renderPassInfo.pSubpasses = &subpass;
            renderPassInfo.dependencyCount = 1;
            renderPassInfo.pDependencies = &dependency;

            if (vkCreateRenderPass(rhi->getDevice()->getHandle(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create render pass!");
            }
        }

        void createDescriptorSetLayout()
        {
            VkDescriptorSetLayoutBinding uboLayoutBinding{};
            uboLayoutBinding.binding = 0;
            uboLayoutBinding.descriptorCount = 1;
            uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboLayoutBinding.pImmutableSamplers = nullptr;
            uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

            VkDescriptorSetLayoutBinding samplerLayoutBinding{};
            samplerLayoutBinding.binding = 1;
            samplerLayoutBinding.descriptorCount = 1;
            samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerLayoutBinding.pImmutableSamplers = nullptr;
            samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

            std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
            VkDescriptorSetLayoutCreateInfo layoutInfo{};
            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
            layoutInfo.pBindings = bindings.data();

            if (vkCreateDescriptorSetLayout(rhi->getDevice()->getHandle(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create descriptor set layout!");
            }
        }

        void createGraphicsPipeline()
        {
            VulkanShader vertShader(std::make_shared<VkDevice>(rhi->getDevice()->getHandle()));
            VulkanShader fragShader(std::make_shared<VkDevice>(rhi->getDevice()->getHandle()));

            vertShader.createShaderModule(FileSystem::getPath("resources/shader/triangle/shader_depth.vert.spv"));
            fragShader.createShaderModule(FileSystem::getPath("resources/shader/triangle/shader_depth.frag.spv"));

            VkShaderModule vertShaderModule = vertShader.getHandle();
            VkShaderModule fragShaderModule = fragShader.getHandle();

            VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
            vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            vertShaderStageInfo.module = vertShaderModule;
            vertShaderStageInfo.pName = "main";

            VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
            fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            fragShaderStageInfo.module = fragShaderModule;
            fragShaderStageInfo.pName = "main";

            VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

            VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
            vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

            auto bindingDescription = Vertex::getBindingDescription();
            auto attributeDescriptions = Vertex::getAttributeDescriptions();

            vertexInputInfo.vertexBindingDescriptionCount = 1;
            vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
            vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
            vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

            VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssembly.primitiveRestartEnable = VK_FALSE;

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = (float) swapChainExtent.width;
            viewport.height = (float) swapChainExtent.height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            VkRect2D scissor{};
            scissor.offset = {0, 0};
            scissor.extent = swapChainExtent;

            VkPipelineViewportStateCreateInfo viewportState{};
            viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.pViewports = &viewport;
            viewportState.scissorCount = 1;
            viewportState.pScissors = &scissor;

            VkPipelineRasterizationStateCreateInfo rasterizer{};
            rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
            rasterizer.lineWidth = 1.0f;
            rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
            rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            rasterizer.depthBiasEnable = VK_FALSE;

            VkPipelineMultisampleStateCreateInfo multisampling{};
            multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable = VK_TRUE;
            multisampling.rasterizationSamples = msaaSamples;
            multisampling.minSampleShading = .2f;

            VkPipelineDepthStencilStateCreateInfo depthStencil{};
            depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencil.depthTestEnable = VK_TRUE;
            depthStencil.depthWriteEnable = VK_TRUE;
            depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
            depthStencil.depthBoundsTestEnable = VK_FALSE;
            depthStencil.stencilTestEnable = VK_FALSE;

            VkPipelineColorBlendAttachmentState colorBlendAttachment{};
            colorBlendAttachment.colorWriteMask =
                    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                    VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = VK_FALSE;

            VkPipelineColorBlendStateCreateInfo colorBlending{};
            colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.logicOpEnable = VK_FALSE;
            colorBlending.logicOp = VK_LOGIC_OP_COPY;
            colorBlending.attachmentCount = 1;
            colorBlending.pAttachments = &colorBlendAttachment;
            colorBlending.blendConstants[0] = 0.0f;
            colorBlending.blendConstants[1] = 0.0f;
            colorBlending.blendConstants[2] = 0.0f;
            colorBlending.blendConstants[3] = 0.0f;

            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 1;
            pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

            if (vkCreatePipelineLayout(rhi->getDevice()->getHandle(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create pipeline layout!");
            }

            VkGraphicsPipelineCreateInfo pipelineInfo{};
            pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = 2;
            pipelineInfo.pStages = shaderStages;
            pipelineInfo.pVertexInputState = &vertexInputInfo;
            pipelineInfo.pInputAssemblyState = &inputAssembly;
            pipelineInfo.pViewportState = &viewportState;
            pipelineInfo.pRasterizationState = &rasterizer;
            pipelineInfo.pMultisampleState = &multisampling;
            pipelineInfo.pDepthStencilState = &depthStencil;
            pipelineInfo.pColorBlendState = &colorBlending;
            pipelineInfo.layout = pipelineLayout;
            pipelineInfo.renderPass = renderPass;
            pipelineInfo.subpass = 0;
            pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

            if (vkCreateGraphicsPipelines(rhi->getDevice()->getHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) !=
                VK_SUCCESS)
            {
                throw std::runtime_error("failed to create graphics pipeline!");
            }
        }

        void createFrameBuffers()
        {
            swapChainFrameBuffers.resize(swapChainImageViews.size());

            for (size_t i = 0; i < swapChainImageViews.size(); i++)
            {
                std::array<VkImageView, 3> attachments = {
                        colorImageView,
                        depthImageView,
                        swapChainImageViews[i]
                };

                VkFramebufferCreateInfo framebufferInfo{};
                framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebufferInfo.renderPass = renderPass;
                framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
                framebufferInfo.pAttachments = attachments.data();
                framebufferInfo.width = swapChainExtent.width;
                framebufferInfo.height = swapChainExtent.height;
                framebufferInfo.layers = 1;

                if (vkCreateFramebuffer(rhi->getDevice()->getHandle(), &framebufferInfo, nullptr, &swapChainFrameBuffers[i]) != VK_SUCCESS)
                {
                    throw std::runtime_error("failed to create framebuffer!");
                }
            }
        }

        void createCommandPool()
        {
            QueueFamilyIndices queueFamilyIndices = findQueueFamilies(rhi->getDevice()->getPhysicalHandle());

            VkCommandPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

            if (vkCreateCommandPool(rhi->getDevice()->getHandle(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create graphics command pool!");
            }
        }

        void createColorResources()
        {
            VkFormat colorFormat = swapChainImageFormat;
            createImage(swapChainExtent.width, swapChainExtent.height, 1, msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL,
                        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colorImage,
                        colorImageMemory);
            colorImageView = createImageView(colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
        }

        void createDepthResources()
        {
            VkFormat depthFormat = findDepthFormat();

            createImage(swapChainExtent.width, swapChainExtent.height, 1, msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL,
                        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
            depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
        }

        VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
        {
            for (VkFormat format : candidates)
            {
                VkFormatProperties props;
                vkGetPhysicalDeviceFormatProperties(rhi->getDevice()->getPhysicalHandle(), format, &props);

                if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
                {
                    return format;
                }
                else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
                {
                    return format;
                }
            }
            throw std::runtime_error("failed to find supported format!");
        }

        VkFormat findDepthFormat()
        {
            return findSupportedFormat(
                    {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
            );
        }

        void createTextureImage()
        {
            int texWidth, texHeight, texChannels;
            stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth,
                                        &texHeight, &texChannels, STBI_rgb_alpha);
            VkDeviceSize imageSize = texWidth * texHeight * 4;
            mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

            if (!pixels)
            {
                throw std::runtime_error("failed to load texture image!");
            }

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                         stagingBufferMemory);

            void* data;
            vkMapMemory(rhi->getDevice()->getHandle(), stagingBufferMemory, 0, imageSize, 0, &data);
            memcpy(data, pixels, static_cast<size_t>(imageSize));
            vkUnmapMemory(rhi->getDevice()->getHandle(), stagingBufferMemory);

            stbi_image_free(pixels);

            createImage(texWidth, texHeight, mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        textureImage, textureImageMemory);

            transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
            copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
            //transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps

            vkDestroyBuffer(rhi->getDevice()->getHandle(), stagingBuffer, nullptr);
            vkFreeMemory(rhi->getDevice()->getHandle(), stagingBufferMemory, nullptr);

            generateMipmaps(textureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels);
        }

        void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
        {
            VkFormatProperties formatProperties;
            vkGetPhysicalDeviceFormatProperties(rhi->getDevice()->getPhysicalHandle(), imageFormat, &formatProperties);

            if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
            {
                throw std::runtime_error("texture image format does not support linear blitting!");
            }

            VkCommandBuffer commandBuffer = beginSingleTimeCommands();

            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.image = image;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;
            barrier.subresourceRange.levelCount = 1;

            int32_t mipWidth = texWidth;
            int32_t mipHeight = texHeight;

            for (uint32_t i = 1; i < mipLevels; i++)
            {
                barrier.subresourceRange.baseArrayLayer = i - 1;
                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

                vkCmdPipelineBarrier(commandBuffer,
                                     VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                                     0, nullptr,
                                     0, nullptr,
                                     1, &barrier);

                VkImageBlit blit{};
                blit.srcOffsets[0] = {0, 0, 0};
                blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
                blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.srcSubresource.mipLevel = i - 1;
                blit.srcSubresource.baseArrayLayer = 0;
                blit.srcSubresource.layerCount = 1;
                blit.dstOffsets[0] = {0, 0, 0};
                blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
                blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.dstSubresource.mipLevel = i;
                blit.dstSubresource.baseArrayLayer = 0;
                blit.dstSubresource.layerCount = 1;

                vkCmdBlitImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                               image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               1, &blit,
                               VK_FILTER_LINEAR);

                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                vkCmdPipelineBarrier(commandBuffer,
                                     VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                                     0, nullptr,
                                     0, nullptr,
                                     1, &barrier);

                if (mipWidth > 1) mipWidth /= 2;
                if (mipHeight > 1) mipHeight /= 2;
            }

            barrier.subresourceRange.baseMipLevel = mipLevels - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                                 0, nullptr,
                                 0, nullptr,
                                 1, &barrier);

            endSingleTimeCommands(commandBuffer);
        }

        VkSampleCountFlagBits getMaxUsableSampleCount()
        {
            VkPhysicalDeviceProperties physicalDeviceProperties;
            vkGetPhysicalDeviceProperties(rhi->getDevice()->getPhysicalHandle(), &physicalDeviceProperties);

            VkSampleCountFlags counts =
                    physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
            if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
            if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
            if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
            if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
            if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
            if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

            return VK_SAMPLE_COUNT_1_BIT;
        }

        void createTextureImageView()
        {
            textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
        }

        void createTextureSampler()
        {
            VkPhysicalDeviceProperties properties{};
            vkGetPhysicalDeviceProperties(rhi->getDevice()->getPhysicalHandle(), &properties);

            VkSamplerCreateInfo samplerInfo{};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter = VK_FILTER_LINEAR;
            samplerInfo.minFilter = VK_FILTER_LINEAR;
            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.anisotropyEnable = VK_TRUE;
            samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
            samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            samplerInfo.unnormalizedCoordinates = VK_FALSE;
            samplerInfo.compareEnable = VK_FALSE;
            samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
            samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            samplerInfo.minLod = 0.0f;
            samplerInfo.maxLod = static_cast<float>(mipLevels);
            samplerInfo.mipLodBias = 0.0f;

            if (vkCreateSampler(rhi->getDevice()->getHandle(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create texture sampler!");
            }
        }

        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
        {
            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = image;
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = format;
            viewInfo.subresourceRange.aspectMask = aspectFlags;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = mipLevels;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            VkImageView imageView;
            if (vkCreateImageView(rhi->getDevice()->getHandle(), &viewInfo, nullptr, &imageView) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create texture image view!");
            }

            return imageView;
        }

        void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
                         VkImageUsageFlags usage,
                         VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory)
        {
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = width;
            imageInfo.extent.height = height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = mipLevels;
            imageInfo.arrayLayers = 1;
            imageInfo.format = format;
            imageInfo.tiling = tiling;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = usage;
            imageInfo.samples = numSamples;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            if (vkCreateImage(rhi->getDevice()->getHandle(), &imageInfo, nullptr, &image) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create image!");
            }

            VkMemoryRequirements memRequirements;
            vkGetImageMemoryRequirements(rhi->getDevice()->getHandle(), image, &memRequirements);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

            if (vkAllocateMemory(rhi->getDevice()->getHandle(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to allocate image memory!");
            }

            vkBindImageMemory(rhi->getDevice()->getHandle(), image, imageMemory, 0);
        }

        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
        {
            VkCommandBuffer commandBuffer = beginSingleTimeCommands();

            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.oldLayout = oldLayout;
            barrier.newLayout = newLayout;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = image;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = mipLevels;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;

            VkPipelineStageFlags sourceStage;
            VkPipelineStageFlags destinationStage;

            if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
            {
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            }
            else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                     newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
            {
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            }
            else
            {
                throw std::invalid_argument("unsupported layout transition!");
            }

            vkCmdPipelineBarrier(
                    commandBuffer,
                    sourceStage, destinationStage,
                    0,
                    0, nullptr,
                    0, nullptr,
                    1, &barrier
            );

            endSingleTimeCommands(commandBuffer);
        }

        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
        {
            VkCommandBuffer commandBuffer = beginSingleTimeCommands();

            VkBufferImageCopy region{};
            region.bufferOffset = 0;
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;
            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount = 1;
            region.imageOffset = {0, 0, 0};
            region.imageExtent = {
                    width,
                    height,
                    1
            };

            vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

            endSingleTimeCommands(commandBuffer);
        }

        void loadModel()
        {
            tinyobj::attrib_t attrib;
            std::vector<tinyobj::shape_t> shapes;
            std::vector<tinyobj::material_t> materials;
            std::string warn, err;

            if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str()))
            {
                throw std::runtime_error(warn + err);
            }

            std::unordered_map<Vertex, uint32_t> uniqueVertices{};

            for (const auto &shape : shapes)
            {
                for (const auto &index : shape.mesh.indices)
                {
                    Vertex vertex{};

                    vertex.pos = {
                            attrib.vertices[3 * index.vertex_index + 0],
                            attrib.vertices[3 * index.vertex_index + 1],
                            attrib.vertices[3 * index.vertex_index + 2]
                    };

                    vertex.texCoord = {
                            attrib.texcoords[2 * index.texcoord_index + 0],
                            1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                    };

                    vertex.color = {1.0f, 1.0f, 1.0f};

                    if (uniqueVertices.count(vertex) == 0)
                    {
                        uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                        vertices.push_back(vertex);
                    }

                    indices.push_back(uniqueVertices[vertex]);
                }
            }
        }

        void createVertexBuffer()
        {
            VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                         stagingBufferMemory);

            void* data;
            vkMapMemory(rhi->getDevice()->getHandle(), stagingBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, vertices.data(), (size_t) bufferSize);
            vkUnmapMemory(rhi->getDevice()->getHandle(), stagingBufferMemory);

            createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

            copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

            vkDestroyBuffer(rhi->getDevice()->getHandle(), stagingBuffer, nullptr);
            vkFreeMemory(rhi->getDevice()->getHandle(), stagingBufferMemory, nullptr);
        }

        void createIndexBuffer()
        {
            VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                         stagingBufferMemory);

            void* data;
            vkMapMemory(rhi->getDevice()->getHandle(), stagingBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, indices.data(), (size_t) bufferSize);
            vkUnmapMemory(rhi->getDevice()->getHandle(), stagingBufferMemory);

            createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

            copyBuffer(stagingBuffer, indexBuffer, bufferSize);

            vkDestroyBuffer(rhi->getDevice()->getHandle(), stagingBuffer, nullptr);
            vkFreeMemory(rhi->getDevice()->getHandle(), stagingBufferMemory, nullptr);
        }

        void createUniformBuffers()
        {
            VkDeviceSize bufferSize = sizeof(UniformBufferObject);

            uniformBuffers.resize(swapChainImages.size());
            uniformBuffersMemory.resize(swapChainImages.size());

            for (size_t i = 0; i < swapChainImages.size(); i++)
            {
                createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i],
                             uniformBuffersMemory[i]);
            }
        }

        void createDescriptorPool()
        {
            std::array<VkDescriptorPoolSize, 2> poolSizes{};
            poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImages.size());
            poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChainImages.size());

            VkDescriptorPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
            poolInfo.pPoolSizes = poolSizes.data();
            poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size());

            if (vkCreateDescriptorPool(rhi->getDevice()->getHandle(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create descriptor pool!");
            }
        }

        void createDescriptorSets()
        {
            std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(), descriptorSetLayout);
            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = descriptorPool;
            allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
            allocInfo.pSetLayouts = layouts.data();

            descriptorSets.resize(swapChainImages.size());
            if (vkAllocateDescriptorSets(rhi->getDevice()->getHandle(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to allocate descriptor sets!");
            }

            for (size_t i = 0; i < swapChainImages.size(); i++)
            {
                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = uniformBuffers[i];
                bufferInfo.offset = 0;
                bufferInfo.range = sizeof(UniformBufferObject);

                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = textureImageView;
                imageInfo.sampler = textureSampler;

                std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

                descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[0].dstSet = descriptorSets[i];
                descriptorWrites[0].dstBinding = 0;
                descriptorWrites[0].dstArrayElement = 0;
                descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrites[0].descriptorCount = 1;
                descriptorWrites[0].pBufferInfo = &bufferInfo;

                descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[1].dstSet = descriptorSets[i];
                descriptorWrites[1].dstBinding = 1;
                descriptorWrites[1].dstArrayElement = 0;
                descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorWrites[1].descriptorCount = 1;
                descriptorWrites[1].pImageInfo = &imageInfo;

                vkUpdateDescriptorSets(rhi->getDevice()->getHandle(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0,
                                       nullptr);
            }
        }

        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,
                          VkDeviceMemory &bufferMemory)
        {
            VkBufferCreateInfo bufferInfo{};
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size = size;
            bufferInfo.usage = usage;
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            if (vkCreateBuffer(rhi->getDevice()->getHandle(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create buffer!");
            }

            VkMemoryRequirements memRequirements;
            vkGetBufferMemoryRequirements(rhi->getDevice()->getHandle(), buffer, &memRequirements);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

            if (vkAllocateMemory(rhi->getDevice()->getHandle(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to allocate buffer memory!");
            }

            vkBindBufferMemory(rhi->getDevice()->getHandle(), buffer, bufferMemory, 0);
        }

        VkCommandBuffer beginSingleTimeCommands()
        {
            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandPool = commandPool;
            allocInfo.commandBufferCount = 1;

            VkCommandBuffer commandBuffer;
            vkAllocateCommandBuffers(rhi->getDevice()->getHandle(), &allocInfo, &commandBuffer);

            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            vkBeginCommandBuffer(commandBuffer, &beginInfo);

            return commandBuffer;
        }

        void endSingleTimeCommands(VkCommandBuffer commandBuffer)
        {
            vkEndCommandBuffer(commandBuffer);

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;

            vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
            vkQueueWaitIdle(graphicsQueue);

            vkFreeCommandBuffers(rhi->getDevice()->getHandle(), commandPool, 1, &commandBuffer);
        }

        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
        {
            VkCommandBuffer commandBuffer = beginSingleTimeCommands();

            VkBufferCopy copyRegion{};
            copyRegion.size = size;
            vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

            endSingleTimeCommands(commandBuffer);
        }

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
        {
            VkPhysicalDeviceMemoryProperties memProperties;
            vkGetPhysicalDeviceMemoryProperties(rhi->getDevice()->getPhysicalHandle(), &memProperties);

            for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
            {
                if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
                {
                    return i;
                }
            }

            throw std::runtime_error("failed to find suitable memory type!");
        }

        void createCommandBuffers()
        {
            commandBuffers.resize(swapChainFrameBuffers.size());

            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool = commandPool;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

            if (vkAllocateCommandBuffers(rhi->getDevice()->getHandle(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to allocate command buffers!");
            }

            for (size_t i = 0; i < commandBuffers.size(); i++)
            {
                VkCommandBufferBeginInfo beginInfo{};
                beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

                if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
                {
                    throw std::runtime_error("failed to begin recording command buffer!");
                }

                VkRenderPassBeginInfo renderPassInfo{};
                renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                renderPassInfo.renderPass = renderPass;
                renderPassInfo.framebuffer = swapChainFrameBuffers[i];
                renderPassInfo.renderArea.offset = {0, 0};
                renderPassInfo.renderArea.extent = swapChainExtent;

                std::array<VkClearValue, 2> clearValues{};
                clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
                clearValues[1].depthStencil = {1.0f, 0};

                renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
                renderPassInfo.pClearValues = clearValues.data();

                vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

                vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

                VkBuffer vertexBuffers[] = {vertexBuffer};
                VkDeviceSize offsets[] = {0};
                vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);

                vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT32);

                vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                                        &descriptorSets[i], 0, nullptr);

                vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

                vkCmdEndRenderPass(commandBuffers[i]);

                if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
                {
                    throw std::runtime_error("failed to record command buffer!");
                }
            }
        }

        void createSyncObjects()
        {
            imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
            renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
            inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
            imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            VkFenceCreateInfo fenceInfo{};
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                if (vkCreateSemaphore(rhi->getDevice()->getHandle(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                    vkCreateSemaphore(rhi->getDevice()->getHandle(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                    vkCreateFence(rhi->getDevice()->getHandle(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
                {
                    throw std::runtime_error("failed to create synchronization objects for a frame!");
                }
            }
        }

        void updateUniformBuffer(uint32_t currentImage)
        {
            static auto startTime = std::chrono::high_resolution_clock::now();

            auto currentTime = std::chrono::high_resolution_clock::now();
            float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

            UniformBufferObject ubo{};
            ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            ubo.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float) swapChainExtent.height, 0.1f, 10.0f);
            ubo.proj[1][1] *= -1;

            void* data;
            vkMapMemory(rhi->getDevice()->getHandle(), uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
            memcpy(data, &ubo, sizeof(ubo));
            vkUnmapMemory(rhi->getDevice()->getHandle(), uniformBuffersMemory[currentImage]);
        }

        void drawFrame()
        {
            vkWaitForFences(rhi->getDevice()->getHandle(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

            uint32_t imageIndex;
            VkResult result = vkAcquireNextImageKHR(rhi->getDevice()->getHandle(), swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame],
                                                    VK_NULL_HANDLE, &imageIndex);

            if (result == VK_ERROR_OUT_OF_DATE_KHR)
            {
                recreateSwapChain();
                return;
            }
            else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            {
                throw std::runtime_error("failed to acquire swap chain image!");
            }

            updateUniformBuffer(imageIndex);

            if (imagesInFlight[imageIndex] != VK_NULL_HANDLE)
            {
                vkWaitForFences(rhi->getDevice()->getHandle(), 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
            }
            imagesInFlight[imageIndex] = inFlightFences[currentFrame];

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

            VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
            VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;

            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

            VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = signalSemaphores;

            vkResetFences(rhi->getDevice()->getHandle(), 1, &inFlightFences[currentFrame]);

            if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to submit draw command buffer!");
            }

            VkPresentInfoKHR presentInfo{};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = signalSemaphores;

            VkSwapchainKHR swapChains[] = {swapChain};
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = swapChains;

            presentInfo.pImageIndices = &imageIndex;

            result = vkQueuePresentKHR(presentQueue, &presentInfo);

            if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
            {
                framebufferResized = false;
                recreateSwapChain();
            }
            else if (result != VK_SUCCESS)
            {
                throw std::runtime_error("failed to present swap chain image!");
            }

            currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        }

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
        {
            for (const auto &availableFormat : availableFormats)
            {
                if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                    availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                {
                    return availableFormat;
                }
            }

            return availableFormats[0];
        }

        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
        {
            for (const auto &availablePresentMode : availablePresentModes)
            {
                if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                {
                    return availablePresentMode;
                }
            }

            return VK_PRESENT_MODE_FIFO_KHR;
        }

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
        {
            if (capabilities.currentExtent.width != UINT32_MAX)
            {
                return capabilities.currentExtent;
            }
            else
            {
                int width, height;
                glfwGetFramebufferSize(mWindow, &width, &height);

                VkExtent2D actualExtent = {
                        static_cast<uint32_t>(width),
                        static_cast<uint32_t>(height)
                };

                actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                                                capabilities.maxImageExtent.width);
                actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                                                 capabilities.maxImageExtent.height);

                return actualExtent;
            }
        }

        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device)
        {
            SwapChainSupportDetails details;

            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

            uint32_t formatCount;
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

            if (formatCount != 0)
            {
                details.formats.resize(formatCount);
                vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
            }

            uint32_t presentModeCount;
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

            if (presentModeCount != 0)
            {
                details.presentModes.resize(presentModeCount);
                vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
            }

            return details;
        }

        bool isDeviceSuitable(VkPhysicalDevice device)
        {
            QueueFamilyIndices indices = findQueueFamilies(device);

            bool extensionsSupported = checkDeviceExtensionSupport(device);

            bool swapChainAdequate = false;
            if (extensionsSupported)
            {
                SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
                swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
            }

            VkPhysicalDeviceFeatures supportedFeatures;
            vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

            return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
        }

        bool checkDeviceExtensionSupport(VkPhysicalDevice device)
        {
            uint32_t extensionCount;
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

            std::vector<VkExtensionProperties> availableExtensions(extensionCount);
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

            std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

            for (const auto &extension : availableExtensions)
            {
                requiredExtensions.erase(extension.extensionName);
            }

            return requiredExtensions.empty();
        }

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device)
        {
            QueueFamilyIndices indices;

            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

            std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

            int i = 0;
            for (const auto &queueFamily : queueFamilies)
            {
                if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    indices.graphicsFamily = i;
                }

                VkBool32 presentSupport = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

                if (presentSupport)
                {
                    indices.presentFamily = i;
                }

                if (indices.isComplete())
                {
                    break;
                }

                i++;
            }

            return indices;
        }

        VkDebugUtilsMessengerEXT debugMessenger;
        VkSurfaceKHR surface;
        
        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

        VkQueue graphicsQueue;
        VkQueue presentQueue;

        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::vector<VkImageView> swapChainImageViews;
        std::vector<VkFramebuffer> swapChainFrameBuffers;

        VkRenderPass renderPass;
        VkDescriptorSetLayout descriptorSetLayout;
        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;

        VkCommandPool commandPool;
        std::vector<VkCommandBuffer> commandBuffers;

        VkImage colorImage;
        VkDeviceMemory colorImageMemory;
        VkImageView colorImageView;

        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;

        uint32_t mipLevels;
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView;
        VkSampler textureSampler;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        VkBuffer vertexBuffer;
        VkBuffer indexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkDeviceMemory indexBufferMemory;

        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;

        VkDescriptorPool descriptorPool;
        std::vector<VkDescriptorSet> descriptorSets;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;
        size_t currentFrame = 0;

        bool framebufferResized = false;

        std::shared_ptr<VulkanRHI> rhi;
    };
}


int main()
{
    Homura::TriangleApplication app;
    try
    {
        app.init();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
