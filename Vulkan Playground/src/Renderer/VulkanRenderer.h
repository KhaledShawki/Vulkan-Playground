#pragma once

#include <GLFW/glfw3.h>
#include <vector>
#include "Vulkan.h"
#include "Log.h"

class VulkanRenderer {

public:
	VulkanRenderer(GLFWwindow* nativeWindow);
	~VulkanRenderer() = default;
	void Init();
	void Cleanup();

private:
	GLFWwindow* m_Window;
	
	// Vulkan instance
	VkInstance m_VulkanInstance;
	void createInstance();
	bool checkInstanceExtensionSupport(std::vector<const char*>* extensionsToCheck);
	
};