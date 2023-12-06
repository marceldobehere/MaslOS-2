#include "input.hpp"

#include "render.hpp"

#include <libm/syscallManager.h>
#include <libm/memStuff.h>
#include <libm/heap/heap.h>

GuiComponentStuff::KeyHitEventInfo lastEventInfo = GuiComponentStuff::KeyHitEventInfo(0, 0);

void move_up(edit_state_t* state) {
	if (state->buffer_ln_idx <= 0 || state->buffer_idx <= 0) {
	} else {
		// move one line up
		int prev_buff = state->buffer_idx;

		for (int i = state->buffer_idx; i > 0; i--) {
			state->buffer_idx--;
			if (state->input_buffer[i - 1] == '\n' || state->buffer_idx < 0) {
				break;
			}
		}
		if (state->buffer_idx < 0) {
			state->buffer_idx = prev_buff;
		} else {
			state->buffer_ln_idx--;
		}
	}
}

void move_down(edit_state_t* state) {
	if (state->buffer_ln_idx >= state->ln_cnt - 1 || state->buffer_idx >= state->current_size) {
	} else {
		// move one line up
		int prev_buff = state->buffer_idx;

		for (int i = state->buffer_idx; i < state->current_size; i++) {
			state->buffer_idx++;
			if (state->input_buffer[i] == '\n' || state->buffer_idx > state->current_size) {
				break;
			}
		}
		if (state->buffer_idx > state->current_size) {
			state->buffer_idx = prev_buff;
		} else {
			state->buffer_ln_idx++;
		}
	}
}

void move_left(edit_state_t* state) {
	if (!(state->buffer_idx <= 0)) {
		if (state->input_buffer[state->buffer_idx - 1] == '\n') {
			state->buffer_ln_idx--;
		}
		state->buffer_idx -= 1;
	}
}

void move_right(edit_state_t* state) {
	if (state->buffer_idx < state->current_size) {
		if (state->input_buffer[state->buffer_idx] == '\n') {
			state->buffer_ln_idx++;
		}
		state->buffer_idx += 1;
	}
}

char async_getc(edit_state_t* state) {
    state->guiInstance->Update();

	char c = lastEventInfo.Chr;
	if (lastEventInfo.Chr != 0) {
		lastEventInfo = GuiComponentStuff::KeyHitEventInfo(0, 0);
	} else if (lastEventInfo.Scancode != 0) {
		switch (lastEventInfo.Scancode) {
			case 1:
				c = 27;
				break;
			case 14:
				c = '\b';
				break;
			case 77: // right
				move_right(state);
				c = -1;
				break;
			case 75: // left
				move_left(state);
				c = -1;
				break;
			case 72: // up
				move_up(state);
				c = -1;
				break;
			case 80: // down
				move_down(state);
				c = -1;
				break;
		}
		lastEventInfo = GuiComponentStuff::KeyHitEventInfo(0, 0);
	}
	return c;
}

bool listen_input(edit_state_t* state) {
	char input = 0;
	while ((input = async_getc(state)) == 0) {
	}

	if (input == -1) {
		return false;
	}

	if (!state->is_in_insert_mode) {
		switch (input) {
			case 'q':
				return true;
			case '\e':
				state->is_in_insert_mode = !state->is_in_insert_mode;
				break;

			case 'a':
				move_left(state);
				break;
			case 'd':
				move_right(state);
				break;
			case 'w':
				move_up(state);
				break;
			case 's':
				move_down(state);
				break;

			case '+': 
				fsWriteFileFromBuffer(state->file_name, state->input_buffer, state->current_size);
				state->is_edited = false;
				break;
		}
	} else {
		switch (input) {
			case '\b': {
				if (state->buffer_idx - 2 < 0 || state->current_size - 1 < 0) {
				} else {
					if (state->input_buffer[state->buffer_idx - 1] == '\n') {
						state->buffer_ln_idx--;
					}

					if (state->buffer_idx == state->current_size) {
					} else {
					    _memmove((void*) &state->input_buffer[state->buffer_idx], (void*) &state->input_buffer[state->buffer_idx - 1], (state->current_size - state->buffer_idx) * sizeof(char));
					}
					if (state->input_buffer[state->buffer_idx] == '\n') {
						state->ln_cnt--;
					} else {
						state->char_cnt--;
					}
					state->input_buffer = (char*) _Realloc((void*) state->input_buffer, --state->current_size);
					state->buffer_idx--;

					rerender_color(state);
				}
			}
			break;

			case '\e': {
				state->is_in_insert_mode = !state->is_in_insert_mode;
			}
			break;


			default: {
				if (input == '\n') {
					state->ln_cnt++;
					state->buffer_ln_idx++;
				} else {
					state->char_cnt++;
				}

				state->is_edited = true;
				state->input_buffer = (char*) _Realloc((void*) state->input_buffer, ++state->current_size); // WHY + 1 ???
				_memmove((void*) &state->input_buffer[state->buffer_idx], (void*) &state->input_buffer[state->buffer_idx+1], (state->current_size - state->buffer_idx) * sizeof(char));
				state->input_buffer[state->buffer_idx] = input;
				state->buffer_idx++;

				rerender_color(state);
			}
			break;
		}
	}

	return false;
}