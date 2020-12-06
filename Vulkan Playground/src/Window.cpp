#include "Window.h"
#include "Log.h"

Window::Window(std::string title, const unsigned int width, const unsigned int height)
{

	// Initialise GLFW
	glfwInit();

	// Set GLFW to not work with OpenGL
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	
	m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

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
