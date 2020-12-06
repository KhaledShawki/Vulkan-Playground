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
	void CreateInstance();
	bool CheckInstanceExtensionSupport(std::vector<const char*>* extensionsToCheck);


	// Devices
	std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
	std::shared_ptr<VulkanDevice> m_Device;

	// Surface
	// TODO: move surface to separated class
	VkSurfaceKHR m_Surface;
	void CreateSurface();
	int32_t m_PresentQueueIndex = -1;
};