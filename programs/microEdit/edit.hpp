#pragma once

#include <stdint.h>

#include <libm/window/window.h>
#include <libm/gui/guiInstance.h>
#include <libm/gui/guiStuff/components/canvas/canvasComponent.h>

typedef struct edit_state {
	char* file_name;
	char* input_buffer;
	bool is_edited;
	bool is_in_insert_mode;

	uint64_t ln_cnt;
	uint64_t char_cnt;

	uint64_t buffer_ln_idx;
	uint64_t buffer_idx;

	uint64_t current_size;

    Window* window;
    GuiInstance* guiInstance;
    GuiComponentStuff::CanvasComponent* canvas;
} edit_state_t;