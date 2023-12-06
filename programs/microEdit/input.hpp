#pragma once

#include "edit.hpp"

#include <libm/gui/guiStuff/generalStuff.h>

bool listen_input(edit_state_t* state);

extern GuiComponentStuff::KeyHitEventInfo lastEventInfo;