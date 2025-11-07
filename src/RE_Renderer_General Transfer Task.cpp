#include "RE_Renderer_Internal.hpp"

namespace RE {
	
	static VulkanTask transferToGpuTask;
	static VkSemaphore vk_hTransferTimelineSemaphore;
	static uint64_t u64TransferTimelineSemaphoreValue = 0;
	static constexpr VkSemaphoreWaitInfo vk_waitForTransfers = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
		.semaphoreCount = 1,
		.pSemaphores = &vk_hTransferTimelineSemaphore,
		.pValues = &u64TransferTimelineSemaphoreValue
	};

	bool init_general_transfer_task() {
		PRINT_DEBUG("Creating Vulkan timeline semaphore for waiting between general transfer tasks");
		const VkSemaphoreTypeCreateInfo vk_timelineSemaphoreCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
			.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE
		};
		const VkSemaphoreCreateInfo vk_semaphoreCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.pNext = &vk_timelineSemaphoreCreateInfo
		};
		if (vkCreateSemaphore(vk_hDevice, &vk_semaphoreCreateInfo, nullptr, &vk_hTransferTimelineSemaphore) == VK_SUCCESS) {
			PRINT_DEBUG("Initializing general transfer task");
			const uint8_t au8LogicalQueues[] = {RE_VK_LOGICAL_QUEUE_IGNORED, get_render_graphics_queue_logical_index()};
			constexpr VkQueueFlagBits vk_aeQueueTypes[] = {VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT};
			constexpr uint32_t au32SeparationIds[] = {0, 1};
			const VulkanTask_Queues queuesToUse = {
				.pau8LogicalQueueIndices = au8LogicalQueues,
				.vk_paeQueueTypes = vk_aeQueueTypes,
				.pau32StrictSeparationIds = au32SeparationIds,
				.u32FunctionsCount = 2
			};
			if (transferToGpuTask.init(queuesToUse, false))
				return true;
			else
				RE_FATAL_ERROR("Failed initializing general transfer task");
		} else
			RE_FATAL_ERROR("Failed creating Vulkan timeline semaphore for waiting between general timeline semaphore");
		return false;
	}

	void destroy_general_transfer_task() {
		wait_for_transfer(std::numeric_limits<uint64_t>::max());
		transferToGpuTask.destroy();
		vkDestroySemaphore(vk_hDevice, vk_hTransferTimelineSemaphore, nullptr);
	}

	bool submit_transfer_task(const VkCommandBufferUsageFlags vk_eUsageFlags, const std::function<void (VkCommandBuffer vk_hCommandBuffer, uint8_t u8PreviousLogicalQueue, uint8_t u8CurrentLogicalQueue, uint8_t u8NextLogicalQueue)> pTransferFunction, const std::function<void (VkCommandBuffer vk_hCommandBuffer, uint8_t u8PreviousLogicalQueue, uint8_t u8CurrentLogicalQueue, uint8_t u8NextLogicalQueue)> pOwnershipAcquireFunction, const uint32_t u32WaitSemaphoreCount, const VkSemaphoreSubmitInfo *const vk_paWaitSemaphores, const uint32_t u32SignalSemaphoreCount, const VkSemaphoreSubmitInfo *const vk_paSignalSemaphores, const VkFence vk_hFence) {
		std::vector<VkSemaphoreSubmitInfo> signalSemaphores;
		signalSemaphores.resize(u32SignalSemaphoreCount + 1);
		std::jthread copySignalSemaphoresThread([&]() {
			std::memcpy(signalSemaphores.data(), vk_paSignalSemaphores, u32SignalSemaphoreCount * sizeof(VkSemaphoreSubmitInfo));
		});
		signalSemaphores.back().sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		signalSemaphores.back().pNext = nullptr;
		signalSemaphores.back().semaphore = vk_hTransferTimelineSemaphore;
		signalSemaphores.back().stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		signalSemaphores.back().deviceIndex = 0;
		wait_for_transfer(std::numeric_limits<uint64_t>::max());
		transferToGpuTask.paFunctions[0] = pTransferFunction;
		transferToGpuTask.paFunctions[1] = pOwnershipAcquireFunction;
		transferToGpuTask.record(vk_eUsageFlags);
		u64TransferTimelineSemaphoreValue++;
		signalSemaphores.back().value = u64TransferTimelineSemaphoreValue;
		copySignalSemaphoresThread.join();
		return transferToGpuTask.submit(u32WaitSemaphoreCount, vk_paWaitSemaphores, signalSemaphores.size(), signalSemaphores.data(), vk_hFence);
	}

	void wait_for_transfer(const uint64_t u64Timeout) {
		PRINT_DEBUG("Waiting for Vulkan timeline semaphore to reach ", u64TransferTimelineSemaphoreValue, " signaling recent transfer task has finished");
		vkWaitSemaphores(vk_hDevice, &vk_waitForTransfers, u64Timeout);
	}

}
