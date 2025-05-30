#include "state.h"
#include <windows.h>

// State *init_state(HINSTANCE hInstance) {

State *init_state(HINSTANCE hInstance) { /// TODO

  State *app_state = calloc(1, sizeof(State));
  if (!app_state)
    return NULL;
  
  app_state->current_filter_type = NO_FILTER;
  app_state->hInstance = hInstance;

  return app_state; // change that
}

// --------------------------------------------------------------

/// f: ta funckja istnieje żeby było schludniej w mainie
void register_class(State *state, HINSTANCE hInstance,
                    const wchar_t *window_class_name) {

  static WNDCLASS window_class = {0};
  window_class.lpfnWndProc = WindowProcessMessage;
  window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
  window_class.hInstance = hInstance;
  window_class.lpszClassName = window_class_name;
  RegisterClassW(&window_class);
}

// --------------------------------------------------------------

// TODO: możliwe że to jest iekompletne. zobaczymy co z bufferem
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

void enable_export(State *app_state) {
  EnableMenuItem(app_state->UI_handles.hMenubar, (UINT_PTR)app_state->UI_handles.hExportMenu, MF_ENABLED);
  EnableMenuItem(app_state->UI_handles.hExportMenu, 21, MF_ENABLED); 
  EnableMenuItem(app_state->UI_handles.hExportMenu, 22, MF_ENABLED); 
  EnableMenuItem(app_state->UI_handles.hExportMenu, 23, MF_ENABLED); 
  EnableMenuItem(app_state->UI_handles.hExportMenu, 24, MF_ENABLED); 
  EnableMenuItem(app_state->UI_handles.hExportMenu, 25, MF_ENABLED); 
  EnableMenuItem(app_state->UI_handles.hMenubar, 4, MF_ENABLED); 
  EnableMenuItem(app_state->UI_handles.hMenubar, 5, MF_ENABLED); 

}
