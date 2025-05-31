#include "state.h"
#include <windows.h>

// --------------------------------------------------------------

State *init_state(HINSTANCE hInstance) { /// TODO

  State *app_state = calloc(1, sizeof(State));
  if (!app_state)
    return NULL;
  
  app_state->current_filter_type = NO_FILTER;
  app_state->hInstance = hInstance;

  // musi być 1 bo jest mnożone w filtrze
  app_state->filter_params->amplify_r = 1;
  app_state->filter_params->amplify_g = 1;
  app_state->filter_params->amplify_b = 1;

  return app_state; // change that
}

// --------------------------------------------------------------

/// f: ta funckja istnieje żeby było schludniej w mainie
void register_class(State *state, HINSTANCE hInstance,
                  const unsigned short *window_class_name) {

  static WNDCLASSW window_class = {0};
  window_class.lpfnWndProc = WindowProcessMessage;
  window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
  window_class.hInstance = hInstance;
  window_class.lpszClassName = window_class_name;
  RegisterClassW(&window_class);
}

// --------------------------------------------------------------

// TODO: możliwe że to jest niekompletne. zobaczymy co z bufferem
int free_state(State *state) {
  free_image(&state->current_image);
  free_image(&state->original_image);
  free(state);

  return 0;
}

// ------------------------------------------------------------------

void set_current_image_path(State *app_state, char* image_path) {
    if (!image_path) return; // nima
    strncpy(app_state->current_image_path, image_path, MAX_PATH - 1);
    app_state->current_image_path[MAX_PATH - 1] = '\0'; // prawie pewien
}

// ------------------------------------------------------------------
