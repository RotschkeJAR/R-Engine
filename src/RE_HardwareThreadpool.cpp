#include "RE_Internal.hpp"

namespace RE {

	const size_t HardwareThreadpool::sAmountOfThreads = std::max(std::jthread::hardware_concurrency() - 3U, 2U);
	
	size_t HardwareThreadpool::find_next_occupation() {
		std::optional<size_t> std_occupyableThread;
		for (size_t i = 0; i < sAmountOfThreads; i++)
			if (!std_threads[i].joinable()) {
				std_occupyableThread = i;
				break;
			}
		if (!std_occupyableThread.has_value())
			std_occupyableThread = *std::min_element(std_agePerThread.get(), std_agePerThread.get() + sAmountOfThreads);
		std_agePerThread[*std_occupyableThread] = (*std::max_element(std_agePerThread.get(), std_agePerThread.get() + sAmountOfThreads)) + 1;
		return *std_occupyableThread;
	}

	HardwareThreadpool::HardwareThreadpool() {
		std_threads = std::make_unique<std::jthread[]>(sAmountOfThreads);
		std_agePerThread = std::make_unique<uint64_t[]>(sAmountOfThreads);
	}
	
	HardwareThreadpool::HardwareThreadpool(HardwareThreadpool &&rrCopy) : std_threads(std::move(std_threads)), std_agePerThread(std::move(std_agePerThread)) {}
	
	HardwareThreadpool::~HardwareThreadpool() {
		join();
	}

	void HardwareThreadpool::move_thread(std::jthread &&rrThread) {
		std::lock_guard<std::mutex> std_lockGuardMute(std_mutex);
		const size_t sOldThreadIndex = find_next_occupation();
		std_threads[sOldThreadIndex] = std::move(rrThread);
	}
	
	bool HardwareThreadpool::joinable() {
		std::lock_guard<std::mutex> std_lockGuardMute(std_mutex);
		for (size_t i = 0; i < sAmountOfThreads; i++)
			if (std_threads[i].joinable())
				return true;
		return false;
	}
	
	void HardwareThreadpool::join() {
		std::lock_guard<std::mutex> std_lockGuardMute(std_mutex);
		for (size_t i = 0; i < sAmountOfThreads; i++)
			if (std_threads[i].joinable())
				std_threads[i].join();
		std::fill(std_agePerThread.get(), std_agePerThread.get() + sAmountOfThreads, 0);
	}

	size_t HardwareThreadpool::free_slots() {
		std::lock_guard<std::mutex> std_lockGuardMute(std_mutex);
		size_t sFreeSlots = 0;
		for (size_t i = 0; i < sAmountOfThreads; i++)
			if (!std_threads[i].joinable())
				sFreeSlots++;
		return sFreeSlots;
	}
	
	size_t HardwareThreadpool::occupied_slots() {
		return sAmountOfThreads - free_slots();
	}
	
	size_t HardwareThreadpool::amount_of_threads() {
		return sAmountOfThreads;
	}

}
