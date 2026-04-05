#include "RE_GPU_Internal.hpp"

namespace RE {
	
	PFN_vkCmdSetEvent2 vkCmdSetEvent2 = nullptr;
	PFN_vkCmdResetEvent2 vkCmdResetEvent2 = nullptr;
	PFN_vkCmdWaitEvents2 vkCmdWaitEvents2 = nullptr;
	PFN_vkCmdPipelineBarrier2 vkCmdPipelineBarrier2 = nullptr;
	PFN_vkCmdWriteTimestamp2 vkCmdWriteTimestamp2 = nullptr;
	PFN_vkQueueSubmit2 vkQueueSubmit2 = nullptr;

	static VkPipelineStageFlags downgrade_pipeline_stage_flags_2(const VkPipelineStageFlags2 vk_mPipelineStageFlags2) {
		constexpr VkPipelineStageFlags2 vk_mConvertablePipelineStageFlags2 = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
				| VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT
				| VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT
				| VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT
				| VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT
				| VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT
				| VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT
				| VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT
				| VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT
				| VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT
				| VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT
				| VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT
				| VK_PIPELINE_STAGE_2_TRANSFER_BIT
				| VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT
				| VK_PIPELINE_STAGE_2_HOST_BIT
				| VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT
				| VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT
				| VK_PIPELINE_STAGE_2_COMMAND_PREPROCESS_BIT_NV
				| VK_PIPELINE_STAGE_2_CONDITIONAL_RENDERING_BIT_EXT
				| VK_PIPELINE_STAGE_2_TASK_SHADER_BIT_EXT
				| VK_PIPELINE_STAGE_2_MESH_SHADER_BIT_EXT
				| VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR
				| VK_PIPELINE_STAGE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR
				| VK_PIPELINE_STAGE_2_FRAGMENT_DENSITY_PROCESS_BIT_EXT
				| VK_PIPELINE_STAGE_2_TRANSFORM_FEEDBACK_BIT_EXT
				| VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
		if ((vk_mPipelineStageFlags2 & (~vk_mConvertablePipelineStageFlags2)) != 0)
			RE_ABORT("Pipeline stage flags provided by the synchronization 2-feature cannot be converted into standard pipeline stage flags");
		return static_cast<VkPipelineStageFlags>(vk_mPipelineStageFlags2 & vk_mConvertablePipelineStageFlags2);
	}

	static VkAccessFlags downgrade_access_flags_2(const VkAccessFlags2 vk_mAccessFlags2) {
		constexpr VkAccessFlags2 vk_mConvertableAccessFlags2 = VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT
				| VK_ACCESS_2_INDEX_READ_BIT
				| VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT
				| VK_ACCESS_2_UNIFORM_READ_BIT
				| VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT
				| VK_ACCESS_2_SHADER_READ_BIT
				| VK_ACCESS_2_SHADER_WRITE_BIT
				| VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT
				| VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT
				| VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT
				| VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
				| VK_ACCESS_2_TRANSFER_READ_BIT
				| VK_ACCESS_2_TRANSFER_WRITE_BIT
				| VK_ACCESS_2_HOST_READ_BIT
				| VK_ACCESS_2_HOST_WRITE_BIT
				| VK_ACCESS_2_MEMORY_READ_BIT
				| VK_ACCESS_2_MEMORY_WRITE_BIT
				| VK_ACCESS_2_TRANSFORM_FEEDBACK_WRITE_BIT_EXT
				| VK_ACCESS_2_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT
				| VK_ACCESS_2_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT
				| VK_ACCESS_2_CONDITIONAL_RENDERING_READ_BIT_EXT
				| VK_ACCESS_2_COMMAND_PREPROCESS_READ_BIT_NV
				| VK_ACCESS_2_COMMAND_PREPROCESS_WRITE_BIT_NV
				| VK_ACCESS_2_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR
				| VK_ACCESS_2_FRAGMENT_DENSITY_MAP_READ_BIT_EXT
				| VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR
				| VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
		if ((vk_mAccessFlags2 & (~vk_mConvertableAccessFlags2)) != 0)
			RE_ABORT("Access flags provided by the synchronization 2-feature cannot be converted into standard access flags");
		return static_cast<VkAccessFlags>(vk_mAccessFlags2 & vk_mConvertableAccessFlags2);
	}

	[[noreturn]]
	static void vk_cmd_set_event_2(const VkCommandBuffer vk_hCommandBuffer, const VkEvent vk_hEvent, const VkDependencyInfo *const vk_pDependencyInfo) {
		RE_ABORT("The synchronization 2-feature is not enabled or loading the Vulkan function \"vkCmdSetEvent2\" failed. It cannot be downgraded to the older variant");
	}

	static void vk_cmd_reset_event_2(const VkCommandBuffer vk_hCommandBuffer, const VkEvent vk_hEvent, const VkPipelineStageFlags2 vk_mStageMask) {
		vkCmdResetEvent(vk_hCommandBuffer, vk_hEvent, downgrade_pipeline_stage_flags_2(vk_mStageMask));
	}

	[[noreturn]]
	static void vk_cmd_wait_event_2(const VkCommandBuffer vk_hCommandBuffer, const uint32_t u32EventCount, const VkEvent *const vk_pahEvents, const VkDependencyInfo *const vk_paDependencyInfos) {
		RE_ABORT("The synchronization 2-feature is not enabled or loading the Vulkan function \"vkCmdWaitEvent2\" failed. It cannot be downgraded to the older variant");
	}

	static void vk_cmd_pipeline_barrier_2(const VkCommandBuffer vk_hCommandBuffer, const VkDependencyInfo *const vk_pDependencyInfo) {
		if (vk_pDependencyInfo->pNext)
			RE_ABORT("Cannot include data structures extending dependency information for pipeline barrier");

		class Barrier final {
			public:
				const VkPipelineStageFlags2 vk_mSrcStages,
						vk_mDstStages;
				std::vector<VkMemoryBarrier> memoryBarriers;
				std::vector<VkBufferMemoryBarrier> bufferBarriers;
				std::vector<VkImageMemoryBarrier> imageBarriers;

				Barrier() = delete;
				Barrier(const VkPipelineStageFlags2 vk_mSrcStages, const VkPipelineStageFlags2 vk_mDstStages) : 
						vk_mSrcStages(vk_mSrcStages),
						vk_mDstStages(vk_mDstStages) {
					memoryBarriers.reserve(3);
					bufferBarriers.reserve(3);
					imageBarriers.reserve(3);
				}
				~Barrier() {}
		};
		std::deque<Barrier> barriers;

		for (uint32_t u32MemoryBarrierIndex = 0; u32MemoryBarrierIndex < vk_pDependencyInfo->memoryBarrierCount; u32MemoryBarrierIndex++) {
			auto barrierIterator = std::find_if(barriers.begin(), barriers.end(), [&](const Barrier &rBarrier) {
						return rBarrier.vk_mSrcStages == vk_pDependencyInfo->pMemoryBarriers[u32MemoryBarrierIndex].srcStageMask
								&& rBarrier.vk_mDstStages == vk_pDependencyInfo->pMemoryBarriers[u32MemoryBarrierIndex].dstStageMask;
					});
			if (barrierIterator == barriers.end()) {
				barriers.emplace_back(
						vk_pDependencyInfo->pMemoryBarriers[u32MemoryBarrierIndex].srcStageMask,
						vk_pDependencyInfo->pMemoryBarriers[u32MemoryBarrierIndex].dstStageMask);
				barriers.back().memoryBarriers.emplace_back(
						VK_STRUCTURE_TYPE_MEMORY_BARRIER,
						vk_pDependencyInfo->pMemoryBarriers[u32MemoryBarrierIndex].pNext,
						downgrade_access_flags_2(vk_pDependencyInfo->pMemoryBarriers[u32MemoryBarrierIndex].srcAccessMask),
						downgrade_access_flags_2(vk_pDependencyInfo->pMemoryBarriers[u32MemoryBarrierIndex].dstAccessMask));
			} else
				barrierIterator->memoryBarriers.emplace_back(
						VK_STRUCTURE_TYPE_MEMORY_BARRIER,
						vk_pDependencyInfo->pMemoryBarriers[u32MemoryBarrierIndex].pNext,
						downgrade_access_flags_2(vk_pDependencyInfo->pMemoryBarriers[u32MemoryBarrierIndex].srcAccessMask),
						downgrade_access_flags_2(vk_pDependencyInfo->pMemoryBarriers[u32MemoryBarrierIndex].dstAccessMask));
		}

		for (uint32_t u32BufferBarrierIndex = 0; u32BufferBarrierIndex < vk_pDependencyInfo->bufferMemoryBarrierCount; u32BufferBarrierIndex++) {
			auto barrierIterator = std::find_if(barriers.begin(), barriers.end(), [&](const Barrier &rBarrier) {
						return rBarrier.vk_mSrcStages == vk_pDependencyInfo->pBufferMemoryBarriers[u32BufferBarrierIndex].srcStageMask
								&& rBarrier.vk_mDstStages == vk_pDependencyInfo->pBufferMemoryBarriers[u32BufferBarrierIndex].dstStageMask;
					});
			if (barrierIterator == barriers.end()) {
				barriers.emplace_back(
						vk_pDependencyInfo->pBufferMemoryBarriers[u32BufferBarrierIndex].srcStageMask,
						vk_pDependencyInfo->pBufferMemoryBarriers[u32BufferBarrierIndex].dstStageMask);
				barriers.back().bufferBarriers.emplace_back(
						VK_STRUCTURE_TYPE_MEMORY_BARRIER,
						vk_pDependencyInfo->pBufferMemoryBarriers[u32BufferBarrierIndex].pNext,
						downgrade_access_flags_2(vk_pDependencyInfo->pBufferMemoryBarriers[u32BufferBarrierIndex].srcAccessMask),
						downgrade_access_flags_2(vk_pDependencyInfo->pBufferMemoryBarriers[u32BufferBarrierIndex].dstAccessMask),
						vk_pDependencyInfo->pBufferMemoryBarriers[u32BufferBarrierIndex].srcQueueFamilyIndex,
						vk_pDependencyInfo->pBufferMemoryBarriers[u32BufferBarrierIndex].dstQueueFamilyIndex,
						vk_pDependencyInfo->pBufferMemoryBarriers[u32BufferBarrierIndex].buffer,
						vk_pDependencyInfo->pBufferMemoryBarriers[u32BufferBarrierIndex].offset,
						vk_pDependencyInfo->pBufferMemoryBarriers[u32BufferBarrierIndex].size);
			} else
				barrierIterator->bufferBarriers.emplace_back(
						VK_STRUCTURE_TYPE_MEMORY_BARRIER,
						vk_pDependencyInfo->pBufferMemoryBarriers[u32BufferBarrierIndex].pNext,
						downgrade_access_flags_2(vk_pDependencyInfo->pBufferMemoryBarriers[u32BufferBarrierIndex].srcAccessMask),
						downgrade_access_flags_2(vk_pDependencyInfo->pBufferMemoryBarriers[u32BufferBarrierIndex].dstAccessMask),
						vk_pDependencyInfo->pBufferMemoryBarriers[u32BufferBarrierIndex].srcQueueFamilyIndex,
						vk_pDependencyInfo->pBufferMemoryBarriers[u32BufferBarrierIndex].dstQueueFamilyIndex,
						vk_pDependencyInfo->pBufferMemoryBarriers[u32BufferBarrierIndex].buffer,
						vk_pDependencyInfo->pBufferMemoryBarriers[u32BufferBarrierIndex].offset,
						vk_pDependencyInfo->pBufferMemoryBarriers[u32BufferBarrierIndex].size);
		}

		for (uint32_t u32ImageBarrierIndex = 0; u32ImageBarrierIndex < vk_pDependencyInfo->imageMemoryBarrierCount; u32ImageBarrierIndex++) {
			auto barrierIterator = std::find_if(barriers.begin(), barriers.end(), [&](const Barrier &rBarrier) {
						return rBarrier.vk_mSrcStages == vk_pDependencyInfo->pImageMemoryBarriers[u32ImageBarrierIndex].srcStageMask
								&& rBarrier.vk_mDstStages == vk_pDependencyInfo->pImageMemoryBarriers[u32ImageBarrierIndex].dstStageMask;
					});
			if (barrierIterator == barriers.end()) {
				barriers.emplace_back(
						vk_pDependencyInfo->pImageMemoryBarriers[u32ImageBarrierIndex].srcStageMask,
						vk_pDependencyInfo->pImageMemoryBarriers[u32ImageBarrierIndex].dstStageMask);
				barriers.back().imageBarriers.emplace_back(
						VK_STRUCTURE_TYPE_MEMORY_BARRIER,
						vk_pDependencyInfo->pImageMemoryBarriers[u32ImageBarrierIndex].pNext,
						downgrade_access_flags_2(vk_pDependencyInfo->pImageMemoryBarriers[u32ImageBarrierIndex].srcAccessMask),
						downgrade_access_flags_2(vk_pDependencyInfo->pImageMemoryBarriers[u32ImageBarrierIndex].dstAccessMask),
						vk_pDependencyInfo->pImageMemoryBarriers[u32ImageBarrierIndex].oldLayout,
						vk_pDependencyInfo->pImageMemoryBarriers[u32ImageBarrierIndex].newLayout,
						vk_pDependencyInfo->pImageMemoryBarriers[u32ImageBarrierIndex].srcQueueFamilyIndex,
						vk_pDependencyInfo->pImageMemoryBarriers[u32ImageBarrierIndex].dstQueueFamilyIndex,
						vk_pDependencyInfo->pImageMemoryBarriers[u32ImageBarrierIndex].image,
						vk_pDependencyInfo->pImageMemoryBarriers[u32ImageBarrierIndex].subresourceRange);
			} else
				barrierIterator->imageBarriers.emplace_back(
						VK_STRUCTURE_TYPE_MEMORY_BARRIER,
						vk_pDependencyInfo->pImageMemoryBarriers[u32ImageBarrierIndex].pNext,
						downgrade_access_flags_2(vk_pDependencyInfo->pImageMemoryBarriers[u32ImageBarrierIndex].srcAccessMask),
						downgrade_access_flags_2(vk_pDependencyInfo->pImageMemoryBarriers[u32ImageBarrierIndex].dstAccessMask),
						vk_pDependencyInfo->pImageMemoryBarriers[u32ImageBarrierIndex].oldLayout,
						vk_pDependencyInfo->pImageMemoryBarriers[u32ImageBarrierIndex].newLayout,
						vk_pDependencyInfo->pImageMemoryBarriers[u32ImageBarrierIndex].srcQueueFamilyIndex,
						vk_pDependencyInfo->pImageMemoryBarriers[u32ImageBarrierIndex].dstQueueFamilyIndex,
						vk_pDependencyInfo->pImageMemoryBarriers[u32ImageBarrierIndex].image,
						vk_pDependencyInfo->pImageMemoryBarriers[u32ImageBarrierIndex].subresourceRange);
		}

		for (const Barrier &rBarrier : barriers)
			vkCmdPipelineBarrier(
					vk_hCommandBuffer,
					rBarrier.vk_mSrcStages,
					rBarrier.vk_mDstStages,
					vk_pDependencyInfo->dependencyFlags,
					static_cast<uint32_t>(rBarrier.memoryBarriers.size()),
					rBarrier.memoryBarriers.data(),
					static_cast<uint32_t>(rBarrier.bufferBarriers.size()),
					rBarrier.bufferBarriers.data(),
					static_cast<uint32_t>(rBarrier.imageBarriers.size()),
					rBarrier.imageBarriers.data());
	}

	static void vk_cmd_write_timestamp_2(const VkCommandBuffer vk_hCommandBuffer, const VkPipelineStageFlags2 vk_mStage, const VkQueryPool vk_hQueryPool, const uint32_t u32Query) {
		if (std::popcount<VkPipelineStageFlags2>(vk_mStage) > 1)
			RE_ABORT("A combination of pipeline stages cannot be reduced to one for writing timestamps the old way");
		vkCmdWriteTimestamp(vk_hCommandBuffer, static_cast<VkPipelineStageFlagBits>(downgrade_pipeline_stage_flags_2(vk_mStage)), vk_hQueryPool, u32Query);
	}

	static VkResult vk_queue_submit_2(const VkQueue vk_hQueue, const uint32_t u32SubmitCount, const VkSubmitInfo2 *const vk_paSubmits, const VkFence vk_hFence) {
		size_t requiredMemoryBlockSize = sizeof(VkSubmitInfo) * u32SubmitCount;
		requiredMemoryBlockSize = next_multiple_inclusive<size_t>(requiredMemoryBlockSize, alignof(VkTimelineSemaphoreSubmitInfo));
		requiredMemoryBlockSize += sizeof(VkTimelineSemaphoreSubmitInfo) * u32SubmitCount;
		for (uint32_t u32SubmitInfoIndex = 0; u32SubmitInfoIndex < u32SubmitCount; u32SubmitInfoIndex++) {
			const uint32_t u32SharedSemaphoreInfoCount = vk_paSubmits[u32SubmitInfoIndex].waitSemaphoreInfoCount + vk_paSubmits[u32SubmitInfoIndex].signalSemaphoreInfoCount;
			requiredMemoryBlockSize = next_multiple_inclusive<size_t>(requiredMemoryBlockSize, alignof(VkSemaphore));
			requiredMemoryBlockSize += sizeof(VkSemaphore) * u32SharedSemaphoreInfoCount;
			requiredMemoryBlockSize = next_multiple_inclusive<size_t>(requiredMemoryBlockSize, alignof(uint64_t));
			requiredMemoryBlockSize += sizeof(uint64_t) * u32SharedSemaphoreInfoCount;
			requiredMemoryBlockSize = next_multiple_inclusive<size_t>(requiredMemoryBlockSize, alignof(VkPipelineStageFlags));
			requiredMemoryBlockSize += sizeof(VkPipelineStageFlags) * vk_paSubmits[u32SubmitInfoIndex].waitSemaphoreInfoCount;
			requiredMemoryBlockSize = next_multiple_inclusive<size_t>(requiredMemoryBlockSize, alignof(VkCommandBuffer));
			requiredMemoryBlockSize += sizeof(VkCommandBuffer) * vk_paSubmits[u32SubmitInfoIndex].commandBufferInfoCount;
		}
		UniqueVoidPointer memoryBlock(safe_malloc(requiredMemoryBlockSize));
		void *pMemoryBlockContent = memoryBlock.get();
		size_t remainingMemoryBlockSpace = requiredMemoryBlockSize;
		VkSubmitInfo *const vk_paNormalSubmits = static_cast<VkSubmitInfo*>(
				not_null(
					align_2(alignof(VkSubmitInfo), sizeof(VkSubmitInfo) * u32SubmitCount, pMemoryBlockContent, remainingMemoryBlockSpace)));
		VkTimelineSemaphoreSubmitInfo *const vk_paTimelineSemaphoreInfos = static_cast<VkTimelineSemaphoreSubmitInfo*>(
				not_null(
					align_2(alignof(VkTimelineSemaphoreSubmitInfo), sizeof(VkTimelineSemaphoreSubmitInfo) * u32SubmitCount, pMemoryBlockContent, remainingMemoryBlockSpace)));
		for (uint32_t u32SubmitInfoIndex = 0; u32SubmitInfoIndex < u32SubmitCount; u32SubmitInfoIndex++) {
			const uint32_t u32CommandBufferCount = vk_paSubmits[u32SubmitInfoIndex].commandBufferInfoCount,
				u32WaitSemaphoreCount = vk_paSubmits[u32SubmitInfoIndex].waitSemaphoreInfoCount,
				u32SignalSemaphoreCount = vk_paSubmits[u32SubmitInfoIndex].signalSemaphoreInfoCount;
			VkCommandBuffer *const vk_pahCommandBuffers = static_cast<VkCommandBuffer*>(
					not_null(
						align_2(alignof(VkCommandBuffer), sizeof(VkCommandBuffer) * u32CommandBufferCount, pMemoryBlockContent, remainingMemoryBlockSpace)));
			for (uint32_t u32CommandBufferIndex = 0; u32CommandBufferIndex < u32CommandBufferCount; u32CommandBufferIndex++)
				vk_pahCommandBuffers[u32CommandBufferIndex] = vk_paSubmits[u32SubmitInfoIndex].pCommandBufferInfos[u32CommandBufferIndex].commandBuffer;
			VkSemaphore *const vk_pahWaitSemaphores = static_cast<VkSemaphore*>(
					not_null(
						align_2(alignof(VkSemaphore), sizeof(VkSemaphore) * u32WaitSemaphoreCount, pMemoryBlockContent, remainingMemoryBlockSpace)));
			uint64_t *const pau64WaitValues = static_cast<uint64_t*>(
					not_null(
						align_2(alignof(uint64_t), sizeof(uint64_t) * u32WaitSemaphoreCount, pMemoryBlockContent, remainingMemoryBlockSpace)));
			VkPipelineStageFlags *const vk_pamPipelineStages = static_cast<VkPipelineStageFlags*>(
					not_null(
						align_2(alignof(VkPipelineStageFlags*), sizeof(VkPipelineStageFlags) * u32WaitSemaphoreCount, pMemoryBlockContent, remainingMemoryBlockSpace)));
			for (uint32_t u32WaitSemaphoreIndex = 0; u32WaitSemaphoreIndex < u32WaitSemaphoreCount; u32WaitSemaphoreIndex++) {
				vk_pahWaitSemaphores[u32WaitSemaphoreIndex] = vk_paSubmits[u32SubmitInfoIndex].pWaitSemaphoreInfos[u32WaitSemaphoreIndex].semaphore;
				pau64WaitValues[u32WaitSemaphoreIndex] = vk_paSubmits[u32SubmitInfoIndex].pWaitSemaphoreInfos[u32WaitSemaphoreIndex].value;
				vk_pamPipelineStages[u32WaitSemaphoreIndex] = downgrade_pipeline_stage_flags_2(vk_paSubmits[u32SubmitInfoIndex].pWaitSemaphoreInfos[u32WaitSemaphoreIndex].stageMask);
			}
			VkSemaphore *const vk_pahSignalSemaphores = static_cast<VkSemaphore*>(
					not_null(
						align_2(alignof(VkSemaphore), sizeof(VkSemaphore) * u32WaitSemaphoreCount, pMemoryBlockContent, remainingMemoryBlockSpace)));
			uint64_t *const pau64SignalValues = static_cast<uint64_t*>(
					not_null(
						align_2(alignof(uint64_t), sizeof(uint64_t) * u32WaitSemaphoreCount, pMemoryBlockContent, remainingMemoryBlockSpace)));
			for (uint32_t u32SignalSemaphoreIndex = 0; u32SignalSemaphoreIndex < u32SignalSemaphoreCount; u32SignalSemaphoreIndex++) {
				vk_pahSignalSemaphores[u32SignalSemaphoreIndex] = vk_paSubmits[u32SubmitInfoIndex].pSignalSemaphoreInfos[u32SignalSemaphoreIndex].semaphore;
				pau64SignalValues[u32SignalSemaphoreIndex] = vk_paSubmits[u32SubmitInfoIndex].pSignalSemaphoreInfos[u32SignalSemaphoreIndex].value;
			}
			vk_paTimelineSemaphoreInfos[u32SubmitInfoIndex].sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
			vk_paTimelineSemaphoreInfos[u32SubmitInfoIndex].pNext = vk_paSubmits[u32SubmitInfoIndex].pNext;
			vk_paTimelineSemaphoreInfos[u32SubmitInfoIndex].waitSemaphoreValueCount = u32WaitSemaphoreCount;
			vk_paTimelineSemaphoreInfos[u32SubmitInfoIndex].pWaitSemaphoreValues = pau64WaitValues;
			vk_paTimelineSemaphoreInfos[u32SubmitInfoIndex].signalSemaphoreValueCount = u32SignalSemaphoreCount;
			vk_paTimelineSemaphoreInfos[u32SubmitInfoIndex].pSignalSemaphoreValues = pau64SignalValues;
			vk_paNormalSubmits[u32SubmitInfoIndex].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			vk_paNormalSubmits[u32SubmitInfoIndex].pNext = &vk_paTimelineSemaphoreInfos[u32SubmitInfoIndex];
			vk_paNormalSubmits[u32SubmitInfoIndex].waitSemaphoreCount = u32WaitSemaphoreCount;
			vk_paNormalSubmits[u32SubmitInfoIndex].pWaitSemaphores = vk_pahWaitSemaphores;
			vk_paNormalSubmits[u32SubmitInfoIndex].pWaitDstStageMask = vk_pamPipelineStages;
			vk_paNormalSubmits[u32SubmitInfoIndex].commandBufferCount = u32CommandBufferCount;
			vk_paNormalSubmits[u32SubmitInfoIndex].pCommandBuffers = vk_pahCommandBuffers;
			vk_paNormalSubmits[u32SubmitInfoIndex].signalSemaphoreCount = u32SignalSemaphoreCount;
			vk_paNormalSubmits[u32SubmitInfoIndex].pSignalSemaphores = vk_pahSignalSemaphores;
		}
		return vkQueueSubmit(vk_hQueue, u32SubmitCount, vk_paNormalSubmits, vk_hFence);
	}

	bool load_synchronization_2_funcs() {
		if (are_vulkan_features_enabled<ENABLED_FEATURE_SYNCHRONIZATION_2_BIT>()) {
			vkCmdSetEvent2 = reinterpret_cast<PFN_vkCmdSetEvent2>(load_vulkan_func_with_device("vkCmdSetEvent2"));
			vkCmdResetEvent2 = reinterpret_cast<PFN_vkCmdResetEvent2>(load_vulkan_func_with_device("vkCmdResetEvent2"));
			vkCmdWaitEvents2 = reinterpret_cast<PFN_vkCmdWaitEvents2>(load_vulkan_func_with_device("vkCmdWaitEvents2"));
			vkCmdPipelineBarrier2 = reinterpret_cast<PFN_vkCmdPipelineBarrier2>(load_vulkan_func_with_device("vkCmdPipelineBarrier2"));
			vkCmdWriteTimestamp2 = reinterpret_cast<PFN_vkCmdWriteTimestamp2>(load_vulkan_func_with_device("vkCmdWriteTimestamp2"));
			vkQueueSubmit2 = reinterpret_cast<PFN_vkQueueSubmit2>(load_vulkan_func_with_device("vkQueueSubmit2"));
			if (vkCmdSetEvent2 
					&& vkCmdResetEvent2 
					&& vkCmdWaitEvents2 
					&& vkCmdPipelineBarrier2 
					&& vkCmdWriteTimestamp2 
					&& vkQueueSubmit2)
				return true;
			RE_ERROR("Failed to load native Vulkan functions related to the synchronization 2-feature");
		}
		vkCmdSetEvent2 = vk_cmd_set_event_2;
		vkCmdResetEvent2 = vk_cmd_reset_event_2;
		vkCmdWaitEvents2 = vk_cmd_wait_event_2;
		vkCmdPipelineBarrier2 = vk_cmd_pipeline_barrier_2;
		vkCmdWriteTimestamp2 = vk_cmd_write_timestamp_2;
		vkQueueSubmit2 = vk_queue_submit_2;
		return true;
	}

	void unload_synchronization_2_funcs() {
		vkCmdSetEvent2 = nullptr;
		vkCmdResetEvent2 = nullptr;
		vkCmdWaitEvents2 = nullptr;
		vkCmdPipelineBarrier2 = nullptr;
		vkCmdWriteTimestamp2 = nullptr;
		vkQueueSubmit2 = nullptr;
	}

}
