#include "render.hpp"
#include "sprintf.hpp"

#include <libm/heap/heap.h>
#include <libm/memStuff.h>
#include <libm/cstrTools.h>

#define CHAR_SIZE 16
#define CHAR_WIDTH 8
#define GRAPHICS_RENDERING_MODE
#define CURSOR_COLOR 0xff33cccc
#define LINE_NUMBER_COLOR 0xffa9a9a9
#define SPACE_BETWEEN_LINE_NUMBER_TEXT 1 * CHAR_WIDTH

syntax_header_t* syntax = NULL;

uint32_t color_translation[] = {
	/*
	FOREGROUND_BLACK,
	FOREGROUND_RED,
	FOREGROUND_GREEN,
	FOREGROUND_YELLOW,
	FOREGROUND_BLUE,
	FOREGROUND_MAGENTA,
	FOREGROUND_CYAN,
	FOREGROUND_WHITE
	*/
	0xFF000000, // black
    0xFFAA0000, // red
    0xFF00AA00, // green
    0xFFFFFF00, // yellow
    0xFF0000AA, // blue
    0xFFAA00AA, // magenta
    0xFF00AAAA, // cyan
    0xFFFFFFFF, // white
};

uint8_t* color = NULL;

void rerender_color(edit_state_t* state) {
	if (syntax) {	
		if (color) {
			_Free(color);
		}
		color = highlight(state->input_buffer, state->current_size, syntax);
	}
}


void render_tui(edit_state_t* state) {
	if (!color) {
		rerender_color(state);
	}

    state->canvas->Clear();

    int max_length_before_line_wrap = state->window->Dimensions.width / CHAR_WIDTH;
	char buff[128] = { 0 };
	sprintf(buff, "File: %s [%c] Mode: --%s-- Current Line: %d Line: %d", state->file_name, state->is_edited ? '*' : '-',  state->is_in_insert_mode ? "INSERT" : "EDIT", state->buffer_ln_idx, state->ln_cnt);

	state->canvas->DrawText(0, state->window->Dimensions.height - CHAR_SIZE, 0xffffffff, 1, buff);

	int j = 0;
	int cur_x;
	int cur_y = 0;
	int already_drawn  = 0;
	bool initial_line_drawn = false;
	int current_line = 1;
	bool cursor_drawn = false;

	sprintf(buff, "%d .", state->ln_cnt);
	int space_to_draw = SPACE_BETWEEN_LINE_NUMBER_TEXT + (StrLen(buff) * CHAR_WIDTH);
	cur_x = space_to_draw;

	int possible_lines_to_draw = state->window->Dimensions.height / CHAR_SIZE - 4;

	state->canvas->DrawLine(space_to_draw - (1 * CHAR_WIDTH), 0, space_to_draw - (1 * CHAR_WIDTH), state->window->Dimensions.height - (2 * CHAR_WIDTH), LINE_NUMBER_COLOR, 1);

	for (int i = 0; i < state->current_size; i++) {
		if ((state->ln_cnt - 1 < possible_lines_to_draw || j >= state->buffer_ln_idx) && already_drawn <= possible_lines_to_draw) {
			if (!initial_line_drawn) {
				initial_line_drawn = true;
				sprintf(buff, "%d.", current_line);
				state->canvas->DrawText(0, cur_y, LINE_NUMBER_COLOR, 1, buff);
			}

			if (i == state->buffer_idx) {
				state->canvas->DrawChar(cur_x, cur_y, CURSOR_COLOR, 1, '|');
				cur_x += CHAR_WIDTH;
				cursor_drawn = true;
			}

			if (state->input_buffer[i] >= 0x20 && state->input_buffer[i] <= 0x7E) {
				state->canvas->DrawChar(cur_x, cur_y, color ? color_translation[color[i]] : 0xffffffff, 1, state->input_buffer[i]);
			}

			cur_x += CHAR_WIDTH;

			if (state->input_buffer[i] == '\n') {
				already_drawn++;
				current_line++;
				cur_x = space_to_draw;
				cur_y += CHAR_SIZE;
				sprintf(buff, "%d.", current_line);
				state->canvas->DrawText(0, cur_y, LINE_NUMBER_COLOR, 1, buff);
			} else if ((cur_x / CHAR_WIDTH) % max_length_before_line_wrap == 0) {
                cur_y += CHAR_SIZE;
				cur_x = space_to_draw;
				already_drawn++;
            }
		} else {
			if (state->input_buffer[i] == '\n') {
				current_line++;
				j++;
			}
		}
	}

	if (!cursor_drawn) {
		if (!initial_line_drawn) {
			sprintf(buff, "%d.", current_line);
			state->canvas->DrawText(0, cur_y, LINE_NUMBER_COLOR, 1, buff);
		}
		state->canvas->DrawChar(cur_x, cur_y, CURSOR_COLOR, 1, '|');
	}
}