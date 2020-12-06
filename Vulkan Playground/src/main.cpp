#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

#include "Window.h"
#include "Log.h"

#include "Renderer/VulkanRenderer.h"


int main()
{
	// Initialise Logger
	Log::Init();
	
	Window* window = new Window("Vulkan test", 1200, 720);
	VulkanRenderer vkRenderer((GLFWwindow*)window->GetNativeWindow());

	vkRenderer.Init();
	
	while (!window->ShouldClose())
	{
		window->OnUpdate();
	}
	vkRenderer.Cleanup();
	window->Shutdown();

	delete window;
	return 0;
}