#pragma once

#include <GLFW/glfw3.h>
#include <vector>

#include "Vulkan.h"
#include "VulkanDevice.h"

#include "Log.h"


class VulkanRenderer {

public:
	VulkanRenderer(GLFWwindow* nativeWindow);
	~VulkanRenderer() = default;
	void Init();
	void Cleanup();
	
	inline static VkInstance GetInstance() { return s_VulkanInstance; }
	std::shared_ptr<VulkanDevice> GetDevice() { return m_Device; }
private:
	GLFWwindow* m_Window;
	
	// Vulkan instance
	inline static VkInstance s_VulkanInstance;
	void createInstance();
	bool checkInstanceExtensionSupport(std::vector<const char*>* extensionsToCheck);


	// Devices
	std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
	std::shared_ptr<VulkanDevice> m_Device;

	
};