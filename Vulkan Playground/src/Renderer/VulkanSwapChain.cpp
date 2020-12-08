#include "VulkanSwapchain.h"
#include "Log.h"

#include "VulkanContext.h"
#include "Window.h"

void VulkanSwapchain::Init(VkInstance vulkanInstance, std::shared_ptr<VulkanPhysicalDevice>& physicalDevice)
{
	m_Instance = vulkanInstance;
	m_PhysicalDevice = physicalDevice;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Surface Creation
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void VulkanSwapchain::InitSurface(GLFWwindow* window)
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SwapChain Creation
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void VulkanSwapchain::CreateSwapchain(bool vsync)
{
	m_Device = VulkanContext::GetVulkanContext()->GetDevice()->GetVulkanDevice();

	VkSwapchainKHR oldSwapchain = m_Swapchain;

	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice->GetVulkanPhysicalDevice(), m_Surface, &capabilities);

	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice->GetVulkanPhysicalDevice(), m_Surface, &formatCount, nullptr);
	ASSERT(formatCount > 0);
	std::vector<VkSurfaceFormatKHR> formats(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice->GetVulkanPhysicalDevice(), m_Surface, &formatCount, formats.data());

	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice->GetVulkanPhysicalDevice(), m_Surface, &presentModeCount, nullptr);
	ASSERT(presentModeCount > 0);
	std::vector<VkPresentModeKHR> presentModes(presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice->GetVulkanPhysicalDevice(), m_Surface, &presentModeCount, presentModes.data());

	if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
	{
		m_Format.format = VK_FORMAT_R8G8B8A8_UNORM;
		m_Format.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	}
	else
	{
		bool isFormatFound = false;
		for (const auto& format : formats)
		{
			if (format.format == VK_FORMAT_R8G8B8A8_UNORM || format.format == VK_FORMAT_B8G8R8A8_UNORM)
			{
				m_Format = format;
				isFormatFound = true;
				break;
			}
		}
		if (!isFormatFound)
		{
			m_Format = formats[0];
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Select a present mode for the swapchain
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// The VK_PRESENT_MODE_FIFO_KHR mode must always be present as per spec
	// This mode waits for the vertical blank ("v-sync")
	VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

	// If v-sync is not requested, try to find a mailbox mode
	// It's the lowest latency non-tearing present mode available
	if (!vsync)
	{
		for (auto& presentMode : presentModes)
		{
			if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}
			swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		}
	}
	VkExtent2D swapchainExtent{};
	// If current extent is at numeric limits, then extent can vary. Otherwise, it is the size of the window.
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		swapchainExtent = capabilities.currentExtent;
	}
	else
	{
		int width, height;
		glfwGetFramebufferSize((GLFWwindow*)Window::GetWindow()->GetNativeWindow(), &width, &height);

		swapchainExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

		swapchainExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, swapchainExtent.width));
		swapchainExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, swapchainExtent.height));
	}
	m_Width = swapchainExtent.width;
	m_Height = swapchainExtent.height;

	uint32_t imageCount = capabilities.minImageCount + 1;
	// capabilities.maxImageCount > 0 => limitless
	if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
	{
		imageCount = capabilities.maxImageCount;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// SwapChain Create Info
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	VkSwapchainCreateInfoKHR swapchainCreateInfo{};
	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.surface = m_Surface;
	swapchainCreateInfo.imageFormat = m_Format.format;
	swapchainCreateInfo.imageColorSpace = m_Format.colorSpace;
	swapchainCreateInfo.imageExtent = { swapchainExtent.width, swapchainExtent.height };
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainCreateInfo.preTransform = capabilities.currentTransform;
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.presentMode = swapchainPresentMode;
	swapchainCreateInfo.minImageCount = imageCount;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // No blending
	swapchainCreateInfo.clipped = VK_TRUE;
	swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainCreateInfo.queueFamilyIndexCount = 0;
	swapchainCreateInfo.pQueueFamilyIndices = nullptr;
	swapchainCreateInfo.oldSwapchain = oldSwapchain;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// SwapChain Creation
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	vkCreateSwapchainKHR(m_Device, &swapchainCreateInfo, nullptr, &m_Swapchain);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Old SwapChain destroying 
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (oldSwapchain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(m_Device, oldSwapchain, nullptr);
	}
}

void VulkanSwapchain::Cleanup()
{

	CORE_INFO("Destroying vulkan swapchain.");
	vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
}
