#pragma once

#include "Vulkan.h"
#include <memory>
#include <unordered_set>

class VulkanPhysicalDevice
{
public:
	struct QueueFamilyIndices
	{
		int32_t Graphic = -1;
		int32_t Compute = -1;
		int32_t Transfer = -1;
	};

public:
	VulkanPhysicalDevice();
	~VulkanPhysicalDevice() = default;

	bool IsExtensionSupported(const char* extensionName) const;
	QueueFamilyIndices GetQueueFamilyIndices(int flags);
	VkPhysicalDevice GetVulkanPhysicalDevice() const { return m_PhysicalDevice; }


	static std::shared_ptr<VulkanPhysicalDevice> Select();

private:
	QueueFamilyIndices m_QueueFamilyIndices;

	VkPhysicalDevice m_PhysicalDevice;
	VkPhysicalDeviceProperties m_Properties;
	VkPhysicalDeviceFeatures m_Features;

	std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
	std::unordered_set<std::string> m_SupportedExtensions;
	std::vector<VkDeviceQueueCreateInfo> m_QueueCreateInfos;

};