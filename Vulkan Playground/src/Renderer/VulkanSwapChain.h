#pragma once

#include "core.h"
#include "Vulkan.h"
#include "GLFW/glfw3.h"
#include "VulkanDevice.h"

#include <algorithm>

class VulkanSwapchain
{
public:
	VulkanSwapchain() = default;
	~VulkanSwapchain() = default;

	void Init(VkInstance vulkanInstance, std::shared_ptr<VulkanPhysicalDevice>& physicalDevice);
	void InitSurface(GLFWwindow* window);
	void CreateSwapchain(bool vsync);
	void Cleanup();

	inline int32_t GetPresentQueueIndex() { return m_PresentQueueIndex; }

private:
	struct SwapchainImage {
		VkImage image;
		VkImageView imageView;
	};

private:
	VkInstance m_Instance;
	std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
	VkDevice  m_Device; 
	VkSurfaceKHR m_Surface;
	int32_t m_PresentQueueIndex = -1;
	
	VkQueue m_PresentQueue;
	VkSurfaceFormatKHR m_Format{};
	
	uint32_t m_Width;
	uint32_t m_Height;

	VkSwapchainKHR m_Swapchain;

	uint32_t m_ImageCount;
	
	std::vector< SwapchainImage> m_SwapchainImages;

};