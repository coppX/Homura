#include <applicationWindow.h>
#include <vulkanRHI.h>
#include <iostream>

namespace Homura
{
	ApplicationWindow::ApplicationWindow(VulkanRHIPtr rhi, int width, int height)
		: mWindow{ nullptr }
		, mWidth{ width }
		, mHeight{ height }
		, mRHI{ rhi }
	{
		glfwInit();
	}

	ApplicationWindow::~ApplicationWindow()
	{

	}

	void ApplicationWindow::create(std::string title)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, true);

		mWindow = glfwCreateWindow(mWidth, mHeight, title.c_str(), nullptr, nullptr);
		if (!mWindow)
		{
			std::cout << "failed to create GLFW window" << std::endl;
			glfwTerminate();
		}
		glfwSetWindowUserPointer(mWindow, mRHI.get());
		glfwSetMouseButtonCallback(mWindow, mouseButtonCallback);
		glfwSetFramebufferSizeCallback(mWindow, framebufferResizeCallback);
	}

	void ApplicationWindow::destroy()
	{
		glfwDestroyWindow(mWindow);
		glfwTerminate();
	}

	void ApplicationWindow::mouseButtonCallback(GLFWwindow* window, int button, int state, int mods)
	{
		auto rhi = reinterpret_cast<VulkanRHI*>(glfwGetWindowUserPointer(window));
		if (rhi->mMouseCallback)
		{
			rhi->mMouseCallback(button, state, mods);
		}
	}

	void ApplicationWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto rhi = reinterpret_cast<VulkanRHI*>(glfwGetWindowUserPointer(window));
		if (rhi->mFramebufferResizeCallback)
		{
			rhi->mFramebufferResizeCallback(width, height);
		}
	}
}