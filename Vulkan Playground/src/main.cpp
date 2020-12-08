#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

#include "Window.h"
#include "Log.h"

#include "Renderer/VulkanContext.h"


int main()
{
	// Initialize Logger
	Log::Init();
	
	std::shared_ptr<Window> window = Window::GetWindow();
	std::shared_ptr<VulkanContext> vulkanContext = VulkanContext::GetVulkanContext();
	

	vulkanContext->Init();
	
	while (!window->ShouldClose())
	{
		window->OnUpdate();
	}
	vulkanContext->Cleanup();
	window->Shutdown();

	return 0;
}