#include "VulkanDevice.h"
#include "VulkanRenderer.h"

////////////////////////////////////////////////////////////////////////////////////
// Vulkan Physical Device
////////////////////////////////////////////////////////////////////////////////////
VulkanPhysicalDevice::VulkanPhysicalDevice()
{
	auto vkInstance = VulkanRenderer::GetInstance();

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);

	ASSERT(deviceCount <= 0, "Can't finde GPUs that support Vulkan Instance!");

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
	m_QueueFamilyIndices = GetQueueFamilyIndices(requestedQueueTypes);

}

bool VulkanPhysicalDevice::IsExtensionSupported(const char* extensionName) const
{
    return false;
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

	return indices;
}

std::shared_ptr<VulkanPhysicalDevice> VulkanPhysicalDevice::Select()
{
    return std::make_shared<VulkanPhysicalDevice>();
}