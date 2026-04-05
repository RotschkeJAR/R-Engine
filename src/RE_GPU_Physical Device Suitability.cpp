#include "RE_GPU_Internal.hpp"
#include "RE_Renderer.hpp"
#include "RE_Renderer_Camera.hpp"
#include "RE_Window.hpp"

namespace RE {
	
	bool is_physical_vulkan_device_suitable(const VkPhysicalDevice vk_hPhysicalDevice, int32_t *const pi32Score) {
		PRINT_DEBUG("Checking capabilities of physical Vulkan device ", vk_hPhysicalDevice, " for being suitable");
		std::queue<std::string> incompatibilities,
				optionals,
				warnings;
		int32_t i32Score = 0;
		const uint32_t u32LargestMonitorsScale = *std::max_element(std::begin(largestMonitorSize.aCoords), std::end(largestMonitorSize.aCoords));
		uint32_t u32QueueFamilyCount;
		VkSampleCountFlags vk_mMsaaSampleCountsAvailable = VK_SAMPLE_COUNT_1_BIT 
				| VK_SAMPLE_COUNT_2_BIT
				| VK_SAMPLE_COUNT_4_BIT
				| VK_SAMPLE_COUNT_8_BIT
				| VK_SAMPLE_COUNT_16_BIT
				| VK_SAMPLE_COUNT_32_BIT
				| VK_SAMPLE_COUNT_64_BIT;
		VkPhysicalDeviceType vk_ePhysicalDeviceType;
		const VkDeviceSize vk_largestMonitorScaleBytes = u32LargestMonitorsScale * sizeof(uint8_t[4]),
			vk_cameraUniformBufferSize = sizeof(CameraShaderData),
			vk_gameObjectStorageBufferSize = sizeof(GameObjectShaderData) * 1000,
			vk_largestRequiredMemorySize = std::max<VkDeviceSize>(vk_gameObjectStorageBufferSize, vk_largestMonitorScaleBytes);
		const char *pacDeviceName;

		{ // Properties
			VkPhysicalDeviceVulkan13Properties vk_physicalDeviceProperties_1_3;
			vk_physicalDeviceProperties_1_3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES;
			vk_physicalDeviceProperties_1_3.pNext = nullptr;
			VkPhysicalDeviceVulkan12Properties vk_physicalDeviceProperties_1_2;
			vk_physicalDeviceProperties_1_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
			vk_physicalDeviceProperties_1_2.pNext = &vk_physicalDeviceProperties_1_3;
			VkPhysicalDeviceVulkan11Properties vk_physicalDeviceProperties_1_1;
			vk_physicalDeviceProperties_1_1.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
			vk_physicalDeviceProperties_1_1.pNext = &vk_physicalDeviceProperties_1_2;
			VkPhysicalDeviceProperties2 vk_physicalDeviceProperties_1_0;
			vk_physicalDeviceProperties_1_0.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
			vk_physicalDeviceProperties_1_0.pNext = &vk_physicalDeviceProperties_1_1;
			const VkPhysicalDeviceProperties &vk_physicalDeviceProperties = vk_physicalDeviceProperties_1_0.properties;
			const VkPhysicalDeviceLimits &vk_physicalDeviceLimits = vk_physicalDeviceProperties.limits;
			[[maybe_unused]] const VkPhysicalDeviceSparseProperties &vk_physicalDeviceSparseProperties = vk_physicalDeviceProperties.sparseProperties;
			vkGetPhysicalDeviceProperties2(vk_hPhysicalDevice, &vk_physicalDeviceProperties_1_0);
			vk_ePhysicalDeviceType = vk_physicalDeviceProperties.deviceType;
			pacDeviceName = vk_physicalDeviceProperties.deviceName;
			const uint32_t u32MajorVulkan = VK_API_VERSION_MAJOR(vk_physicalDeviceProperties.apiVersion),
				u32MinorVulkan = VK_API_VERSION_MINOR(vk_physicalDeviceProperties.apiVersion),
				u32PatchVulkan = VK_API_VERSION_PATCH(vk_physicalDeviceProperties.apiVersion);
			if (u32MajorVulkan != VK_API_VERSION_MAJOR(RE_VK_API_VERSION) 
					|| u32MinorVulkan < VK_API_VERSION_MINOR(RE_VK_API_VERSION) 
					|| (u32MinorVulkan == VK_API_VERSION_MINOR(RE_VK_API_VERSION) && u32PatchVulkan < VK_API_VERSION_PATCH(RE_VK_API_VERSION))) {
				println_colored(
						append_to_string("The physical device ", vk_physicalDeviceProperties.deviceName, " has Vulkan ", u32MajorVulkan, ".", u32MinorVulkan, ".", u32PatchVulkan, " installed, but the engine uses ", VK_API_VERSION_MAJOR(RE_VK_API_VERSION), ".", VK_API_VERSION_MINOR(RE_VK_API_VERSION), ".", VK_API_VERSION_PATCH(RE_VK_API_VERSION), ". Different major versions of Vulkan aren't compatible with each other, minor versions are backwards compatible with previous.").c_str(), 
						RE_TERMINAL_COLOR_RED, 
						false, 
						false);
				if (pi32Score)
					*pi32Score = std::numeric_limits<int32_t>::min();
				return false;
			}
			if (vk_physicalDeviceProperties_1_3.maxBufferSize < vk_largestRequiredMemorySize)
				incompatibilities.emplace("The maximum buffer size should be large enough to store 1000 game objects and/or 32-bit RGBA textures as big as twice the largest monitor's scale or more");
			i32Score += static_cast<int32_t>(vk_physicalDeviceProperties_1_3.maxBufferSize / 500000000);
			if ((vk_physicalDeviceProperties_1_2.supportedDepthResolveModes & VK_RESOLVE_MODE_MIN_BIT) == 0)
				incompatibilities.emplace("The required resolve mode for depths is not supported");
			if ((vk_physicalDeviceProperties_1_2.supportedStencilResolveModes & VK_RESOLVE_MODE_MAX_BIT) == 0)
				incompatibilities.emplace("The required resolve mode for stencils is not supported");
			if (vk_physicalDeviceProperties_1_2.maxTimelineSemaphoreValueDifference < 64)
				incompatibilities.emplace("The maximum difference for timeline semaphore values should be at least 64 or more");
			if (vk_physicalDeviceProperties_1_1.maxPerSetDescriptors < RE_VK_MAX_SAMPLED_IMAGES)
				warnings.emplace("The maximum amount of descriptors within a set is lower than the required minimum. Support has to be queried later");
			if (vk_physicalDeviceProperties_1_1.maxMemoryAllocationSize < vk_largestRequiredMemorySize)
				warnings.emplace("The maximum memory allocation size should be large enough to store 1000 game objects and/or 32-bit RGBA textures as big as twice the largest monitor's scale or more");
			i32Score += static_cast<int32_t>(vk_physicalDeviceProperties_1_1.maxMemoryAllocationSize / 500000000);
			switch (vk_physicalDeviceProperties.deviceType) {
				case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
					i32Score += 1000;
					break;
				case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
					i32Score += 500;
					break;
				default:
					i32Score += -1000;
			}
			if (vk_physicalDeviceLimits.maxImageDimension2D < u32LargestMonitorsScale)
				incompatibilities.emplace("The GPU should support image sizes equal or larger than twice of the largest monitor's scale");
			i32Score += static_cast<int32_t>(vk_physicalDeviceLimits.maxImageDimension2D / u32LargestMonitorsScale);
			if (vk_physicalDeviceLimits.maxImageArrayLayers < RE_VK_FRAMES_IN_FLIGHT)
				incompatibilities.push(append_to_string("The maximum of image layers has to be enough to store as many frames are rendered in flight: ", RE_VK_FRAMES_IN_FLIGHT, " or more"));
			if (vk_physicalDeviceLimits.maxUniformBufferRange < vk_cameraUniformBufferSize)
				incompatibilities.push(append_to_string("The maximum range of a uniform buffer should be at least large enough to store ", vk_cameraUniformBufferSize, " bytes or more"));
			if (vk_physicalDeviceLimits.maxStorageBufferRange < vk_gameObjectStorageBufferSize)
				incompatibilities.push(append_to_string("The maximum range of a storage buffer should be at least large enough to store 1000 game objects equal to ", vk_gameObjectStorageBufferSize, " bytes or more"));
			if (vk_physicalDeviceLimits.maxPushConstantsSize < 128)
				incompatibilities.emplace("There should be at least 128 or more bytes of space available for push constants");
			if (vk_physicalDeviceLimits.maxMemoryAllocationCount < 64)
				incompatibilities.emplace("There should be at least 64 or more memory allocations available");
			if (vk_physicalDeviceLimits.maxSamplerAllocationCount < 100)
				incompatibilities.emplace("There should be at least 100 or more sampler allocations available");
			if (vk_physicalDeviceLimits.maxBoundDescriptorSets < 2)
				incompatibilities.emplace("There should be at least 2 or more descriptor sets being bound at once");
			if (vk_physicalDeviceLimits.maxPerStageDescriptorSamplers < RE_VK_MAX_SAMPLED_IMAGES)
				incompatibilities.push(append_to_string("There should be at least ", RE_VK_MAX_SAMPLED_IMAGES, " or more samplers available per pipeline stage within a descriptor set"));
			if (vk_physicalDeviceLimits.maxPerStageDescriptorUniformBuffers < 1)
				incompatibilities.emplace("There should be at least 1 or more uniform buffer available per pipeline stage within a descriptor set");
			if (vk_physicalDeviceLimits.maxPerStageDescriptorStorageBuffers < 2)
				incompatibilities.emplace("There should be at least 2 or more storage buffers available per pipeline stage within a descriptor set");
			if (vk_physicalDeviceLimits.maxPerStageDescriptorSampledImages < RE_VK_MAX_SAMPLED_IMAGES)
				incompatibilities.push(append_to_string("There should be at least ", RE_VK_MAX_SAMPLED_IMAGES, " or more sampled images available per pipeline stage within a descriptor set"));
			if (vk_physicalDeviceLimits.maxPerStageResources < /*descriptors*/ 3 + /*framebuffer attachments*/ 3)
				incompatibilities.emplace("There should be at least 6 or more resources be accessible per pipeline stage");
			if (vk_physicalDeviceLimits.maxDescriptorSetSamplers < RE_VK_MAX_SAMPLED_IMAGES)
				incompatibilities.push(append_to_string("There should be at least ", RE_VK_MAX_SAMPLED_IMAGES, " or more samplers available in an entire pipeline"));
			if (vk_physicalDeviceLimits.maxDescriptorSetUniformBuffers < 1)
				incompatibilities.emplace("There should be at least 1 or more uniform buffer accessible in an entire pipeline");
			if (vk_physicalDeviceLimits.maxDescriptorSetStorageBuffers < 3)
				incompatibilities.emplace("There should be at least 3 or more storage buffers accessible in an entire pipeline");
			if (vk_physicalDeviceLimits.maxDescriptorSetSampledImages < RE_VK_MAX_SAMPLED_IMAGES)
				incompatibilities.push(append_to_string("There should be at least ", RE_VK_MAX_SAMPLED_IMAGES, " sampled images available in an entire pipeline"));
			if (vk_physicalDeviceLimits.maxVertexInputAttributes < 2)
				incompatibilities.emplace("The maximum count of vertex input attributes of a pipeline should be at least 2 or more");
			if (vk_physicalDeviceLimits.maxVertexInputBindings < 1)
				incompatibilities.emplace("The maximum count of vertex bindings of a pipeline should be at least 1 or more");
			if (vk_physicalDeviceLimits.maxVertexInputAttributeOffset < sizeof(float) * 3)
				incompatibilities.emplace("The maximum offset of bytes within a vertex binding of a pipeline should be at least 12 or more");
			if (vk_physicalDeviceLimits.maxVertexInputBindingStride < sizeof(float) * 5)
				incompatibilities.emplace("The maximum vertex stride of bytes of a pipeline should be at least 20 or more");
			if (vk_physicalDeviceLimits.maxVertexOutputComponents < 3)
				incompatibilities.emplace("The maximum count of vertex shader output components within a pipeline should be at least 3 or more");
			if (vk_physicalDeviceLimits.maxFragmentInputComponents < 3)
				incompatibilities.emplace("The maximum count of fragment shader input components within a pipeline should be at least 3 or more");
			if (vk_physicalDeviceLimits.maxFragmentOutputAttachments < 3)
				incompatibilities.emplace("There should be at least 3 or more output attachments accessible by the fragment shader within a pipeline");
			if (vk_physicalDeviceLimits.maxComputeWorkGroupCount[0] < 1)
				incompatibilities.emplace("The maximum count X of dispatchable work groups should be at least 1 or more");
			if (vk_physicalDeviceLimits.maxComputeWorkGroupCount[1] < 1)
				incompatibilities.emplace("The maximum count Y of dispatchable work groups should be at least 1 or more");
			if (vk_physicalDeviceLimits.maxComputeWorkGroupCount[2] < 1)
				incompatibilities.emplace("The maximum count Z of dispatchable work groups should be at least 1 or more");
			if (vk_physicalDeviceLimits.maxComputeWorkGroupInvocations < 1000)
				incompatibilities.emplace("The maximum count of work group invocations should be at least 1000 or more");
			if (vk_physicalDeviceLimits.maxComputeWorkGroupSize[0] < 1000)
				incompatibilities.emplace("The maximum group size X should be at least 1000 or more");
			if (vk_physicalDeviceLimits.maxComputeWorkGroupSize[1] < 1)
				incompatibilities.emplace("The maximum group size Y should be at least 1 or more");
			if (vk_physicalDeviceLimits.maxComputeWorkGroupSize[2] < 1)
				incompatibilities.emplace("The maximum group size Z should be at least 1 or more");
			if (vk_physicalDeviceLimits.maxDrawIndexedIndexValue < 4)
				incompatibilities.emplace("The maximum index within a draw call should be at least 4 or more");
			if (vk_physicalDeviceLimits.maxDrawIndirectCount < 1000)
				incompatibilities.emplace("The maximum indirect draw count should be at least 1000 or more");
			if (vk_physicalDeviceLimits.maxViewports < 1)
				incompatibilities.emplace("There should be at least 1 or more viewports assignable to a pipeline");
			if (vk_physicalDeviceLimits.maxViewportDimensions[0] < u32LargestMonitorsScale)
				incompatibilities.emplace("The maximum of viewport width should be equal or more than twice of the largest monitor's scale");
			if (vk_physicalDeviceLimits.maxViewportDimensions[1] < u32LargestMonitorsScale)
				incompatibilities.emplace("The maximum of viewport height should be equal or more than twice of the largest monitor's scale");
			if (vk_physicalDeviceLimits.maxFramebufferWidth < u32LargestMonitorsScale)
				incompatibilities.emplace("The maximum framebuffer width should be equal or bigger than twice of the largest monitor's scale");
			if (vk_physicalDeviceLimits.maxFramebufferHeight < u32LargestMonitorsScale)
				incompatibilities.emplace("The maximum framebuffer height should be equal or bigger than twice of the largest monitor's scale");
			vk_mMsaaSampleCountsAvailable &= vk_physicalDeviceLimits.framebufferColorSampleCounts 
					& vk_physicalDeviceLimits.framebufferDepthSampleCounts 
					& vk_physicalDeviceLimits.framebufferStencilSampleCounts;
			if (vk_physicalDeviceLimits.maxColorAttachments < 1)
				incompatibilities.emplace("The maximum count of color attachments within a subpass in a render pass should be equal or more than 1");
		}
		{ // Features
			VkPhysicalDeviceVulkan13Features vk_physicalDeviceFeatures_1_3;
			vk_physicalDeviceFeatures_1_3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
			vk_physicalDeviceFeatures_1_3.pNext = nullptr;
			VkPhysicalDeviceVulkan12Features vk_physicalDeviceFeatures_1_2;
			vk_physicalDeviceFeatures_1_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
			vk_physicalDeviceFeatures_1_2.pNext = &vk_physicalDeviceFeatures_1_3;
			VkPhysicalDeviceVulkan11Features vk_physicalDeviceFeatures_1_1;
			vk_physicalDeviceFeatures_1_1.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
			vk_physicalDeviceFeatures_1_1.pNext = &vk_physicalDeviceFeatures_1_2;
			VkPhysicalDeviceFeatures2 vk_physicalDeviceFeatures_1_0;
			vk_physicalDeviceFeatures_1_0.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
			vk_physicalDeviceFeatures_1_0.pNext = &vk_physicalDeviceFeatures_1_1;
			const VkPhysicalDeviceFeatures &vk_physicalDeviceFeatures = vk_physicalDeviceFeatures_1_0.features;
			vkGetPhysicalDeviceFeatures2(vk_hPhysicalDevice, &vk_physicalDeviceFeatures_1_0);
			if (vk_physicalDeviceFeatures_1_3.synchronization2 != VK_TRUE)
				warnings.emplace("The advanced synchronization-feature is not supported");
			if (vk_physicalDeviceFeatures_1_3.dynamicRendering != VK_TRUE)
				incompatibilities.emplace("The dynamic rendering-feature (relinquishment of static render passes) is not supported");
			if (vk_physicalDeviceFeatures_1_2.descriptorBindingUpdateUnusedWhilePending != VK_TRUE)
				incompatibilities.emplace("The GPU doesn't support unused descriptor bindings being updated while pending");
			if (vk_physicalDeviceFeatures_1_2.descriptorBindingPartiallyBound != VK_TRUE)
				incompatibilities.emplace("The GPU doesn't support partially bound descriptor bindings");
			if (vk_physicalDeviceFeatures_1_2.timelineSemaphore != VK_TRUE)
				incompatibilities.emplace("Timeline semaphores aren't supported");
			if (vk_physicalDeviceFeatures.sampleRateShading != VK_TRUE)
				optionals.emplace("Sample shading (fragment shader executes per sample) is not supported");
			if (vk_physicalDeviceFeatures.samplerAnisotropy != VK_TRUE)
				optionals.emplace("Anisotropic filtering for textures is not supported");
		}
		{ // Queues
			constexpr VkQueueFlags vk_amExpectedQueueFlags[] = {
				VK_QUEUE_TRANSFER_BIT & VK_QUEUE_COMPUTE_BIT & VK_QUEUE_GRAPHICS_BIT,
				VK_QUEUE_TRANSFER_BIT & VK_QUEUE_COMPUTE_BIT,
				VK_QUEUE_TRANSFER_BIT
			};
			vkGetPhysicalDeviceQueueFamilyProperties2(vk_hPhysicalDevice, &u32QueueFamilyCount, nullptr);
			std::unique_ptr<VkQueueFamilyProperties2[]> queueFamilies = std::make_unique<VkQueueFamilyProperties2[]>(u32QueueFamilyCount);
			for (uint32_t u32QueueFamilyIndex = 0; u32QueueFamilyIndex < u32QueueFamilyCount; u32QueueFamilyIndex++) {
				queueFamilies[u32QueueFamilyIndex].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
				queueFamilies[u32QueueFamilyIndex].pNext = nullptr;
			}
			vkGetPhysicalDeviceQueueFamilyProperties2(vk_hPhysicalDevice, &u32QueueFamilyCount, queueFamilies.get());
			for (uint32_t u32QueueFamilyIndex = 0; u32QueueFamilyIndex < u32QueueFamilyCount; u32QueueFamilyIndex++) {
				const VkQueueFlags vk_mQueueFlags = queueFamilies[u32QueueFamilyIndex].queueFamilyProperties.queueFlags;
				for (const VkQueueFlags vk_mExpectedQueueFlags : vk_amExpectedQueueFlags)
					if ((vk_mQueueFlags & vk_mExpectedQueueFlags) == vk_mExpectedQueueFlags)
						i32Score += 10 - std::popcount<VkQueueFlags>(vk_mQueueFlags & (~vk_mExpectedQueueFlags));
			}
		}
		{ // Memories
			VkPhysicalDeviceMemoryProperties2 vk_physicalDeviceMemoryProperties2;
			vk_physicalDeviceMemoryProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
			vk_physicalDeviceMemoryProperties2.pNext = nullptr;
			const VkPhysicalDeviceMemoryProperties &vk_physicalDeviceMemoryProperties = vk_physicalDeviceMemoryProperties2.memoryProperties;
			vkGetPhysicalDeviceMemoryProperties2(vk_hPhysicalDevice, &vk_physicalDeviceMemoryProperties2);
			VkDeviceSize vk_cpuMemory = 0,
				vk_gpuMemory = 0;
			bool bDiscreteMemoryExists = false;
			for (uint32_t u32MemoryHeapIndex = 0; u32MemoryHeapIndex < vk_physicalDeviceMemoryProperties.memoryHeapCount; u32MemoryHeapIndex++) {
				if ((vk_physicalDeviceMemoryProperties.memoryHeaps[u32MemoryHeapIndex].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) != 0) {
					vk_gpuMemory += vk_physicalDeviceMemoryProperties.memoryHeaps[u32MemoryHeapIndex].size;
					bDiscreteMemoryExists = true;
				} else
					vk_cpuMemory += vk_physicalDeviceMemoryProperties.memoryHeaps[u32MemoryHeapIndex].size;
			}
			if (vk_ePhysicalDeviceType != VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
				if (!bDiscreteMemoryExists) {
					warnings.emplace("This device doesn't have its own physical memory");
					i32Score += -100;
				} else
					i32Score += (vk_cpuMemory / 1000000000) * (vk_gpuMemory / 1000000000) - 16;
			} else
				i32Score += vk_cpuMemory / 1000000000 + vk_gpuMemory / 1000000000 - 16;
			uint8_t m8CoherencePresence = 0;
			constexpr uint8_t m8GpuCoherenceAbsent = 0x1,
				m8GpuCoherencePresent = 0x2,
				m8CpuCoherenceAbsent = 0x4,
				m8CpuCoherencePresent = 0x8;
			for (uint32_t u32MemoryTypeIndex = 0; u32MemoryTypeIndex < vk_physicalDeviceMemoryProperties.memoryTypeCount; u32MemoryTypeIndex++) {
				const VkMemoryPropertyFlags vk_mPropertyFlags = vk_physicalDeviceMemoryProperties.memoryTypes[u32MemoryTypeIndex].propertyFlags;
				if ((vk_mPropertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0 && (vk_mPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == 0) {
					// device-local RAM
					i32Score += std::popcount(vk_mPropertyFlags & (~(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT))) * -5 + 50;
					if ((vk_mPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
						m8CoherencePresence |= m8GpuCoherenceAbsent;
					else
						m8CoherencePresence |= m8GpuCoherencePresent;
				} else if ((vk_mPropertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == 0 && (vk_mPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0) {
					// CPU-local RAM
					i32Score += std::popcount(vk_mPropertyFlags & (~VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) * -5 + 50;
					if ((vk_mPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
						m8CoherencePresence |= m8CpuCoherenceAbsent;
					else
						m8CoherencePresence |= m8CpuCoherencePresent;
				}
			}
			if ((m8CoherencePresence & (m8GpuCoherenceAbsent | m8GpuCoherencePresent)) == (m8GpuCoherenceAbsent | m8GpuCoherencePresent)) // device
				i32Score += 50;
			if ((m8CoherencePresence & (m8CpuCoherenceAbsent | m8CpuCoherencePresent)) == (m8CpuCoherenceAbsent | m8CpuCoherencePresent)) // CPU
				i32Score += 50;
		}
		{ // Extensions
			uint32_t u32ExtensionCount;
			vkEnumerateDeviceExtensionProperties(vk_hPhysicalDevice, nullptr, &u32ExtensionCount, nullptr);
			std::unique_ptr<VkExtensionProperties[]> extensions = std::make_unique<VkExtensionProperties[]>(u32ExtensionCount);
			vkEnumerateDeviceExtensionProperties(vk_hPhysicalDevice, nullptr, &u32ExtensionCount, extensions.get());
			bool bSwapchainExtensionAvailable = false;
			for (uint32_t u32ExtensionIndex = 0; u32ExtensionIndex < u32ExtensionCount; u32ExtensionIndex++) {
				if (are_string_contents_equal(extensions[u32ExtensionIndex].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
					bSwapchainExtensionAvailable = true;
					break;
				}
			}
			if (!bSwapchainExtensionAvailable)
				incompatibilities.emplace("The Swapchain-extension for displaying images is not supported");
		}
		{ // Vertex formats
			constexpr VkFormat vk_aeVertexFormats[] = {
				VK_FORMAT_R32_UINT,
				VK_FORMAT_R32_SINT,
				VK_FORMAT_R32_SFLOAT,
				VK_FORMAT_R32G32_UINT,
				VK_FORMAT_R32G32_SINT,
				VK_FORMAT_R32G32_SFLOAT,
				VK_FORMAT_R32G32B32_UINT,
				VK_FORMAT_R32G32B32_SINT,
				VK_FORMAT_R32G32B32_SFLOAT,
				VK_FORMAT_R32G32B32A32_UINT,
				VK_FORMAT_R32G32B32A32_SINT,
				VK_FORMAT_R32G32B32A32_SFLOAT
			};
			VkFormatProperties2 vk_formatProperties2;
			vk_formatProperties2.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
			vk_formatProperties2.pNext = nullptr;
			const VkFormatProperties &vk_rFormatProperties = vk_formatProperties2.formatProperties;
			for (const VkFormat vk_eVertexFormat : vk_aeVertexFormats) {
				vkGetPhysicalDeviceFormatProperties2(vk_hPhysicalDevice, vk_eVertexFormat, &vk_formatProperties2);
				if ((vk_rFormatProperties.bufferFeatures & VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) == 0) {
					incompatibilities.emplace("Not all relevant formats for vertex buffers can be used for the said purpose");
					break;
				}
			}
		}
		{ // Image formats
			VkFormatProperties2 vk_formatProperties2;
			vk_formatProperties2.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
			vk_formatProperties2.pNext = nullptr;
			const VkFormatProperties &vk_rFormatProperties = vk_formatProperties2.formatProperties;
			VkImageFormatProperties2 vk_imageFormatProperties2;
			vk_imageFormatProperties2.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2;
			vk_imageFormatProperties2.pNext = nullptr;
			const VkImageFormatProperties &vk_rImageFormatProperties = vk_imageFormatProperties2.imageFormatProperties;
			VkPhysicalDeviceImageFormatInfo2 vk_imageFormatInfo;
			vk_imageFormatInfo.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2;
			vk_imageFormatInfo.pNext = nullptr;
			vk_imageFormatInfo.type = VK_IMAGE_TYPE_2D;
			vk_imageFormatInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			vk_imageFormatInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			vk_imageFormatInfo.flags = 0;
			bool bTextureAllChannelsSupported = false,
				bRenderTargetSupported = false,
				bDepthSupported = false,
				bStencilSupported = false;

			constexpr VkFormat vk_aeTextureFormats[] = {
				VK_FORMAT_R8_SRGB,
				VK_FORMAT_R8G8_SRGB,
				VK_FORMAT_R8G8B8_SRGB,
				VK_FORMAT_B8G8R8_SRGB,
				VK_FORMAT_R8G8B8A8_SRGB,
				VK_FORMAT_B8G8R8A8_SRGB
			};
			for (const VkFormat vk_eTextureFormat : vk_aeTextureFormats) {
				vkGetPhysicalDeviceFormatProperties2(vk_hPhysicalDevice, vk_eTextureFormat, &vk_formatProperties2);
				constexpr VkFormatFeatureFlags vk_mExpectedFormatFeatures = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
				if ((vk_rFormatProperties.optimalTilingFeatures & vk_mExpectedFormatFeatures) != vk_mExpectedFormatFeatures)
					continue;
				vk_imageFormatInfo.format = vk_eTextureFormat;
				const VkResult vk_eResult = vkGetPhysicalDeviceImageFormatProperties2(vk_hPhysicalDevice, &vk_imageFormatInfo, &vk_imageFormatProperties2);
				switch (vk_eResult) {
					case VK_SUCCESS:
						if (vk_rImageFormatProperties.maxExtent.width >= largestMonitorSize[0]
								&& vk_rImageFormatProperties.maxExtent.height >= largestMonitorSize[1]
								&& vk_rImageFormatProperties.maxMipLevels >= 4
								&& vk_rImageFormatProperties.maxResourceSize >= 4 * largestMonitorSize.area()) {
							i32Score += 100;
							switch (vk_eTextureFormat) {
								case VK_FORMAT_R8G8B8A8_SRGB:
								case VK_FORMAT_B8G8R8A8_SRGB:
									bTextureAllChannelsSupported = true;
									break;
								default:
									break;
							}
						}
						break;
					case VK_ERROR_FORMAT_NOT_SUPPORTED:
						break;
					default:
						RE_ERROR("Failed to retrieve image format properties of Vulkan format ", std::hex, vk_eTextureFormat, " (Vulkan error code: ", vk_eResult, ")");
				}
			}

			vk_imageFormatInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			constexpr VkFormat vk_aeRenderTargetFormats[] = {
				VK_FORMAT_R8G8B8A8_SRGB,
				VK_FORMAT_B8G8R8A8_SRGB,
				VK_FORMAT_A2R10G10B10_UNORM_PACK32,
				VK_FORMAT_A2B10G10R10_UNORM_PACK32,
				VK_FORMAT_R16G16B16A16_SFLOAT,
				VK_FORMAT_B10G11R11_UFLOAT_PACK32
			};
			for (const VkFormat vk_eRenderTargetFormat : vk_aeRenderTargetFormats) {
				vkGetPhysicalDeviceFormatProperties2(vk_hPhysicalDevice, vk_eRenderTargetFormat, &vk_formatProperties2);
				constexpr VkFormatFeatureFlags vk_mExpectedFormatFeatures = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
				if ((vk_rFormatProperties.optimalTilingFeatures & vk_mExpectedFormatFeatures) != vk_mExpectedFormatFeatures)
					continue;
				vk_imageFormatInfo.format = vk_eRenderTargetFormat;
				const VkResult vk_eResult = vkGetPhysicalDeviceImageFormatProperties2(vk_hPhysicalDevice, &vk_imageFormatInfo, &vk_imageFormatProperties2);
				switch (vk_eResult) {
					case VK_SUCCESS:
						if (vk_rImageFormatProperties.maxExtent.width >= largestMonitorSize[0]
								&& vk_rImageFormatProperties.maxExtent.height >= largestMonitorSize[1]
								&& vk_rImageFormatProperties.maxArrayLayers >= RE_VK_FRAMES_IN_FLIGHT
								&& vk_rImageFormatProperties.maxResourceSize >= 8 * largestMonitorSize.area()) {
							i32Score += 100;
							bRenderTargetSupported = true;
						}
						break;
					case VK_ERROR_FORMAT_NOT_SUPPORTED:
						break;
					default:
						RE_ERROR("Failed to retrieve image format properties of Vulkan format ", std::hex, vk_eRenderTargetFormat, " (Vulkan error code: ", vk_eResult, ")");
				}
			}

			constexpr VkFormat vk_aeDepthStencilFormats[] = {
				VK_FORMAT_D16_UNORM,
				VK_FORMAT_X8_D24_UNORM_PACK32,
				VK_FORMAT_D32_SFLOAT,
				VK_FORMAT_S8_UINT,
				VK_FORMAT_D16_UNORM_S8_UINT,
				VK_FORMAT_D24_UNORM_S8_UINT,
				VK_FORMAT_D32_SFLOAT_S8_UINT
			};
			for (uint8_t u8TransientIter = 0; u8TransientIter < 2; u8TransientIter++) {
				switch (u8TransientIter) {
					case 0:
						vk_imageFormatInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
						break;
					case 1:
						vk_imageFormatInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
						break;
					[[unlikely]] default:
						RE_ABORT("Unknown iteration index ", u8TransientIter, " for checking depth-stencil image capabilities");
				}
				for (const VkFormat vk_eDepthStencilFormat : vk_aeDepthStencilFormats) {
					vkGetPhysicalDeviceFormatProperties2(vk_hPhysicalDevice, vk_eDepthStencilFormat, &vk_formatProperties2);
					constexpr VkFormatFeatureFlags vk_mExpectedFormatFeatures = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
					if ((vk_rFormatProperties.optimalTilingFeatures & vk_mExpectedFormatFeatures) != vk_mExpectedFormatFeatures)
						continue;
					vk_imageFormatInfo.format = vk_eDepthStencilFormat;
					const VkResult vk_eResult = vkGetPhysicalDeviceImageFormatProperties2(vk_hPhysicalDevice, &vk_imageFormatInfo, &vk_imageFormatProperties2);
					switch (vk_eResult) {
						case VK_SUCCESS:
							if (vk_rImageFormatProperties.maxExtent.width >= largestMonitorSize[0]
									&& vk_rImageFormatProperties.maxExtent.height >= largestMonitorSize[1]
									&& vk_rImageFormatProperties.maxArrayLayers >= RE_VK_FRAMES_IN_FLIGHT
									&& vk_rImageFormatProperties.maxResourceSize >= 8 * largestMonitorSize.area()) {
								i32Score += 100;
								vk_mMsaaSampleCountsAvailable &= vk_rImageFormatProperties.sampleCounts;
								switch (vk_eDepthStencilFormat) {
									case VK_FORMAT_D16_UNORM:
									case VK_FORMAT_X8_D24_UNORM_PACK32:
									case VK_FORMAT_D32_SFLOAT:
										bDepthSupported = true;
										break;
									case VK_FORMAT_S8_UINT:
										bStencilSupported = true;
										break;
									case VK_FORMAT_D16_UNORM_S8_UINT:
									case VK_FORMAT_D24_UNORM_S8_UINT:
									case VK_FORMAT_D32_SFLOAT_S8_UINT:
										bDepthSupported = true;
										bStencilSupported = true;
										break;
									[[unlikely]] default:
										RE_ABORT("Unknown Vulkan format, that neither supports depth and/or stencils");
								}
							}
							break;
						case VK_ERROR_FORMAT_NOT_SUPPORTED:
							break;
						default:
							RE_ERROR("Failed to retrieve image format properties of Vulkan format ", std::hex, vk_eDepthStencilFormat, " (Vulkan error code: ", vk_eResult, ")");
					}
				}
				if (bDepthSupported && bStencilSupported)
					break;
			}
			if (!bTextureAllChannelsSupported)
				incompatibilities.emplace("The essential image formats RGBA and BGRA aren't supported on this device");
			if (!bRenderTargetSupported)
				incompatibilities.emplace("None of the essential image formats used for rendering aren't supported on this device");
			if (!bDepthSupported)
				incompatibilities.emplace("The essential image formats supporting depth aren't available on this device");
			if (!bStencilSupported)
				incompatibilities.emplace("The essential image formats supporting stencils aren't available on this device");
		}
		{ // Surface
			VkBool32 vk_bSurfaceSupportExists = VK_FALSE;
			for (uint32_t u32QueueFamilyIndex = 0; u32QueueFamilyIndex < u32QueueFamilyCount; u32QueueFamilyIndex++) {
				vkGetPhysicalDeviceSurfaceSupportKHR(vk_hPhysicalDevice, u32QueueFamilyIndex, vk_hSurface, &vk_bSurfaceSupportExists);
				if (vk_bSurfaceSupportExists == VK_TRUE)
					break;
			}
			if (vk_bSurfaceSupportExists != VK_TRUE)
				incompatibilities.emplace("The GPU cannot display the contents to the display");
			VkSurfaceCapabilitiesKHR vk_surfaceCapabilities;
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_hPhysicalDevice, vk_hSurface, &vk_surfaceCapabilities);
			if ((vk_surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) == 0)
				incompatibilities.emplace("The GPU doesn't support identity transforms of the content");
			if ((vk_surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) == 0)
				incompatibilities.emplace("The GPU doesn't support opaque displays");
			constexpr VkImageUsageFlags vk_mExpectedSurfaceImageUsages = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			if ((vk_surfaceCapabilities.supportedUsageFlags & vk_mExpectedSurfaceImageUsages) == vk_mExpectedSurfaceImageUsages) {
				// Surface Formats
				uint32_t u32SurfaceFormatCount;
				vkGetPhysicalDeviceSurfaceFormatsKHR(vk_hPhysicalDevice, vk_hSurface, &u32SurfaceFormatCount, nullptr);
				std::unique_ptr<VkSurfaceFormatKHR[]> surfaceFormats = std::make_unique<VkSurfaceFormatKHR[]>(u32SurfaceFormatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(vk_hPhysicalDevice, vk_hSurface, &u32SurfaceFormatCount, surfaceFormats.get());
				VkPhysicalDeviceImageFormatInfo2 vk_imageFormatInfo;
				vk_imageFormatInfo.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2;
				vk_imageFormatInfo.pNext = nullptr;
				vk_imageFormatInfo.type = VK_IMAGE_TYPE_2D;
				vk_imageFormatInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
				vk_imageFormatInfo.usage = vk_mExpectedSurfaceImageUsages;
				vk_imageFormatInfo.flags = 0;
				VkImageFormatProperties2 vk_imageFormatProperties2;
				vk_imageFormatProperties2.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2;
				vk_imageFormatProperties2.pNext = nullptr;
				const VkImageFormatProperties &vk_rImageFormatProperties = vk_imageFormatProperties2.imageFormatProperties;
				bool bSurfaceFormatAppropriatelySupported = false;
				for (uint32_t u32SurfaceFormatIndex = 0; u32SurfaceFormatIndex < u32SurfaceFormatCount; u32SurfaceFormatIndex++) {
					if (surfaceFormats[u32SurfaceFormatIndex].colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
						continue;
					vk_imageFormatInfo.format = surfaceFormats[u32SurfaceFormatIndex].format;
					const VkResult vk_eResult = vkGetPhysicalDeviceImageFormatProperties2(vk_hPhysicalDevice, &vk_imageFormatInfo, &vk_imageFormatProperties2);
					switch (vk_eResult) {
						case VK_SUCCESS:
							if (vk_rImageFormatProperties.maxExtent.width >= largestMonitorSize[0]
									&& vk_rImageFormatProperties.maxExtent.height >= largestMonitorSize[1]
									&& vk_rImageFormatProperties.maxArrayLayers >= RE_VK_FRAMES_IN_FLIGHT
									&& vk_rImageFormatProperties.maxResourceSize >= 8 * largestMonitorSize.area()) {
								bSurfaceFormatAppropriatelySupported = true;
								goto BREAK_SURFACE_FORMAT_LOOP;
							}
							break;
						case VK_ERROR_FORMAT_NOT_SUPPORTED:
							break;
						default:
							RE_ERROR("Failed to retrieve image format properties of Vulkan format ", std::hex, vk_imageFormatInfo.format, " (Vulkan error code: ", vk_eResult, ")");
					}
				}
			BREAK_SURFACE_FORMAT_LOOP:
				if (!bSurfaceFormatAppropriatelySupported)
					incompatibilities.emplace("Neither of all available surface formats is properly supported for rendering");
			} else
				incompatibilities.emplace("The required image usage flags aren't supported for displayable content");
			{ // Present modes
				uint32_t u32PresentModeCount;
				vkGetPhysicalDeviceSurfacePresentModesKHR(vk_hPhysicalDevice, vk_hSurface, &u32PresentModeCount, nullptr);
				std::unique_ptr<VkPresentModeKHR[]> presentModes = std::make_unique<VkPresentModeKHR[]>(u32PresentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(vk_hPhysicalDevice, vk_hSurface, &u32PresentModeCount, presentModes.get());
				bool bFifoPresentationSupported = false;
				for (uint32_t u32PresentModeIndex = 0; u32PresentModeIndex < u32PresentModeCount; u32PresentModeIndex++)
					if (presentModes[u32PresentModeIndex] == VK_PRESENT_MODE_FIFO_KHR) {
						bFifoPresentationSupported = true;
						break;
					}
				if (!bFifoPresentationSupported)
					incompatibilities.emplace("The GPU doesn't support the FIFO-present mode");
			}
		}
		if (!vk_mMsaaSampleCountsAvailable)
			incompatibilities.emplace("All resources required for rendering are not compatible with MSAA either en- or disabled");
		else if ((vk_mMsaaSampleCountsAvailable & VK_SAMPLE_COUNT_1_BIT) == 0) {
			warnings.emplace("Rendering without MSAA enabled is not possible, because certain resources cannot be used for this purpose");
			i32Score += -1000;
		} else
			i32Score += std::popcount<VkSampleCountFlags>(vk_mMsaaSampleCountsAvailable & (~VK_SAMPLE_COUNT_1_BIT)) * 5 + 10;

		if (pi32Score)
			*pi32Score = i32Score;
		const bool bDeviceCompatible = incompatibilities.empty();
		if (!incompatibilities.empty()) {
			print_colored("Physical Vulkan device ", RE_TERMINAL_COLOR_RED, false, false);
			print_colored(pacDeviceName, RE_TERMINAL_COLOR_RED, false, false);
			println_colored(" has incompatibility/-ies:", RE_TERMINAL_COLOR_RED, false, false);
			do {
				print_colored(" - ", RE_TERMINAL_COLOR_RED, false, false);
				println_colored(incompatibilities.front(), RE_TERMINAL_COLOR_RED, false, false);
				incompatibilities.pop();
			} while (!incompatibilities.empty());
		}
		if (!warnings.empty()) {
			print_colored("There are warnings about the physical Vulkan device ", RE_TERMINAL_COLOR_BRIGHT_RED, false, false);
			print_colored(pacDeviceName, RE_TERMINAL_COLOR_BRIGHT_RED, false, false);
			println_colored(":", RE_TERMINAL_COLOR_BRIGHT_RED, false, false);
			do {
				print_colored(" - ", RE_TERMINAL_COLOR_BRIGHT_RED, false, false);
				println_colored(warnings.front(), RE_TERMINAL_COLOR_BRIGHT_RED, false, false);
				warnings.pop();
			} while (!warnings.empty());
		}
		if (!optionals.empty()) {
			print_colored("Physical Vulkan device ", RE_TERMINAL_COLOR_BRIGHT_YELLOW, false, false);
			print_colored(pacDeviceName, RE_TERMINAL_COLOR_BRIGHT_YELLOW, false, false);
			println_colored(" doesn't support the following features, which aren't mandatory:", RE_TERMINAL_COLOR_BRIGHT_YELLOW, false, false);
			do {
				print_colored(" - ", RE_TERMINAL_COLOR_BRIGHT_YELLOW, false, false);
				println_colored(optionals.front(), RE_TERMINAL_COLOR_BRIGHT_YELLOW, false, false);
				optionals.pop();
			} while (!optionals.empty());
		}
		return bDeviceCompatible;
	}

}
