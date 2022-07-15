#ifndef HOMURA_WINDOW_H
#define HOMURA_WINDOW_H
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkanTypes.h>
#include <string>
//
//struct GLFW;
//struct GLFWwindow;

namespace Homura
{
	class ApplicationWindow
	{
	public:
		ApplicationWindow(VulkanRHIPtr rhi, int width, int height);
		~ApplicationWindow();

		void create(std::string title);
		void destroy();

		GLFWwindow* getHandle()
		{
			return mWindow;
		}

		int getWidth()
		{
			return mWidth;
		}

		int getHeight()
		{
			return mHeight;
		}

		bool shouldClose()
		{
			return glfwWindowShouldClose(mWindow);
		}

		void processInput()
		{
			if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			{
				glfwSetWindowShouldClose(mWindow, true);
			}
			glfwPollEvents();
		}

		static void mouseButtonCallback(GLFWwindow* window, int button, int state, int mods);
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	private:
		GLFWwindow* mWindow;
		int mWidth;
		int mHeight;
		VulkanRHIPtr mRHI;
	};
}
#endif