#pragma once

#ifndef UI_H
#define UI_H

#define UNICODE
#define _UNICODE

#include <stdint.h>
#include <windows.h>
#include <limits.h>
#include <commctrl.h>
#include <shellapi.h>


#include "Filters.h"
#include "Image.h"

#define FILTER_TYPES_COUNT 6

/// Foreward declaration or something, nie można includwać window_precudure bo
/// będzie include loop.
LRESULT CALLBACK WindowProcessMessage(HWND hwnd, UINT message, WPARAM wParam,
                                      LPARAM lParam);

LRESULT CALLBACK PanelProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                           UINT_PTR wp, DWORD_PTR lp);

//-------------------------------------------------------------------------------

typedef struct {
  int width;  // NOTE: a może by to tak dać do Dim stuct?
  int height; // NOTE: to też.

  BITMAPINFO frame_bitmap_info;
  HBITMAP frame_bitmap;
  HDC device_context;
} DisplayBuffer;

//-------------------------------------------------------------------------------

typedef struct {
  HWND hBlur;
  HWND hSharpen;
  HWND hSepia;
  HWND hAmplify;
  HWND hNegative;
  HWND hMonochrome;

} Filter_contorl_group_handles;

//-------------------------------------------------------------------------------

typedef struct {
  DisplayBuffer display_buffer;

  // tu jest tego za dużo,
  // ja nie wiem w sumei czy to będczie potrzebne
  HWND hwnd_main; // manual init.
  HWND hSidebar;
  HWND hComboBox;
  HMENU hMenubar;
  HMENU hExportMenu;
  HFONT hFont;
  HBRUSH hComboBgBrush;
  Filter_contorl_group_handles filter_controls;

} UI;

//-------------------------------------------------------------------------------

typedef struct {
  int system_width;
  int system_height;
  int width;
  int height;
  int x;
  int y;
} Window_initial_position;
/// TODO

//-------------------------------------------------------------------------------

DisplayBuffer make_display_buffer(HWND hwnd);
Window_initial_position getWindowInfo();
int choose_new_file_path(HWND hwnd, OPENFILENAME *ofn, char *filename);
int show_save_dialog(HWND hwnd, char *out_path);
int init_UI(HWND hwnd, UI *ui);
// void show_filter_controls(HWND hwnd, Filter_type filter); // nw
void enable_export(UI* ui);

void switch_controls(const Filter_contorl_group_handles *handles, const HWND *window_to_show);

//-------------------------------------------------------------------------------

#endif // UI_H
