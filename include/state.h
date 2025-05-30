// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
#ifndef STATE_H
#define STATE_H

#define UNICODE
#define _UNICODE

#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

#include "Filters.h"
#include "Image.h"
#include "UI.h"

//-------------------------------------------------------------------------------
/// THIS HAS TO be done initiating before call for create window.
/// bo create window podaje state do winproc
//-------------------------------------------------------------------------------

typedef struct {
  bool IMAGE_LOADED;
  bool PREVIEW_ORIGINAL;
} Flags;

//-------------------------------------------------------------------------------

typedef struct {
  Image original_image;
  Image current_image;
  char current_image_path[MAX_PATH];
  char export_file_path[MAX_PATH];
  HINSTANCE hInstance;
  UI UI_handles;

  // to zostanie zamienione na pointer do koleiki
  Filter_type current_filter_type;
  Flags flags;

} State;

//-------------------------------------------------------------------------------

State *init_state(HINSTANCE hInstance);
void set_current_image_path(State *app_state, char *image_path);
int free_state(State *app_state); // return an error lol
void enable_export(State *app_state);
void register_class(State *state, HINSTANCE hInstance,
                    const wchar_t *window_class_name);

//-------------------------------------------------------------------------------

#endif // STATE_H
