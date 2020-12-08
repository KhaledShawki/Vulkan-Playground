#pragma once

#include <GLFW/glfw3.h>
#include <vector>

#include "Vulkan.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"


#include "Log.h"


class VulkanContext {

public:
	VulkanContext(GLFWwindow* nativeWindow);
	~VulkanContext() = default;
	void Init();
	void Cleanup();
	
	static std::shared_ptr<VulkanContext> GetVulkanContext();

	inline static VkInstance GetInstance() { return s_VulkanInstance; }
	std::shared_ptr<VulkanDevice> GetDevice() const { return m_Device; }
	std::shared_ptr<VulkanSwapchain> GetVulkanSwapchain() const { return m_VulkanSwapchain; }
private:
	GLFWwindow* m_Window;
	
	inline static std::shared_ptr<VulkanContext> s_VulkanContext;
	// Vulkan instance
	inline static VkInstance s_VulkanInstance;
	void CreateInstance();
	bool CheckInstanceExtensionSupport(std::vector<const char*>* extensionsToCheck);


	// Devices
	std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
	std::shared_ptr<VulkanDevice> m_Device;

	// Surface and SwapChain
	std::shared_ptr<VulkanSwapchain> m_VulkanSwapchain;
};