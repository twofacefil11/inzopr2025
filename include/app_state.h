#pragma once

#ifndef APP_STATE_H
#define APP_STATE_H

#include "filters.h"
#include <stdbool.h>
#include <stdint.h>
#include <windef.h>

typedef enum { PNG, JPG, BMP, TGA, HDR } ExportFormat;

// frame prosto dla windowsa
struct Frame {
  int width;
  int height;
  uint32_t *pixels; // uwaga uwaga, od windowsa 10, windows alokuje miejsce
                    // samodzielnie na tą array.
};

// STATE OF THE APP. everything goes in here. it might not be global eventually
typedef struct {
  Image *image_data;
  struct Frame *frame;

  HWND hwnd_main;
  HWND hwnd_combobox;
  HWND hwnd_sidebar;

  Filter current_filter;

  bool shouldQuit;

} AppState;

#endif // APP_STATE_H
