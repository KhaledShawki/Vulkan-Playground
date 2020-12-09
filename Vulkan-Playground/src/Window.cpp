#include "Window.h"
#include "Log.h"

Window::Window(std::string title, const unsigned int width, const unsigned int height)
{

	// Initialize GLFW
	glfwInit();

	// Set GLFW to not work with OpenGL
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	
	m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

}



std::shared_ptr<Window> Window::GetWindow()
{
	if (!s_Window)
	{
		s_Window = std::make_shared<Window>("Vulkan Playground", 1200, 720);
	}
	return s_Window;
}

bool Window::ShouldClose()
{
	return glfwWindowShouldClose(m_Window);
}

void Window::OnUpdate()
{
	glfwPollEvents();
}

void Window::Shutdown()
{
	glfwDestroyWindow(m_Window);
	CORE_INFO("Terminating GLFW");
	glfwTerminate();
}
