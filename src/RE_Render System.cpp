#include "RE_Render System.hpp"
#include "RE_Renderer.hpp"
#include "RE_Window.hpp"
#include "RE_Main.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"

#include <queue>

namespace RE {

	// Attributes initialized at beginning and rarely changed
	VkPhysicalDevice *vk_phPhysicalDevicesAvailable = nullptr;
	uint32_t u32PhysicalDevicesAvailableCount = 0U;
	VkPhysicalDevice vk_hPhysicalDeviceSelected = VK_NULL_HANDLE;
	VkPhysicalDeviceMemoryProperties vk_physicalDeviceMemoryProperties = {};
	VkQueue vk_hDeviceQueueFamilies[RE_VK_QUEUE_COUNT] = {};
	uint32_t u32DeviceQueueFamilyIndices[RE_VK_QUEUE_COUNT] = {};
	VkSurfaceKHR vk_hSurface = VK_NULL_HANDLE;
	VkSurfaceCapabilitiesKHR vk_surfaceCapabilities = {};
	uint32_t u32SurfaceFormatsAvailableCount = 0U;
	VkSurfaceFormatKHR *vk_pSurfaceFormatsAvailable = nullptr;
	VkSurfaceFormatKHR vk_surfaceFormatSelected = {};
	VkPresentModeKHR vk_ePresentModeVsync = VK_PRESENT_MODE_FIFO_KHR, vk_ePresentModeNoVsync = VK_PRESENT_MODE_FIFO_KHR;
	VkSwapchainKHR vk_hSwapchain = VK_NULL_HANDLE;
	VkFormat vk_eSwapchainImageFormat = VK_FORMAT_UNDEFINED;
	VkExtent2D vk_swapchainResolution = {};
	uint32_t u32SwapchainImageCount = 0U;
	VkImage *vk_phSwapchainImages = nullptr;
	VkImageView *vk_phSwapchainImageViews = nullptr;
	VkCommandPool vk_hCommandPools[RE_VK_COMMAND_POOL_COUNT] = {};
	VkCommandBuffer vk_hDummyTransferCommandBuffer = VK_NULL_HANDLE;

	uint8_t u8RenderSystemFlags = 0b00000110U;
#define SWAPCHAIN_DIRTY_BIT 0
#define VSYNC_SETTING_BIT 1
#define FPS_BOUND_TO_VSYNC_BIT 2
#define GRAPHICS_QUEUE_SUPPORTS_TRANSFER_BIT 3

	static void println_vkbool32(const char* pcName, VkBool32 vk_bState) {
		print("\t\t\t", pcName, ": ");
		if (vk_bState)
			println_colored("true", RE_TERMINAL_COLOR_GREEN, false, false);
		else
			println_colored("false", RE_TERMINAL_COLOR_RED, false, false);
	}

	static void select_best_physical_vulkan_device() {
		int32_t i32BestDeviceScore = std::numeric_limits<int32_t>::min();
		for (uint32_t i = 0U; i < u32PhysicalDevicesAvailableCount; i++) {
			VkPhysicalDeviceProperties vk_physicalDeviceProperties;
			vkGetPhysicalDeviceProperties(vk_phPhysicalDevicesAvailable[i], &vk_physicalDeviceProperties);
			int32_t i32CurrentDeviceScore = 0;
			switch (vk_physicalDeviceProperties.deviceType) {
				case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
					i32CurrentDeviceScore += 1000;
					break;
				case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
					i32CurrentDeviceScore += 500;
					break;
				default:
					i32CurrentDeviceScore -= 1000;
					break;
			}
			i32CurrentDeviceScore += vk_physicalDeviceProperties.limits.maxImageDimension2D;
			if (i32BestDeviceScore < i32CurrentDeviceScore) {
				i32BestDeviceScore = i32CurrentDeviceScore;
				vk_hPhysicalDeviceSelected = vk_phPhysicalDevicesAvailable[i];
			}
		}
	}

	static void select_best_surface_format() {
		int32_t i32BestSurfaceFormatScore = std::numeric_limits<int32_t>::min();
		for (uint32_t i = 0U; i < u32SurfaceFormatsAvailableCount; i++) {
			int32_t i32CurrentSurfaceFormatScore = 0;
			switch (vk_pSurfaceFormatsAvailable[i].format) {
				case VK_FORMAT_R8G8B8A8_UNORM:
				case VK_FORMAT_B8G8R8A8_UNORM:
					i32CurrentSurfaceFormatScore += 500;
					break;
				case VK_FORMAT_R8G8B8A8_SRGB:
				case VK_FORMAT_B8G8R8A8_SRGB:
					i32CurrentSurfaceFormatScore += 1000;
					break;
				default:
					i32CurrentSurfaceFormatScore -= 2000;
					break;
			}
			i32CurrentSurfaceFormatScore += (vk_pSurfaceFormatsAvailable[i].colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ? -1 : 1) * 5000;
			if (i32BestSurfaceFormatScore < i32CurrentSurfaceFormatScore) {
				i32BestSurfaceFormatScore = i32CurrentSurfaceFormatScore;
				vk_surfaceFormatSelected = vk_pSurfaceFormatsAvailable[i];
			}
		}
	}

	static bool create_surface() {
		return CATCH_SIGNAL_AND_RETURN(Window::pInstance->create_vulkan_surface(vk_hSurface), bool);
	}
	
	static void destroy_surface() {
		vkDestroySurfaceKHR(vk_hInstance, vk_hSurface, nullptr);
		DELETE_ARRAY_SAFELY(vk_pSurfaceFormatsAvailable);
		vk_hSurface = VK_NULL_HANDLE;
	}

	static void fetch_surface_infos() {
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &vk_surfaceCapabilities);
		vkGetPhysicalDeviceSurfaceFormatsKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &u32SurfaceFormatsAvailableCount, nullptr);
		DELETE_ARRAY_SAFELY(vk_pSurfaceFormatsAvailable);
		vk_pSurfaceFormatsAvailable = new VkSurfaceFormatKHR[u32SurfaceFormatsAvailableCount];
		vkGetPhysicalDeviceSurfaceFormatsKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &u32SurfaceFormatsAvailableCount, vk_pSurfaceFormatsAvailable);
		uint32_t u32PresentModesCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &u32PresentModesCount, nullptr);
		VkPresentModeKHR *const vk_peAllPresentModes = new VkPresentModeKHR[u32PresentModesCount];
		vkGetPhysicalDeviceSurfacePresentModesKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &u32PresentModesCount, vk_peAllPresentModes);

		vk_ePresentModeVsync = VK_PRESENT_MODE_FIFO_KHR;
		vk_ePresentModeNoVsync = VK_PRESENT_MODE_FIFO_KHR;
		for (uint32_t u32PresentModeIndex = 0U; u32PresentModeIndex < u32PresentModesCount; u32PresentModeIndex++) {
			switch (vk_peAllPresentModes[u32PresentModeIndex]) {
				case VK_PRESENT_MODE_IMMEDIATE_KHR:
					vk_ePresentModeNoVsync = VK_PRESENT_MODE_IMMEDIATE_KHR;
					break;
				case VK_PRESENT_MODE_MAILBOX_KHR:
					vk_ePresentModeVsync = VK_PRESENT_MODE_MAILBOX_KHR;
					break;
				default:
					break;
			}
		}
		delete[] vk_peAllPresentModes;
	}

	static bool alloc_physical_device_list() {
		DEFINE_SIGNAL_GUARD(sigGuardAllocPhysicalDeviceList);
		uint32_t u32TotalPhysicalDeviceCount;
		vkEnumeratePhysicalDevices(vk_hInstance, &u32TotalPhysicalDeviceCount, nullptr);
		if (!u32TotalPhysicalDeviceCount) {
			RE_FATAL_ERROR("There aren't any physical devices supporting Vulkan");
			return false;
		}
		PRINT_LN("Available GPUs:");
		VkPhysicalDevice *const vk_phTotalPhysicalDevice = new VkPhysicalDevice[u32TotalPhysicalDeviceCount];
		vkEnumeratePhysicalDevices(vk_hInstance, &u32TotalPhysicalDeviceCount, vk_phTotalPhysicalDevice);
		std::queue<VkPhysicalDevice> suitablePhysicalDevices;
		for (uint32_t u32PhysicalDeviceIndex = 0U; u32PhysicalDeviceIndex < u32TotalPhysicalDeviceCount; u32PhysicalDeviceIndex++) {
			const VkPhysicalDevice vk_hPhysicalDevice = vk_phTotalPhysicalDevice[u32PhysicalDeviceIndex];

			// Fetch general information about the GPU
			VkPhysicalDeviceProperties vk_physicalDeviceProperties;
			vkGetPhysicalDeviceProperties(vk_hPhysicalDevice, &vk_physicalDeviceProperties);

			// Fetch extensions-data about the GPU
			uint32_t u32PhysicalDeviceExtensionCount;
			vkEnumerateDeviceExtensionProperties(vk_hPhysicalDevice, nullptr, &u32PhysicalDeviceExtensionCount, nullptr);
			VkExtensionProperties *const vk_pPhysicalDeviceExtensionProperties = new VkExtensionProperties[u32PhysicalDeviceExtensionCount];
			vkEnumerateDeviceExtensionProperties(vk_hPhysicalDevice, nullptr, &u32PhysicalDeviceExtensionCount, vk_pPhysicalDeviceExtensionProperties);

			// Fetch queue-data about the GPU
			uint32_t u32PhysicalDeviceQueueFamilyCount;
			vkGetPhysicalDeviceQueueFamilyProperties(vk_hPhysicalDevice, &u32PhysicalDeviceQueueFamilyCount, nullptr);
			VkQueueFamilyProperties *const vk_pPhysicalDeviceQueueFamilyProperties = new VkQueueFamilyProperties[u32PhysicalDeviceQueueFamilyCount];
			vkGetPhysicalDeviceQueueFamilyProperties(vk_hPhysicalDevice, &u32PhysicalDeviceQueueFamilyCount, vk_pPhysicalDeviceQueueFamilyProperties);

			// Fetch surface-data about the GPU
			VkSurfaceCapabilitiesKHR vk_physicalDeviceSurfaceCapabilities;
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_hPhysicalDevice, vk_hSurface, &vk_physicalDeviceSurfaceCapabilities);

			{ // Prints information about physical device to console
				print("\t", vk_physicalDeviceProperties.deviceName, " [Driver version: ", VK_VERSION_MAJOR(vk_physicalDeviceProperties.driverVersion), '.', VK_VERSION_MINOR(vk_physicalDeviceProperties.driverVersion), '.', VK_VERSION_PATCH(vk_physicalDeviceProperties.driverVersion), "; Device type: ");
				switch (vk_physicalDeviceProperties.deviceType) {
					case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
						print("integrated GPU");
						break;
					case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
						print("discrete GPU");
						break;
					case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
						print("virtual GPU");
						break;
					case VK_PHYSICAL_DEVICE_TYPE_CPU:
						print("CPU");
						break;
					default:
						print("unknown");
						break;
				}
				println("] supporting Vulkan ", VK_VERSION_MAJOR(vk_physicalDeviceProperties.apiVersion), '.', VK_VERSION_MINOR(vk_physicalDeviceProperties.apiVersion), '.', VK_VERSION_PATCH(vk_physicalDeviceProperties.apiVersion));
				println("\t\tAvailable device extensions:");
				for (uint32_t u32PhysicalDeviceExtensionIndex = 0U; u32PhysicalDeviceExtensionIndex < u32PhysicalDeviceExtensionCount; u32PhysicalDeviceExtensionIndex++)
					println("\t\t\t", vk_pPhysicalDeviceExtensionProperties[u32PhysicalDeviceExtensionIndex].extensionName, " (", VK_VERSION_MAJOR(vk_pPhysicalDeviceExtensionProperties[u32PhysicalDeviceExtensionIndex].specVersion), '.', VK_VERSION_MINOR(vk_pPhysicalDeviceExtensionProperties[u32PhysicalDeviceExtensionIndex].specVersion), '.', VK_VERSION_PATCH(vk_pPhysicalDeviceExtensionProperties[u32PhysicalDeviceExtensionIndex].specVersion), ")");
				uint32_t u32PhysicalDeviceLayerCount;
				vkEnumerateDeviceLayerProperties(vk_hPhysicalDevice, &u32PhysicalDeviceLayerCount, nullptr);
				VkLayerProperties *vk_pPhysicalDeviceLayerProperties = new VkLayerProperties[u32PhysicalDeviceLayerCount];
				vkEnumerateDeviceLayerProperties(vk_hPhysicalDevice, &u32PhysicalDeviceLayerCount, vk_pPhysicalDeviceLayerProperties);
				println("\t\tVulkan layers present on the device (deprecated):");
				for (uint32_t u32PhysicalDeviceLayerIndex = 0U; u32PhysicalDeviceLayerIndex < u32PhysicalDeviceLayerCount; u32PhysicalDeviceLayerIndex++)
					println("\t\t\t", vk_pPhysicalDeviceLayerProperties[u32PhysicalDeviceLayerIndex].layerName, " (", VK_VERSION_MAJOR(vk_pPhysicalDeviceLayerProperties[u32PhysicalDeviceLayerIndex].implementationVersion), '.', VK_VERSION_MINOR(vk_pPhysicalDeviceLayerProperties[u32PhysicalDeviceLayerIndex].implementationVersion), '.', VK_VERSION_PATCH(vk_pPhysicalDeviceLayerProperties[u32PhysicalDeviceLayerIndex].implementationVersion), "): ", vk_pPhysicalDeviceLayerProperties[u32PhysicalDeviceLayerIndex].description);
				delete[] vk_pPhysicalDeviceLayerProperties;
				println("\t\tQueues available on the GPU:");
				for (uint32_t u32PhysicalDeviceQueueFamilyIndex = 0U; u32PhysicalDeviceQueueFamilyIndex < u32PhysicalDeviceQueueFamilyCount; u32PhysicalDeviceQueueFamilyIndex++) {
					print("\t\t\t", u32PhysicalDeviceQueueFamilyIndex, ": ");
					bool bMultiPurposeQueue = false;
#define PRINT_COMMA() bMultiPurposeQueue ? print(", ") : print(), bMultiPurposeQueue = true
					if (vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
						PRINT_COMMA();
						print("graphics");
					}
					if (vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_COMPUTE_BIT) {
						PRINT_COMMA();
						print("compute");
					}
					if (vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_TRANSFER_BIT) {
						PRINT_COMMA();
						print("transfer");
					}
					if (vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
						PRINT_COMMA();
						print("sparse binding");
					}
					if (vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR) {
						PRINT_COMMA();
						print("video decoding (extension)");
					}
					if (vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_OPTICAL_FLOW_BIT_NV) {
						PRINT_COMMA();
						print("optical flow (NVIDIA extension)");
					}
					if (vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_PROTECTED_BIT)
						bMultiPurposeQueue ? print("; (protected queue)") : print("(protected queue)");
#undef PRINT_COMMA
					println();
				}
				VkPhysicalDeviceFeatures vk_physicalDeviceFeaturesAvailable;
				vkGetPhysicalDeviceFeatures(vk_hPhysicalDevice, &vk_physicalDeviceFeaturesAvailable);
				println("\t\tFeatures supported by the GPU device:");
				println_vkbool32("robustBufferAccess", vk_physicalDeviceFeaturesAvailable.robustBufferAccess);
				println_vkbool32("fullDrawIndexUint32", vk_physicalDeviceFeaturesAvailable.fullDrawIndexUint32);
				println_vkbool32("imageCubeArray", vk_physicalDeviceFeaturesAvailable.imageCubeArray);
				println_vkbool32("independentBlend", vk_physicalDeviceFeaturesAvailable.independentBlend);
				println_vkbool32("geometryShader", vk_physicalDeviceFeaturesAvailable.geometryShader);
				println_vkbool32("tessellationShader", vk_physicalDeviceFeaturesAvailable.tessellationShader);
				println_vkbool32("sampleRateShading", vk_physicalDeviceFeaturesAvailable.sampleRateShading);
				println_vkbool32("dualSrcBlend", vk_physicalDeviceFeaturesAvailable.dualSrcBlend);
				println_vkbool32("logicOp", vk_physicalDeviceFeaturesAvailable.logicOp);
				println_vkbool32("multiDrawIndirect", vk_physicalDeviceFeaturesAvailable.multiDrawIndirect);
				println_vkbool32("drawIndirectFirstInstance", vk_physicalDeviceFeaturesAvailable.drawIndirectFirstInstance);
				println_vkbool32("depthClamp", vk_physicalDeviceFeaturesAvailable.depthClamp);
				println_vkbool32("depthBiasClamp", vk_physicalDeviceFeaturesAvailable.depthBiasClamp);
				println_vkbool32("fillModeNonSolid", vk_physicalDeviceFeaturesAvailable.fillModeNonSolid);
				println_vkbool32("depthBounds", vk_physicalDeviceFeaturesAvailable.depthBounds);
				println_vkbool32("wideLines", vk_physicalDeviceFeaturesAvailable.wideLines);
				println_vkbool32("largePoints", vk_physicalDeviceFeaturesAvailable.largePoints);
				println_vkbool32("alphaToOne", vk_physicalDeviceFeaturesAvailable.alphaToOne);
				println_vkbool32("multiViewport", vk_physicalDeviceFeaturesAvailable.multiViewport);
				println_vkbool32("samplerAnisotropy", vk_physicalDeviceFeaturesAvailable.samplerAnisotropy);
				println_vkbool32("textureCompressionETC2", vk_physicalDeviceFeaturesAvailable.textureCompressionETC2);
				println_vkbool32("textureCompressionASTC_LDR", vk_physicalDeviceFeaturesAvailable.textureCompressionASTC_LDR);
				println_vkbool32("textureCompressionBC", vk_physicalDeviceFeaturesAvailable.textureCompressionBC);
				println_vkbool32("occlusionQueryPrecise", vk_physicalDeviceFeaturesAvailable.occlusionQueryPrecise);
				println_vkbool32("pipelineStatisticsQuery", vk_physicalDeviceFeaturesAvailable.pipelineStatisticsQuery);
				println_vkbool32("vertexPipelineStoresAndAtomics", vk_physicalDeviceFeaturesAvailable.vertexPipelineStoresAndAtomics);
				println_vkbool32("fragmentStoresAndAtomics", vk_physicalDeviceFeaturesAvailable.fragmentStoresAndAtomics);
				println_vkbool32("shaderTessellationAndGeometryPointSize", vk_physicalDeviceFeaturesAvailable.shaderTessellationAndGeometryPointSize);
				println_vkbool32("shaderImageGatherExtended", vk_physicalDeviceFeaturesAvailable.shaderImageGatherExtended);
				println_vkbool32("shaderStorageImageExtendedFormats", vk_physicalDeviceFeaturesAvailable.shaderStorageImageExtendedFormats);
				println_vkbool32("shaderStorageImageMultisample", vk_physicalDeviceFeaturesAvailable.shaderStorageImageMultisample);
				println_vkbool32("shaderStorageImageReadWithoutFormat", vk_physicalDeviceFeaturesAvailable.shaderStorageImageReadWithoutFormat);
				println_vkbool32("shaderStorageImageWriteWithoutFormat", vk_physicalDeviceFeaturesAvailable.shaderStorageImageWriteWithoutFormat);
				println_vkbool32("shaderUniformBufferArrayDynamicIndexing", vk_physicalDeviceFeaturesAvailable.shaderUniformBufferArrayDynamicIndexing);
				println_vkbool32("shaderSampledImageArrayDynamicIndexing", vk_physicalDeviceFeaturesAvailable.shaderSampledImageArrayDynamicIndexing);
				println_vkbool32("shaderStorageBufferArrayDynamicIndexing", vk_physicalDeviceFeaturesAvailable.shaderStorageBufferArrayDynamicIndexing);
				println_vkbool32("shaderStorageImageArrayDynamicIndexing", vk_physicalDeviceFeaturesAvailable.shaderStorageImageArrayDynamicIndexing);
				println_vkbool32("shaderClipDistance", vk_physicalDeviceFeaturesAvailable.shaderClipDistance);
				println_vkbool32("shaderCullDistance", vk_physicalDeviceFeaturesAvailable.shaderCullDistance);
				println_vkbool32("shaderFloat64", vk_physicalDeviceFeaturesAvailable.shaderFloat64);
				println_vkbool32("shaderInt64", vk_physicalDeviceFeaturesAvailable.shaderInt64);
				println_vkbool32("shaderInt16", vk_physicalDeviceFeaturesAvailable.shaderInt16);
				println_vkbool32("shaderResourceResidency", vk_physicalDeviceFeaturesAvailable.shaderResourceResidency);
				println_vkbool32("shaderResourceMinLod", vk_physicalDeviceFeaturesAvailable.shaderResourceMinLod);
				println_vkbool32("sparseBinding", vk_physicalDeviceFeaturesAvailable.sparseBinding);
				println_vkbool32("sparseResidencyBuffer", vk_physicalDeviceFeaturesAvailable.sparseResidencyBuffer);
				println_vkbool32("sparseResidencyImage2D", vk_physicalDeviceFeaturesAvailable.sparseResidencyImage2D);
				println_vkbool32("sparseResidencyImage3D", vk_physicalDeviceFeaturesAvailable.sparseResidencyImage3D);
				println_vkbool32("sparseResidency2Samples", vk_physicalDeviceFeaturesAvailable.sparseResidency2Samples);
				println_vkbool32("sparseResidency4Samples", vk_physicalDeviceFeaturesAvailable.sparseResidency4Samples);
				println_vkbool32("sparseResidency8Samples", vk_physicalDeviceFeaturesAvailable.sparseResidency8Samples);
				println_vkbool32("sparseResidency16Samples", vk_physicalDeviceFeaturesAvailable.sparseResidency16Samples);
				println_vkbool32("sparseResidencyAliased", vk_physicalDeviceFeaturesAvailable.sparseResidencyAliased);
				println_vkbool32("variableMultisampleRate", vk_physicalDeviceFeaturesAvailable.variableMultisampleRate);
				println_vkbool32("inheritedQueries", vk_physicalDeviceFeaturesAvailable.inheritedQueries);
				println("\t\tGPU limits:");
				println("\t\t\tmaxImageDimension1D: ", vk_physicalDeviceProperties.limits.maxImageDimension1D);
				println("\t\t\tmaxImageDimension2D: ", vk_physicalDeviceProperties.limits.maxImageDimension2D);
				println("\t\t\tmaxImageDimension3D: ", vk_physicalDeviceProperties.limits.maxImageDimension3D);
				println("\t\t\tmaxImageDimensionCube: ", vk_physicalDeviceProperties.limits.maxImageDimensionCube);
				println("\t\t\tmaxImageArrayLayers: ", vk_physicalDeviceProperties.limits.maxImageArrayLayers);
				println("\t\t\tmaxTexelBufferElements: ", vk_physicalDeviceProperties.limits.maxTexelBufferElements);
				println("\t\t\tmaxUniformBufferRange: ", vk_physicalDeviceProperties.limits.maxUniformBufferRange);
				println("\t\t\tmaxStorageBufferRange: ", vk_physicalDeviceProperties.limits.maxStorageBufferRange);
				println("\t\t\tmaxPushConstantsSize: ", vk_physicalDeviceProperties.limits.maxPushConstantsSize);
				println("\t\t\tmaxMemoryAllocationCount: ", vk_physicalDeviceProperties.limits.maxMemoryAllocationCount);
				println("\t\t\tmaxSamplerAllocationCount: ", vk_physicalDeviceProperties.limits.maxSamplerAllocationCount);
				println("\t\t\tbufferImageGranularity: ", vk_physicalDeviceProperties.limits.bufferImageGranularity);
				println("\t\t\tsparseAddressSpaceSize: ", vk_physicalDeviceProperties.limits.sparseAddressSpaceSize);
				println("\t\t\tmaxBoundDescriptorSets: ", vk_physicalDeviceProperties.limits.maxBoundDescriptorSets);
				println("\t\t\tmaxPerStageDescriptorSamplers: ", vk_physicalDeviceProperties.limits.maxPerStageDescriptorSamplers);
				println("\t\t\tmaxPerStageDescriptorUniformBuffers: ", vk_physicalDeviceProperties.limits.maxPerStageDescriptorUniformBuffers);
				println("\t\t\tmaxPerStageDescriptorStorageBuffers: ", vk_physicalDeviceProperties.limits.maxPerStageDescriptorStorageBuffers);
				println("\t\t\tmaxPerStageDescriptorSampledImages: ", vk_physicalDeviceProperties.limits.maxPerStageDescriptorSampledImages);
				println("\t\t\tmaxPerStageDescriptorStorageImages: ", vk_physicalDeviceProperties.limits.maxPerStageDescriptorStorageImages);
				println("\t\t\tmaxPerStageDescriptorInputAttachments: ", vk_physicalDeviceProperties.limits.maxPerStageDescriptorInputAttachments);
				println("\t\t\tmaxPerStageResources: ", vk_physicalDeviceProperties.limits.maxPerStageResources);
				println("\t\t\tmaxDescriptorSetSamplers: ", vk_physicalDeviceProperties.limits.maxDescriptorSetSamplers);
				println("\t\t\tmaxDescriptorSetUniformBuffers: ", vk_physicalDeviceProperties.limits.maxDescriptorSetUniformBuffers);
				println("\t\t\tmaxDescriptorSetUniformBuffersDynamic: ", vk_physicalDeviceProperties.limits.maxDescriptorSetUniformBuffersDynamic);
				println("\t\t\tmaxDescriptorSetStorageBuffers: ", vk_physicalDeviceProperties.limits.maxDescriptorSetStorageBuffers);
				println("\t\t\tmaxDescriptorSetStorageBuffersDynamic: ", vk_physicalDeviceProperties.limits.maxDescriptorSetStorageBuffersDynamic);
				println("\t\t\tmaxDescriptorSetSampledImages: ", vk_physicalDeviceProperties.limits.maxDescriptorSetSampledImages);
				println("\t\t\tmaxDescriptorSetStorageImages: ", vk_physicalDeviceProperties.limits.maxDescriptorSetStorageImages);
				println("\t\t\tmaxDescriptorSetInputAttachments: ", vk_physicalDeviceProperties.limits.maxDescriptorSetInputAttachments);
				println("\t\t\tmaxVertexInputAttributes: ", vk_physicalDeviceProperties.limits.maxVertexInputAttributes);
				println("\t\t\tmaxVertexInputBindings: ", vk_physicalDeviceProperties.limits.maxVertexInputBindings);
				println("\t\t\tmaxVertexInputAttributeOffset: ", vk_physicalDeviceProperties.limits.maxVertexInputAttributeOffset);
				println("\t\t\tmaxVertexInputBindingStride: ", vk_physicalDeviceProperties.limits.maxVertexInputBindingStride);
				println("\t\t\tmaxVertexOutputComponents: ", vk_physicalDeviceProperties.limits.maxVertexOutputComponents);
				println("\t\t\tmaxTessellationGenerationLevel: ", vk_physicalDeviceProperties.limits.maxTessellationGenerationLevel);
				println("\t\t\tmaxTessellationPatchSize: ", vk_physicalDeviceProperties.limits.maxTessellationPatchSize);
				println("\t\t\tmaxTessellationControlPerVertexInputComponents: ", vk_physicalDeviceProperties.limits.maxTessellationControlPerVertexInputComponents);
				println("\t\t\tmaxTessellationControlPerVertexOutputComponents: ", vk_physicalDeviceProperties.limits.maxTessellationControlPerVertexOutputComponents);
				println("\t\t\tmaxTessellationControlPerPatchOutputComponents: ", vk_physicalDeviceProperties.limits.maxTessellationControlPerPatchOutputComponents);
				println("\t\t\tmaxTessellationControlTotalOutputComponents: ", vk_physicalDeviceProperties.limits.maxTessellationControlTotalOutputComponents);
				println("\t\t\tmaxTessellationEvaluationInputComponents: ", vk_physicalDeviceProperties.limits.maxTessellationEvaluationInputComponents);
				println("\t\t\tmaxTessellationEvaluationOutputComponents: ", vk_physicalDeviceProperties.limits.maxTessellationEvaluationOutputComponents);
				println("\t\t\tmaxGeometryShaderInvocations: ", vk_physicalDeviceProperties.limits.maxGeometryShaderInvocations);
				println("\t\t\tmaxGeometryInputComponents: ", vk_physicalDeviceProperties.limits.maxGeometryInputComponents);
				println("\t\t\tmaxGeometryOutputComponents: ", vk_physicalDeviceProperties.limits.maxGeometryOutputComponents);
				println("\t\t\tmaxGeometryOutputVertices: ", vk_physicalDeviceProperties.limits.maxGeometryOutputVertices);
				println("\t\t\tmaxGeometryTotalOutputComponents: ", vk_physicalDeviceProperties.limits.maxGeometryTotalOutputComponents);
				println("\t\t\tmaxFragmentInputComponents: ", vk_physicalDeviceProperties.limits.maxFragmentInputComponents);
				println("\t\t\tmaxFragmentOutputAttachments: ", vk_physicalDeviceProperties.limits.maxFragmentOutputAttachments);
				println("\t\t\tmaxFragmentDualSrcAttachments: ", vk_physicalDeviceProperties.limits.maxFragmentDualSrcAttachments);
				println("\t\t\tmaxFragmentCombinedOutputResources: ", vk_physicalDeviceProperties.limits.maxFragmentCombinedOutputResources);
				println("\t\t\tmaxComputeSharedMemorySize: ", vk_physicalDeviceProperties.limits.maxComputeSharedMemorySize);
				println("\t\t\tmaxComputeWorkGroupCount: ", array_to_string(vk_physicalDeviceProperties.limits.maxComputeWorkGroupCount, sizeof(vk_physicalDeviceProperties.limits.maxComputeWorkGroupCount) / sizeof(vk_physicalDeviceProperties.limits.maxComputeWorkGroupCount[0])));
				println("\t\t\tmaxComputeWorkGroupInvocations: ", vk_physicalDeviceProperties.limits.maxComputeWorkGroupInvocations);
				println("\t\t\tmaxComputeWorkGroupSize: ", array_to_string(vk_physicalDeviceProperties.limits.maxComputeWorkGroupSize, sizeof(vk_physicalDeviceProperties.limits.maxComputeWorkGroupSize) / sizeof(vk_physicalDeviceProperties.limits.maxComputeWorkGroupSize[0])));
				println("\t\t\tsubPixelPrecisionBits: ", vk_physicalDeviceProperties.limits.subPixelPrecisionBits);
				println("\t\t\tsubTexelPrecisionBits: ", vk_physicalDeviceProperties.limits.subTexelPrecisionBits);
				println("\t\t\tmipmapPrecisionBits: ", vk_physicalDeviceProperties.limits.mipmapPrecisionBits);
				println("\t\t\tmaxDrawIndexedIndexValue: ", vk_physicalDeviceProperties.limits.maxDrawIndexedIndexValue);
				println("\t\t\tmaxDrawIndirectCount: ", vk_physicalDeviceProperties.limits.maxDrawIndirectCount);
				println("\t\t\tmaxSamplerLodBias: ", vk_physicalDeviceProperties.limits.maxSamplerLodBias);
				println("\t\t\tmaxSamplerAnisotropy: ", vk_physicalDeviceProperties.limits.maxSamplerAnisotropy);
				println("\t\t\tmaxViewports: ", vk_physicalDeviceProperties.limits.maxViewports);
				println("\t\t\tmaxViewportDimensions: ", array_to_string(vk_physicalDeviceProperties.limits.maxViewportDimensions, sizeof(vk_physicalDeviceProperties.limits.maxViewportDimensions) / sizeof(vk_physicalDeviceProperties.limits.maxViewportDimensions[0])));
				println("\t\t\tviewportBoundsRange: ", array_to_string(vk_physicalDeviceProperties.limits.viewportBoundsRange, sizeof(vk_physicalDeviceProperties.limits.viewportBoundsRange) / sizeof(vk_physicalDeviceProperties.limits.viewportBoundsRange[0])));
				println("\t\t\tviewportSubPixelBits: ", vk_physicalDeviceProperties.limits.viewportSubPixelBits);
				println("\t\t\tminMemoryMapAlignment: ", vk_physicalDeviceProperties.limits.minMemoryMapAlignment);
				println("\t\t\tminTexelBufferOffsetAlignment: ", vk_physicalDeviceProperties.limits.minTexelBufferOffsetAlignment);
				println("\t\t\tminUniformBufferOffsetAlignment: ", vk_physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
				println("\t\t\tminStorageBufferOffsetAlignment: ", vk_physicalDeviceProperties.limits.minStorageBufferOffsetAlignment);
				println("\t\t\tminTexelOffset: ", vk_physicalDeviceProperties.limits.minTexelOffset);
				println("\t\t\tmaxTexelOffset: ", vk_physicalDeviceProperties.limits.maxTexelOffset);
				println("\t\t\tminTexelGatherOffset: ", vk_physicalDeviceProperties.limits.minTexelGatherOffset);
				println("\t\t\tmaxTexelGatherOffset: ", vk_physicalDeviceProperties.limits.maxTexelGatherOffset);
				println("\t\t\tminInterpolationOffset: ", vk_physicalDeviceProperties.limits.minInterpolationOffset);
				println("\t\t\tmaxInterpolationOffset: ", vk_physicalDeviceProperties.limits.maxInterpolationOffset);
				println("\t\t\tsubPixelInterpolationOffsetBits: ", vk_physicalDeviceProperties.limits.subPixelInterpolationOffsetBits);
				println("\t\t\tmaxFramebufferWidth: ", vk_physicalDeviceProperties.limits.maxFramebufferWidth);
				println("\t\t\tmaxFramebufferHeight: ", vk_physicalDeviceProperties.limits.maxFramebufferHeight);
				println("\t\t\tmaxFramebufferLayers: ", vk_physicalDeviceProperties.limits.maxFramebufferLayers);
				println("\t\t\tframebufferColorSampleCounts: ", bitmask_to_string(vk_physicalDeviceProperties.limits.framebufferColorSampleCounts));
				println("\t\t\tframebufferDepthSampleCounts: ", bitmask_to_string(vk_physicalDeviceProperties.limits.framebufferDepthSampleCounts));
				println("\t\t\tframebufferStencilSampleCounts: ", bitmask_to_string(vk_physicalDeviceProperties.limits.framebufferStencilSampleCounts));
				println("\t\t\tframebufferNoAttachmentsSampleCounts: ", bitmask_to_string(vk_physicalDeviceProperties.limits.framebufferNoAttachmentsSampleCounts));
				println("\t\t\tmaxColorAttachments: ", vk_physicalDeviceProperties.limits.maxColorAttachments);
				println("\t\t\tsampledImageColorSampleCounts: ", bitmask_to_string(vk_physicalDeviceProperties.limits.sampledImageColorSampleCounts));
				println("\t\t\tsampledImageIntegerSampleCounts: ", bitmask_to_string(vk_physicalDeviceProperties.limits.sampledImageIntegerSampleCounts));
				println("\t\t\tsampledImageDepthSampleCounts: ", bitmask_to_string(vk_physicalDeviceProperties.limits.sampledImageDepthSampleCounts));
				println("\t\t\tsampledImageStencilSampleCounts: ", bitmask_to_string(vk_physicalDeviceProperties.limits.sampledImageStencilSampleCounts));
				println("\t\t\tstorageImageSampleCounts: ", bitmask_to_string(vk_physicalDeviceProperties.limits.storageImageSampleCounts));
				println("\t\t\tmaxSampleMaskWords: ", vk_physicalDeviceProperties.limits.maxSampleMaskWords);
				println_vkbool32("timestampComputeAndGraphics: ", vk_physicalDeviceProperties.limits.timestampComputeAndGraphics);
				println("\t\t\ttimestampPeriod: ", vk_physicalDeviceProperties.limits.timestampPeriod);
				println("\t\t\tmaxClipDistances: ", vk_physicalDeviceProperties.limits.maxClipDistances);
				println("\t\t\tmaxCullDistances: ", vk_physicalDeviceProperties.limits.maxCullDistances);
				println("\t\t\tmaxCombinedClipAndCullDistances: ", vk_physicalDeviceProperties.limits.maxCombinedClipAndCullDistances);
				println("\t\t\tdiscreteQueuePriorities: ", vk_physicalDeviceProperties.limits.discreteQueuePriorities);
				println("\t\t\tpointSizeRange: ", array_to_string(vk_physicalDeviceProperties.limits.pointSizeRange, sizeof(vk_physicalDeviceProperties.limits.pointSizeRange) / sizeof(vk_physicalDeviceProperties.limits.pointSizeRange[0])));
				println("\t\t\tlineWidthRange: ", array_to_string(vk_physicalDeviceProperties.limits.lineWidthRange, sizeof(vk_physicalDeviceProperties.limits.lineWidthRange) / sizeof(vk_physicalDeviceProperties.limits.lineWidthRange[0])));
				println("\t\t\tpointSizeGranularity: ", vk_physicalDeviceProperties.limits.pointSizeGranularity);
				println("\t\t\tlineWidthGranularity: ", vk_physicalDeviceProperties.limits.lineWidthGranularity);
				println_vkbool32("strictLines: ", vk_physicalDeviceProperties.limits.strictLines);
				println_vkbool32("standardSampleLocations: ", vk_physicalDeviceProperties.limits.standardSampleLocations);
				println("\t\t\toptimalBufferCopyOffsetAlignment: ", vk_physicalDeviceProperties.limits.optimalBufferCopyOffsetAlignment);
				println("\t\t\toptimalBufferCopyRowPitchAlignment: ", vk_physicalDeviceProperties.limits.optimalBufferCopyRowPitchAlignment);
				println("\t\t\tnonCoherentAtomSize: ", vk_physicalDeviceProperties.limits.nonCoherentAtomSize);
				println("\t\tSupported surface capabilities:");
				println("\t\t\tsupportedTransforms: ", bitmask_to_string(vk_physicalDeviceSurfaceCapabilities.supportedTransforms));
				println("\t\t\tsupportedCompositeAlpha: ", bitmask_to_string(vk_physicalDeviceSurfaceCapabilities.supportedCompositeAlpha));
				println("\t\t\tsupportedUsageFlags: ", bitmask_to_string(vk_physicalDeviceSurfaceCapabilities.supportedUsageFlags));
			} // End of printing information about physical device to console


			std::queue<const char*> missingFeatures;

			// Check if there are surface formats defined
			uint32_t u32PhysicalDeviceSurfaceFormatCount;
			vkGetPhysicalDeviceSurfaceFormatsKHR(vk_hPhysicalDevice, vk_hSurface, &u32PhysicalDeviceSurfaceFormatCount, nullptr);
			if (!u32PhysicalDeviceSurfaceFormatCount)
				missingFeatures.push("No surface formats available");

			// Check if there are present modes defined
			uint32_t u32PhysicalDevicePresentModeCount;
			vkGetPhysicalDeviceSurfacePresentModesKHR(vk_hPhysicalDevice, vk_hSurface, &u32PhysicalDevicePresentModeCount, nullptr);
			if (!u32PhysicalDevicePresentModeCount)
				missingFeatures.push("No presentation modes available");

			// Check if swapchain images support the required image usages
			if ((vk_physicalDeviceSurfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) == 0 || (vk_physicalDeviceSurfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) == 0)
				missingFeatures.push("Swapchain images neither support being used for color attachments and/or transfer operations");

			// Check if the required extensions exist
			bool bSwapchainExtists = false;
			for (uint32_t u32PhysicalDeviceExtensionIndex = 0U; u32PhysicalDeviceExtensionIndex < u32PhysicalDeviceExtensionCount; u32PhysicalDeviceExtensionIndex++) {
				if (!bSwapchainExtists && are_string_contents_equal(vk_pPhysicalDeviceExtensionProperties[u32PhysicalDeviceExtensionIndex].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME))
					bSwapchainExtists = true;
				if (bSwapchainExtists)
					break;
			}
			if (!bSwapchainExtists)
				missingFeatures.push("The swapchain extension doesn't exist on this GPU");

			// Check if required queues exist
			bool bGraphicsQueueExists = false, bPresentQueueExists = false, bTransferQueueExists = false;
			for (uint32_t u32PhysicalDeviceQueueFamilyIndex = 0U; u32PhysicalDeviceQueueFamilyIndex < u32PhysicalDeviceQueueFamilyCount; u32PhysicalDeviceQueueFamilyIndex++) {
				if (!bPresentQueueExists) {
					VkBool32 surfaceSupportExists;
					vkGetPhysicalDeviceSurfaceSupportKHR(vk_hPhysicalDevice, u32PhysicalDeviceQueueFamilyIndex, vk_hSurface, &surfaceSupportExists);
					if (surfaceSupportExists)
						bPresentQueueExists = true;
				}
				if (!bGraphicsQueueExists && (vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT))) {
					bGraphicsQueueExists = true;
					bTransferQueueExists = true;
				} else if (!bTransferQueueExists && (vk_pPhysicalDeviceQueueFamilyProperties[u32PhysicalDeviceQueueFamilyIndex].queueFlags & VK_QUEUE_TRANSFER_BIT))
					bTransferQueueExists = true;
				if (bGraphicsQueueExists && bPresentQueueExists && bTransferQueueExists)
					break;
			}
			if (!bGraphicsQueueExists)
				missingFeatures.push("A graphics queue supporting transfers either doesn't exist");
			if (!bPresentQueueExists)
				missingFeatures.push("The presentation queue doesn't exist");
			if (!bTransferQueueExists)
				missingFeatures.push("A dedicated transfer queue or a graphics queue supporting transfers aswell doesn't exist");

			delete[] vk_pPhysicalDeviceExtensionProperties;
			delete[] vk_pPhysicalDeviceQueueFamilyProperties;
			if (!missingFeatures.empty()) {
				println_colored(append_to_string("\tPhysical Vulkan device ", vk_physicalDeviceProperties.deviceName, " has been discarded for not fulfilling the requirement(s):").c_str(), RE_TERMINAL_COLOR_RED, false, true);
				do {
					println_colored(append_to_string("\t - ", missingFeatures.front()).c_str(), RE_TERMINAL_COLOR_RED, false, true);
					missingFeatures.pop();
				} while (!missingFeatures.empty());
				continue;
			} else
				suitablePhysicalDevices.push(vk_hPhysicalDevice);
		}
		delete[] vk_phTotalPhysicalDevice;
		u32PhysicalDevicesAvailableCount = suitablePhysicalDevices.size();
		if (!u32PhysicalDevicesAvailableCount) {
			RE_FATAL_ERROR("There aren't any physical Vulkan devices supporting the necessary features");
			return false;
		}
		vk_phPhysicalDevicesAvailable = new VkPhysicalDevice[u32PhysicalDevicesAvailableCount];
		uint32_t u32CurrentIndex = 0U;
		do {
			vk_phPhysicalDevicesAvailable[u32CurrentIndex] = suitablePhysicalDevices.front();
			u32CurrentIndex++;
			suitablePhysicalDevices.pop();
		} while (!suitablePhysicalDevices.empty());
		return true;
	}

	static void free_physical_device_list() {
		DELETE_ARRAY_SAFELY(vk_phPhysicalDevicesAvailable);
	}

	static bool setup_interfaces_to_device() {
		uint32_t u32PhysicalDeviceSelectedQueueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(vk_hPhysicalDeviceSelected, &u32PhysicalDeviceSelectedQueueFamilyCount, nullptr);
		VkQueueFamilyProperties *vk_pPhysicalDeviceSelectedQueueFamilies = new VkQueueFamilyProperties[u32PhysicalDeviceSelectedQueueFamilyCount];
		vkGetPhysicalDeviceQueueFamilyProperties(vk_hPhysicalDeviceSelected, &u32PhysicalDeviceSelectedQueueFamilyCount, vk_pPhysicalDeviceSelectedQueueFamilies);
		CATCH_SIGNAL(set_bit<uint8_t>(u8RenderSystemFlags, GRAPHICS_QUEUE_SUPPORTS_TRANSFER_BIT, (vk_pPhysicalDeviceSelectedQueueFamilies[u32DeviceQueueFamilyIndices[RE_VK_QUEUE_GRAPHICS_INDEX]].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0U));
		delete[] vk_pPhysicalDeviceSelectedQueueFamilies;

		vkGetDeviceQueue(vk_hDevice, u32DeviceQueueFamilyIndices[RE_VK_QUEUE_GRAPHICS_INDEX], 0, &vk_hDeviceQueueFamilies[RE_VK_QUEUE_GRAPHICS_INDEX]);
		vkGetDeviceQueue(vk_hDevice, u32DeviceQueueFamilyIndices[RE_VK_QUEUE_PRESENT_INDEX], 0, &vk_hDeviceQueueFamilies[RE_VK_QUEUE_PRESENT_INDEX]);
		vkGetDeviceQueue(vk_hDevice, u32DeviceQueueFamilyIndices[RE_VK_QUEUE_TRANSFER_INDEX], 0, &vk_hDeviceQueueFamilies[RE_VK_QUEUE_TRANSFER_INDEX]);

		VkCommandPoolCreateInfo vk_commandPoolCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = u32DeviceQueueFamilyIndices[RE_VK_QUEUE_GRAPHICS_INDEX]
		};
		vkCreateCommandPool(vk_hDevice, &vk_commandPoolCreateInfo, nullptr, &vk_hCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX]);
		vk_commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		vkCreateCommandPool(vk_hDevice, &vk_commandPoolCreateInfo, nullptr, &vk_hCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_TRANSIENT_INDEX]);
		vk_commandPoolCreateInfo.queueFamilyIndex = u32DeviceQueueFamilyIndices[RE_VK_QUEUE_TRANSFER_INDEX];
		vkCreateCommandPool(vk_hDevice, &vk_commandPoolCreateInfo, nullptr, &vk_hCommandPools[RE_VK_COMMAND_POOL_TRANSFER_TRANSIENT_INDEX]);
		vk_commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		vkCreateCommandPool(vk_hDevice, &vk_commandPoolCreateInfo, nullptr, &vk_hCommandPools[RE_VK_COMMAND_POOL_TRANSFER_PERSISTENT_INDEX]);
		const VkCommandBufferAllocateInfo vk_commandBufferAllocInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = vk_hCommandPools[RE_VK_COMMAND_POOL_TRANSFER_PERSISTENT_INDEX],
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1U
		};
		if (!vkAllocateCommandBuffers(vk_hDevice, &vk_commandBufferAllocInfo, &vk_hDummyTransferCommandBuffer))
			return false;
		const VkCommandBufferBeginInfo vk_beginRecordingCommandBuffer = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT
		};
		if (!vkBeginCommandBuffer(vk_hDummyTransferCommandBuffer, &vk_beginRecordingCommandBuffer))
			return false;
		vkEndCommandBuffer(vk_hDummyTransferCommandBuffer);
		return true;
	}

	static void destroy_interfaces_to_device() {
		vk_hDummyTransferCommandBuffer = VK_NULL_HANDLE;
		for (uint8_t u8CommandPoolIndex = 0U; u8CommandPoolIndex < RE_VK_COMMAND_POOL_COUNT; u8CommandPoolIndex++) {
			vkDestroyCommandPool(vk_hDevice, vk_hCommandPools[u8CommandPoolIndex], nullptr);
			vk_hCommandPools[u8CommandPoolIndex] = VK_NULL_HANDLE;
		}
		for (uint8_t u8QueueFamilyIndex = 0U; u8QueueFamilyIndex < RE_VK_QUEUE_COUNT; u8QueueFamilyIndex++) {
			u32DeviceQueueFamilyIndices[u8QueueFamilyIndex] = 0U;
			vk_hDeviceQueueFamilies[u8QueueFamilyIndex] = VK_NULL_HANDLE;
		}
	}

	static bool create_swapchain() {
		// Create actual sweapchain
		const VkSwapchainKHR vk_hOldSwapchain = vk_hSwapchain;
		if (vk_hOldSwapchain) {
			CATCH_SIGNAL(swapchain_destroyed_renderer());
			for (uint32_t u32SwapchainImageIndex = 0U; u32SwapchainImageIndex < u32SwapchainImageCount; u32SwapchainImageIndex++)
				vkDestroyImageView(vk_hDevice, vk_phSwapchainImageViews[u32SwapchainImageIndex], nullptr);
			DELETE_ARRAY_SAFELY(vk_phSwapchainImages);
			DELETE_ARRAY_SAFELY(vk_phSwapchainImageViews);
		}
		if (vk_surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			vk_swapchainResolution = vk_surfaceCapabilities.currentExtent;
		else {
			vk_swapchainResolution.width = std::clamp<uint32_t>(windowSize[0], vk_surfaceCapabilities.minImageExtent.width, vk_surfaceCapabilities.maxImageExtent.width);
			vk_swapchainResolution.height = std::clamp<uint32_t>(windowSize[1], vk_surfaceCapabilities.minImageExtent.height, vk_surfaceCapabilities.maxImageExtent.height);
		}
		VkSwapchainCreateInfoKHR vk_swapchainCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = vk_hSurface,
			.minImageCount = std::clamp(vk_surfaceCapabilities.minImageCount + 1U, vk_surfaceCapabilities.minImageCount, vk_surfaceCapabilities.maxImageCount > 0U ? vk_surfaceCapabilities.maxImageCount : std::numeric_limits<uint32_t>::max()),
			.imageFormat = vk_surfaceFormatSelected.format,
			.imageColorSpace = vk_surfaceFormatSelected.colorSpace,
			.imageExtent = vk_swapchainResolution,
			.imageArrayLayers = 1U,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			.preTransform = vk_surfaceCapabilities.currentTransform,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.clipped = VK_TRUE,
			.oldSwapchain = vk_hOldSwapchain
		};
		const uint32_t u32SwapchainRelevantQueueIndices[2] = {u32DeviceQueueFamilyIndices[RE_VK_QUEUE_GRAPHICS_INDEX], u32DeviceQueueFamilyIndices[RE_VK_QUEUE_PRESENT_INDEX]};
		if (u32DeviceQueueFamilyIndices[RE_VK_QUEUE_GRAPHICS_INDEX] != u32DeviceQueueFamilyIndices[RE_VK_QUEUE_PRESENT_INDEX]) {
			vk_swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			vk_swapchainCreateInfo.queueFamilyIndexCount = 2U;
			vk_swapchainCreateInfo.pQueueFamilyIndices = u32SwapchainRelevantQueueIndices;
		} else
			vk_swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		if (is_bit_true<uint8_t>(u8RenderSystemFlags, FPS_BOUND_TO_VSYNC_BIT))
			vk_swapchainCreateInfo.presentMode = is_bit_true<uint8_t>(u8RenderSystemFlags, VSYNC_SETTING_BIT) ? VK_PRESENT_MODE_FIFO_KHR : vk_ePresentModeNoVsync;
		else
			vk_swapchainCreateInfo.presentMode = is_bit_true<uint8_t>(u8RenderSystemFlags, VSYNC_SETTING_BIT) ? vk_ePresentModeVsync : vk_ePresentModeNoVsync;
		if (!vkCreateSwapchainKHR(vk_hDevice, &vk_swapchainCreateInfo, nullptr, &vk_hSwapchain)) {
			RE_ERROR("Failed creating Vulkan swapchain");
			return false;
		}
		vk_eSwapchainImageFormat = vk_surfaceFormatSelected.format;
		if (vk_hOldSwapchain != VK_NULL_HANDLE)
			vkDestroySwapchainKHR(vk_hDevice, vk_hOldSwapchain, nullptr);
		vkGetSwapchainImagesKHR(vk_hDevice, vk_hSwapchain, &u32SwapchainImageCount, nullptr);
		vk_phSwapchainImages = new VkImage[u32SwapchainImageCount];
		vkGetSwapchainImagesKHR(vk_hDevice, vk_hSwapchain, &u32SwapchainImageCount, vk_phSwapchainImages);
		// End of creating actual swapchain

		// Create swapchain image views
		vk_phSwapchainImageViews = new VkImageView[u32SwapchainImageCount];
		{
			uint32_t u32SwapchainImageViewsCreated = 0U;
			while (u32SwapchainImageViewsCreated < u32SwapchainImageCount) {
				if (!create_vulkan_image_view(vk_phSwapchainImages[u32SwapchainImageViewsCreated], VK_IMAGE_VIEW_TYPE_2D, vk_eSwapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 0U, 1U, 0U, 1U, &vk_phSwapchainImageViews[u32SwapchainImageViewsCreated])) {
					RE_FATAL_ERROR(append_to_string("Failed to create Vulkan image view at index ", u32SwapchainImageViewsCreated));
					break;
				}
				u32SwapchainImageViewsCreated++;
				continue;
			}
			if (u32SwapchainImageViewsCreated != u32SwapchainImageCount) {
				for (uint32_t u32SwapchainImageDeleteIndex = 0U; u32SwapchainImageDeleteIndex < u32SwapchainImageViewsCreated; u32SwapchainImageDeleteIndex++)
					vkDestroyImageView(vk_hDevice, vk_phSwapchainImageViews[u32SwapchainImageDeleteIndex], nullptr);
				DELETE_ARRAY_SAFELY(vk_phSwapchainImages);
				DELETE_ARRAY_SAFELY(vk_phSwapchainImageViews);
				vkDestroySwapchainKHR(vk_hDevice, vk_hSwapchain, nullptr);
				vk_hSwapchain = VK_NULL_HANDLE;
				return false;
			}
		} // End of creating swapchain image views
		return true;
	}
	
	static void destroy_swapchain() {
		CATCH_SIGNAL(swapchain_destroyed_renderer());
		for (uint32_t u32SwapchainImageIndex = 0U; u32SwapchainImageIndex < u32SwapchainImageCount; u32SwapchainImageIndex++)
			vkDestroyImageView(vk_hDevice, vk_phSwapchainImageViews[u32SwapchainImageIndex], nullptr);
		DELETE_ARRAY_SAFELY(vk_phSwapchainImages);
		DELETE_ARRAY_SAFELY(vk_phSwapchainImageViews);
		vkDestroySwapchainKHR(vk_hDevice, vk_hSwapchain, nullptr);
		vk_hSwapchain = VK_NULL_HANDLE;
	}

	static bool recreate_swapchain() {
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_hPhysicalDeviceSelected, vk_hSurface, &vk_surfaceCapabilities);
		WAIT_FOR_IDLE_VULKAN_DEVICE();
		if (!CATCH_SIGNAL_AND_RETURN(create_swapchain() && swapchain_created_renderer(), bool)) {
			RE_ERROR("Failed recreating the swapchain");
			return false;
		}
		return true;
	}

	bool init_render_system() {
		if (CATCH_SIGNAL_AND_RETURN(create_surface(), bool)) {
			if (CATCH_SIGNAL_AND_RETURN(alloc_physical_device_list(), bool)) {
				CATCH_SIGNAL(select_best_physical_vulkan_device());
				VkPhysicalDeviceProperties vk_physicalDeviceSelectedProperties;
				vkGetPhysicalDeviceProperties(vk_hPhysicalDeviceSelected, &vk_physicalDeviceSelectedProperties);
				PRINT_LN(append_to_string("Device selected: ", vk_physicalDeviceSelectedProperties.deviceName).c_str());
				vkGetPhysicalDeviceMemoryProperties(vk_hPhysicalDeviceSelected, &vk_physicalDeviceMemoryProperties);
				CATCH_SIGNAL(fetch_surface_infos());
				CATCH_SIGNAL(select_best_surface_format());
				if (CATCH_SIGNAL_AND_RETURN(init_vulkan_device(), bool)) {
					if (CATCH_SIGNAL_AND_RETURN(setup_interfaces_to_device(), bool)) {
						if (CATCH_SIGNAL_AND_RETURN(create_swapchain(), bool))
							return true;
						CATCH_SIGNAL(destroy_interfaces_to_device());
					}
					CATCH_SIGNAL(destroy_vulkan_device());
				}
				CATCH_SIGNAL(free_physical_device_list());
			}
			CATCH_SIGNAL(destroy_surface());
		}
		return false;
	}

	void destroy_render_system() {
		CATCH_SIGNAL(destroy_swapchain());
		CATCH_SIGNAL(destroy_interfaces_to_device());
		CATCH_SIGNAL(destroy_vulkan_device());
		vk_hPhysicalDeviceSelected = VK_NULL_HANDLE;
		CATCH_SIGNAL(free_physical_device_list());
		CATCH_SIGNAL(destroy_surface());
	}

	bool refresh_swapchain() {
		if (is_bit_true<uint8_t>(u8RenderSystemFlags, SWAPCHAIN_DIRTY_BIT)) {
			if (!CATCH_SIGNAL_AND_RETURN(recreate_swapchain(), bool))
				return false;
			set_bit<uint8_t>(u8RenderSystemFlags, SWAPCHAIN_DIRTY_BIT, false);
		}
		return true;
	}

	void mark_swapchain_dirty() {
		set_bit<uint8_t>(u8RenderSystemFlags, SWAPCHAIN_DIRTY_BIT, bRunning);
	}

	bool does_graphics_queue_support_transfer() {
		return is_bit_true<uint8_t>(u8RenderSystemFlags, SWAPCHAIN_DIRTY_BIT);
	}

	void enable_vsync(bool bEnableVsync) {
		if (is_bit_true<uint8_t>(u8RenderSystemFlags, VSYNC_SETTING_BIT) != bEnableVsync) {
			set_bit<uint8_t>(u8RenderSystemFlags, VSYNC_SETTING_BIT, bEnableVsync);
			set_bit<uint8_t>(u8RenderSystemFlags, SWAPCHAIN_DIRTY_BIT, bRunning);
		}
	}

	bool is_vsync_enabled() {
		return is_bit_true<uint8_t>(u8RenderSystemFlags, VSYNC_SETTING_BIT);
	}

	void bind_fps_to_vsync(bool bBindFpsToVsync) {
		if (is_bit_true<uint8_t>(u8RenderSystemFlags, FPS_BOUND_TO_VSYNC_BIT) != bBindFpsToVsync) {
			set_bit<uint8_t>(u8RenderSystemFlags, FPS_BOUND_TO_VSYNC_BIT, bBindFpsToVsync);
			set_bit<uint8_t>(u8RenderSystemFlags, SWAPCHAIN_DIRTY_BIT, bRunning);
		}
	}

	bool is_fps_bound_to_vsync() {
		return is_bit_true<uint8_t>(u8RenderSystemFlags, FPS_BOUND_TO_VSYNC_BIT);
	}

}
