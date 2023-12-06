#pragma once

#include "edit.hpp"
#include "syntax.hpp"

void render_tui(edit_state_t* state);

void rerender_color(edit_state_t* state);

extern syntax_header_t* syntax;