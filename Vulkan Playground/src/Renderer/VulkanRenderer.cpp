#include "VulkanRenderer.h"


VulkanRenderer::VulkanRenderer(GLFWwindow* nativeWindow)
{
	m_Window = nativeWindow;
}

void VulkanRenderer::Init()
{
	
	CreateInstance();

	m_PhysicalDevice = VulkanPhysicalDevice::Select();
	VkPhysicalDeviceFeatures enabledFeatures{};
	m_Device = std::make_shared<VulkanDevice>(m_PhysicalDevice, enabledFeatures);
	
	CreateSurface();
} 

void VulkanRenderer::Cleanup()
{
	vkDestroySurfaceKHR(s_VulkanInstance, m_Surface, nullptr);
	m_Device->Cleanup();
	vkDestroyInstance(s_VulkanInstance, nullptr);
}

void VulkanRenderer::CreateInstance()
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Application Info
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	VkApplicationInfo applicationInfo = {};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pApplicationName = "Vulkan test";
	applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.pEngineName = "No Engine";
	applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.apiVersion = VK_VERSION_1_2;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Extensions and Validation
	/////////////////////////////////////////////////////////////////
	std::vector<const char*> instanceExtensions = std::vector<const char*>();

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	// Get GLFW extensions
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	// Add GLFW extensions to list 
	for (size_t i = 0; i < glfwExtensionCount; i++)
	{
		instanceExtensions.push_back(glfwExtensions[i]);
	}

	ASSERT(CheckInstanceExtensionSupport(&instanceExtensions), "Required extensions are not supported!");
	

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Instance Info
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &applicationInfo;
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
	createInfo.ppEnabledExtensionNames = instanceExtensions.data();


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Instance Creation
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	VK_CHECK_RESULT(vkCreateInstance(&createInfo, nullptr, &s_VulkanInstance));

}

bool VulkanRenderer::CheckInstanceExtensionSupport(std::vector<const char*>* extensionsToCheck)
{
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> instanceExtensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, instanceExtensions.data());

	for (const auto& ext : *extensionsToCheck)
	{
		bool hasExtension = false;
		for (const auto& iExt : instanceExtensions)
		{
			if (strcmp(ext, iExt.extensionName))
			{
				hasExtension = true;
				break;
			}
		}

		if (!hasExtension)
		{
			return false;
		}
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Surface Creation
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void VulkanRenderer::CreateSurface()
{
	VK_CHECK_RESULT(glfwCreateWindowSurface(s_VulkanInstance, m_Window, nullptr, &m_Surface));
	// Get available queue family properties
	uint32_t queueCount;
	vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice->GetVulkanPhysicalDevice(), &queueCount, nullptr);
	ASSERT(queueCount >= 1);

	std::vector<VkQueueFamilyProperties> queueProps(queueCount);
	vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice->GetVulkanPhysicalDevice(), &queueCount, queueProps.data());

	// Search for a graphics and a present queue in the array of queue families, 
	// try to find one that supports both
	for (uint32_t i = 0; i < queueCount; i++)
	{
		VkBool32 presentSupport = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice->GetVulkanPhysicalDevice(), i, m_Surface, &presentSupport);
		if (presentSupport == VK_TRUE)
		{
			m_PresentQueueIndex = i;
			if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
			{
				break;
			}
		}
	}
}



