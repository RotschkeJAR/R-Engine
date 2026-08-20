#include "RE_Renderer_Images_Internal.hpp"
#include "RE_Main.hpp"
#include "RE_Window.hpp"

namespace RE {

	Vector2u renderImageSize;
	static float fGameScreenPercentage = 1.0f;

	bool create_renderer_images() {
		if (create_character_image()) {
			if (alloc_memory_for_renderer_images()) {
				if (create_character_image_view()) {
					return true;
				}
				free_memory_for_renderer_images();
			}
			destroy_character_image();
		}
		return false;
	}

	void destroy_renderer_images() {
		destroy_character_image_view();
		destroy_character_image();
		free_memory_for_renderer_images();
	}

	bool create_swapchain_related_images() {
		PRINT_DEBUG("Calculating size of renderable images");
		for (unsigned uDimensionIndex = 0; uDimensionIndex < renderImageSize.dimensions(); uDimensionIndex++)
			renderImageSize[uDimensionIndex] = std::clamp<uint32_t>(
					static_cast<uint32_t>(std::round(windowSize[uDimensionIndex] * (iScreenPercentage / static_cast<float>(SCREEN_PERCENTAGE_100)) * fGameScreenPercentage)),
					1U,
					windowSize[uDimensionIndex]);
		const bool bSinglesampledImageRequired = IS_SINGLESAMPLED_IMAGE_REQUIRED();
		if (create_depth_stencil_image()) {
			if (!bSinglesampledImageRequired)
				goto SKIP_SINGLESAMPLED_IMAGE_CREATION;
			if (create_singlesampled_image()) {
			SKIP_SINGLESAMPLED_IMAGE_CREATION:
				if (create_render_target_image()) {
					if (alloc_memory_for_swapchain_related_images()) {
						if (create_depth_stencil_image_views()) {
							if (!bSinglesampledImageRequired)
								goto SKIP_SINGLESAMPLED_IMAGE_VIEW_CREATION;
							if (create_singlesampled_image_views()) {
							SKIP_SINGLESAMPLED_IMAGE_VIEW_CREATION:
								if (create_render_target_image_views()) {
									return true;
								}
								destroy_singlesampled_image_views();
							}
							destroy_depth_stencil_image_views();
						}
						free_memory_for_swapchain_related_images();
					}
					destroy_render_target_image();
				}
				destroy_singlesampled_image();
			}
			destroy_depth_stencil_image();
		}
		return false;
	}

	void destroy_swapchain_related_images() {
		destroy_render_target_image_views();
		destroy_render_target_image();
		destroy_singlesampled_image_views();
		destroy_singlesampled_image();
		destroy_depth_stencil_image_views();
		destroy_depth_stencil_image();
		free_memory_for_swapchain_related_images();
	}

	void set_screen_percentage(float fNewPercentage) {
		if (fNewPercentage <= 0.0f) {
			RE_ERROR("Screen percentage cannot be zero or negative");
			return;
		}
		fNewPercentage = std::min<float>(fNewPercentage, 1.0f);
		if (fGameScreenPercentage != fNewPercentage) {
			fGameScreenPercentage = fNewPercentage;
			if (!bSwapchainDirty && bRunning) {
				PRINT_DEBUG("Applying new screen percentage settings");
				wait_for_rendering_finished();
				destroy_swapchain_related_images();
				create_swapchain_related_images();
			}
		}
	}

}
