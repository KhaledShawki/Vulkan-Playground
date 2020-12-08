#include "VulkanSwapChain.h"
#include "Log.h"

void VulkanSwapChain::Init(VkInstance vulkanInstance, std::shared_ptr<VulkanPhysicalDevice>& physicalDevice)
{
	m_Instance = vulkanInstance;
	m_PhysicalDevice = physicalDevice;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Surface Creation
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void VulkanSwapChain::InitSurface(GLFWwindow* window)
{
	VkPhysicalDevice physicalDevice = m_PhysicalDevice->GetVulkanPhysicalDevice();
	VK_CHECK_RESULT(glfwCreateWindowSurface(m_Instance, window, nullptr, &m_Surface));
	// Get available queue family properties
	uint32_t queueCount;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, nullptr);
	ASSERT(queueCount >= 1);

	std::vector<VkQueueFamilyProperties> queueProps(queueCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, queueProps.data());

	// Search for a graphics and a present queue in the array of queue families, 
	// try to find one that supports both
	for (uint32_t i = 0; i < queueCount; i++)
	{
		VkBool32 presentSupport = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, m_Surface, &presentSupport);
		if (presentSupport == VK_TRUE)
		{
			m_PresentQueueIndex = i;
			if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
			{
				break;
			}
		}
	}
	CORE_INFO("Present queue index: {0}", m_PresentQueueIndex);
}

void VulkanSwapChain::CreateSwapChain()
{
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice->GetVulkanPhysicalDevice(), m_Surface, &m_SwapChainSupportDetails.Capabilities);

	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice->GetVulkanPhysicalDevice(), m_Surface, &formatCount, nullptr);
	ASSERT(formatCount > 0);
	m_SwapChainSupportDetails.Formats.resize(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice->GetVulkanPhysicalDevice(), m_Surface, &formatCount, m_SwapChainSupportDetails.Formats.data());

	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice->GetVulkanPhysicalDevice(), m_Surface, &presentModeCount, nullptr);
	ASSERT(presentModeCount > 0);
	m_SwapChainSupportDetails.PresentModes.resize(presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice->GetVulkanPhysicalDevice(), m_Surface, &presentModeCount, m_SwapChainSupportDetails.PresentModes.data());
}
