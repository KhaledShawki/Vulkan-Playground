#pragma once

#include "core.h"
#include "Vulkan.h"
#include "GLFW/glfw3.h"
#include "VulkanDevice.h"

class VulkanSwapChain
{
public:
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR Capabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> PresentModes;
	};

public:
	VulkanSwapChain() = default;
	~VulkanSwapChain() = default;

	void Init(VkInstance vulkanInstance, std::shared_ptr<VulkanPhysicalDevice>& physicalDevice);
	void InitSurface(GLFWwindow* window);
	void CreateSwapChain();

	inline int32_t GetPresentQueueIndex() { return m_PresentQueueIndex; }
	inline SwapChainSupportDetails GetSwapChainDetails() { return m_SwapChainSupportDetails; }
private:
	VkInstance m_Instance;
	std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
	VkSurfaceKHR m_Surface;
	int32_t m_PresentQueueIndex = -1;

	SwapChainSupportDetails m_SwapChainSupportDetails;

};