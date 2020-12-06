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
	
	inline static VkInstance GetInstance() { return s_VulkanInstance; }

private:
	GLFWwindow* m_Window;
	
	// Vulkan instance
	static VkInstance s_VulkanInstance;
	void createInstance();
	bool checkInstanceExtensionSupport(std::vector<const char*>* extensionsToCheck);

	// Physical Device
	std::shared_ptr<VkPhysicalDevice> m_PhysicalDevice;
	

	// Logical Device
	VkDevice m_Device;
	
};