#include "VulkanDevice.h"
#include "VulkanContext.h"

////////////////////////////////////////////////////////////////////////////////////
// Vulkan Physical Device
////////////////////////////////////////////////////////////////////////////////////
VulkanPhysicalDevice::VulkanPhysicalDevice()
{
	auto vkInstance = VulkanContext::GetInstance();

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);

	ASSERT(deviceCount > 0, "Can't finde GPUs that support Vulkan Instance!");

	std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    VK_CHECK_RESULT(vkEnumeratePhysicalDevices(vkInstance, &deviceCount, physicalDevices.data()));

	VkPhysicalDevice selectedPhysicalDevice = nullptr;
	
	for (VkPhysicalDevice physicalDevice : physicalDevices)
	{
		vkGetPhysicalDeviceProperties(physicalDevice, &m_Properties);

		if (m_Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			selectedPhysicalDevice = physicalDevice;
			break;
		}
	}
	if (!selectedPhysicalDevice)
	{
		CORE_TRACE("Could not find discrete GPU.");
		selectedPhysicalDevice = physicalDevices.back();
	}

	ASSERT(selectedPhysicalDevice, "Could not find any physical devices!");
	m_PhysicalDevice = selectedPhysicalDevice;


	vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &m_Features);

	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, nullptr);
	ASSERT(queueFamilyCount > 0);
	m_QueueFamilyProperties.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, m_QueueFamilyProperties.data());

	int requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
	const float defaultQueuePriority(0.0f);
	m_QueueFamilyIndices = GetQueueFamilyIndices(requestedQueueTypes);
	// Graphic queue
	if (requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT)
	{
		VkDeviceQueueCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		createInfo.queueFamilyIndex = m_QueueFamilyIndices.Graphic;
		createInfo.queueCount = 1;
		createInfo.pQueuePriorities = &defaultQueuePriority;
		m_QueueCreateInfos.push_back(createInfo);
	}
	
	// Dedicated compute queue
	if (requestedQueueTypes & VK_QUEUE_COMPUTE_BIT)
	{
		if (m_QueueFamilyIndices.Compute != m_QueueFamilyIndices.Graphic)
		{
			// If compute family index differs, we need an additional queue create info for the compute queue
			VkDeviceQueueCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			createInfo.queueFamilyIndex = m_QueueFamilyIndices.Compute;
			createInfo.queueCount = 1;
			createInfo.pQueuePriorities = &defaultQueuePriority;
			m_QueueCreateInfos.push_back(createInfo);
		}
	}

	// Dedicated transfer queue
	if (requestedQueueTypes & VK_QUEUE_TRANSFER_BIT)
	{
		if (m_QueueFamilyIndices.Transfer != m_QueueFamilyIndices.Graphic && (m_QueueFamilyIndices.Transfer != m_QueueFamilyIndices.Compute))
		{
			// If transfer family index differs, we need an additional queue create info for the compute queue
			VkDeviceQueueCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			createInfo.queueFamilyIndex = m_QueueFamilyIndices.Transfer;
			createInfo.queueCount = 1;
			createInfo.pQueuePriorities = &defaultQueuePriority;
			m_QueueCreateInfos.push_back(createInfo);
		}
	}

	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, nullptr);
	if (extensionCount > 0)
	{
		std::vector<VkExtensionProperties> deviceExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, deviceExtensions.data());

		CORE_TRACE("Supported extensions :");
		for (auto& ext : deviceExtensions)
		{
			m_SupportedExtensions.emplace(ext.extensionName);
			CORE_TRACE("\t{0}", ext.extensionName);
		}
	}

}

bool VulkanPhysicalDevice::IsExtensionSupported(const std::string& extensionName) const
{
    return m_SupportedExtensions.find(extensionName) != m_SupportedExtensions.end();
}

VulkanPhysicalDevice::QueueFamilyIndices VulkanPhysicalDevice::GetQueueFamilyIndices(int flags)
{
	QueueFamilyIndices indices;

	// Dedicated queue for compute
	// Try to find an index for the queue family that supports compute but not graphics
	if (flags & VK_QUEUE_COMPUTE_BIT)
	{
		for (uint32_t  i = 0; i < m_QueueFamilyProperties.size(); i++)
		{
			auto& queueFamilyProperties = m_QueueFamilyProperties[i];
			if ((queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) && ((queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
			{
				indices.Compute = i;
				break;
			}
		}
	}
	// Dedicated queue for transfer
	// Try to find an index for the queue family that supports transfer, but no graphics and compute.
	if (flags & VK_QUEUE_TRANSFER_BIT)
	{
		for (uint32_t  i = 0; i < m_QueueFamilyProperties.size(); i++)
		{
			auto& queueFamilyProperties = m_QueueFamilyProperties[i];
			if ((queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT) && ((queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) == 0) && ((queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT ) == 0))
			{
				indices.Transfer = i;
				break;
			}
		}
	}
	// For other queue types or if there is no separate compute or transfer queue, return the first one that supports the requested flags
	for (uint32_t i = 0; i < m_QueueFamilyProperties.size(); i++)
	{
		auto& queueFamilyProperties = m_QueueFamilyProperties[i];

		if (flags & VK_QUEUE_GRAPHICS_BIT)
		{
			if ((queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT))
			{
				indices.Graphic = i;
			}
		}

		if (flags & VK_QUEUE_COMPUTE_BIT && (indices.Compute == -1))
		{
			if ((queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT))
			{
				indices.Compute = i;
			}
		}

		if (flags & VK_QUEUE_TRANSFER_BIT && (indices.Compute == -1))
		{
			if ((queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT))
			{
				indices.Transfer = i;
			}
		}
	}

	CORE_INFO("Graphic Queu index: {0}", indices.Graphic);
	CORE_INFO("Compute Queu index: {0}", indices.Compute);
	CORE_INFO("Transfer Queu index: {0}", indices.Transfer);
	return indices;
}

std::shared_ptr<VulkanPhysicalDevice> VulkanPhysicalDevice::Select()
{
    return std::make_shared<VulkanPhysicalDevice>();
}


////////////////////////////////////////////////////////////////////////////////////
// Vulkan Logical Device
////////////////////////////////////////////////////////////////////////////////////
VulkanDevice::VulkanDevice(const std::shared_ptr<VulkanPhysicalDevice>& physicalDevice, VkPhysicalDeviceFeatures enabledFeatures)
	:m_PhysicalDevice(physicalDevice), m_EnabledFeatures(enabledFeatures)
{
	// Present queue
	// Add the present queue index if it does not have the same index as the graphics, transfer or compute queue index
	{
		const float defaultQueuePriority(0.0f);

		int32_t presentQueueIndex = VulkanContext::GetVulkanContext()->GetVulkanSwapchain()->GetPresentQueueIndex();
		if (presentQueueIndex != m_PhysicalDevice->m_QueueFamilyIndices.Graphic && presentQueueIndex != m_PhysicalDevice->m_QueueFamilyIndices.Compute && presentQueueIndex != m_PhysicalDevice->m_QueueFamilyIndices.Transfer)
		{
			VkDeviceQueueCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			createInfo.queueFamilyIndex = presentQueueIndex;
			createInfo.queueCount = 1;
			createInfo.pQueuePriorities = &defaultQueuePriority;
			m_PhysicalDevice->m_QueueCreateInfos.push_back(createInfo);
		}
	}
	const std::vector<char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(m_PhysicalDevice->m_QueueCreateInfos.size());
	deviceCreateInfo.pQueueCreateInfos = m_PhysicalDevice->m_QueueCreateInfos.data();
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	deviceCreateInfo.pEnabledFeatures = &enabledFeatures;

	VK_CHECK_RESULT(vkCreateDevice(m_PhysicalDevice->GetVulkanPhysicalDevice(), &deviceCreateInfo, nullptr, &m_LogicalDevice));

	// Get a graphics queue from the device
	vkGetDeviceQueue(m_LogicalDevice, m_PhysicalDevice->m_QueueFamilyIndices.Graphic, 0, &m_Queue);
}


void VulkanDevice::Cleanup()
{
	CORE_INFO("Destroying vulkan logical device.");
	vkDestroyDevice(m_LogicalDevice, nullptr);
}
