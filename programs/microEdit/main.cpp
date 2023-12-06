#include <libm/syscallManager.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/rendering/Cols.h>
#include <libm/wmStuff/wmStuff.h>
#include <libm/cstr.h>
#include <libm/cstrTools.h>
#include <libm/gui/guiInstance.h>
#include <libm/gui/guiStuff/components/canvas/canvasComponent.h>
#include <libm/heap/heap.h>

#include "edit.hpp"
#include "input.hpp"
#include "syntax.hpp"
#include "render.hpp"

char* get_file_to_open(int argc, char** argv) {
    if (argc != 1) {
        return (char*) "bruh:programs/microEdit/assets/cheatsheet.txt";
    } else {
        return argv[0];
    }
}

const char* get_file_extension(const char* filename) {
	const char* chr_ptr = filename + StrIndexOf(filename, '.');
	if (chr_ptr == NULL) {
		return "";
	}
	return ++chr_ptr;
}

void KeyHit(void* _, GuiComponentStuff::KeyHitEventInfo info) {
    lastEventInfo = info;
}

int main(int argc, char** argv) {
    initWindowManagerStuff();

    Window* window = requestWindow();
    if (window == nullptr) {
        return -1;
    }


    _Free(window->Title);
    window->Title = StrCopy("MicroEdit");

    window->Dimensions.width = 80 * 8;
    window->Dimensions.height = 25 * 16;

    setWindow(window);

    GuiInstance* guiInstance = new GuiInstance(window);
    guiInstance->Init();
    guiInstance->screen->backgroundColor = 0xff000000;

    GuiComponentStuff::CanvasComponent* canvas = new GuiComponentStuff::CanvasComponent(0xff000000, GuiComponentStuff::ComponentSize(1., 1.), guiInstance->screen);
    guiInstance->screen->children->Add(canvas);
    canvas->KeyHitFunc = KeyHit;





    edit_state_t state = { 0 };

    state.window = window;
    state.guiInstance = guiInstance;
    state.canvas = canvas;

    const char* syx = StrCombineAndFree(StrCombine("bruh:programs/microEdit/assets/", get_file_extension(get_file_to_open(argc, argv))), ".syx");
	syntax = load_syntax(syx);
    _Free(syx);

    state.file_name = get_file_to_open(argc, argv);
    state.is_edited = false;
    state.is_in_insert_mode = true;

    fsReadFile(state.file_name, (void**) &state.input_buffer, &state.current_size);
    state.buffer_idx = state.current_size;

    for (int i = 0; i < state.current_size; i++) {
		state.char_cnt++;
		if (state.input_buffer[i] == '\n') {
			state.ln_cnt++;
			state.buffer_ln_idx++;
			state.char_cnt--;
		}
	}

    state.buffer_ln_idx = 0;
    state.buffer_idx = 0;

    while (!CheckForWindowClosed(window)) {
        guiInstance->Update();
        render_tui(&state);
        guiInstance->Render(false);

        if (listen_input(&state)) {
             break;        
        }

        // TODO:
        // Fix random crashes when resize
        // Fix sometimes not starting correctly
        // Improve Rendering using Advanced Text box
        // Improve Update Loop and reduce polling / not needed redraws
    }
    return 0;
}