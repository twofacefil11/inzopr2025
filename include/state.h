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

typedef struct {
  bool IMAGE_LOADED;
  bool PREVIEW_ORIGINAL;
} Flags;

//-------------------------------------------------------------------------------


typedef struct {

  HINSTANCE hInstance;
  UI UI_handles;

  Flags flags;
  
  
  Image original_image;
  Image current_image;

  char current_image_path[MAX_PATH];
  char export_file_path[MAX_PATH];

  // to zostanie zamienione na pointer do koleiki
  Filter_type current_filter_type;
  Filter_params filter_params;
  HICON hic;
  float zoom;

} State;

//-------------------------------------------------------------------------------

State *init_state(HINSTANCE hInstance);

void set_current_image_path(State *app_state, char *image_path);
void register_class(State *state, HINSTANCE hInstance,
                    const unsigned short *window_class_name);

int free_state(State *app_state); // return an error lol

//-------------------------------------------------------------------------------

#endif // STATE_H
