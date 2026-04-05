#ifndef __RE_GPU_SYNCHRONIZATION_2_H__
#define __RE_GPU_SYNCHRONIZATION_2_H__

#include "RE_GPU.hpp"

namespace RE {

	extern PFN_vkCmdSetEvent2 vkCmdSetEvent2;
	extern PFN_vkCmdResetEvent2 vkCmdResetEvent2;
	extern PFN_vkCmdWaitEvents2 vkCmdWaitEvents2;
	extern PFN_vkCmdPipelineBarrier2 vkCmdPipelineBarrier2;
	extern PFN_vkCmdWriteTimestamp2 vkCmdWriteTimestamp2;
	extern PFN_vkQueueSubmit2 vkQueueSubmit2;

}

#endif /* __RE_GPU_SYNCHRONIZATION_2_H__ */
