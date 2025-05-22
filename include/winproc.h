#ifndef WINPROC_H
#define WINPROC_H

// #define STB_IMAGE_IMPLEMENTATION
// #define STB_IMAGE_WRITE_IMPLEMENTATION

#define ID_COMBOBOX 1001
#include <windows.h>
#include <commdlg.h> // for GetOpenFileName
#include <stdint.h>

#include "utils.h"
#include "app_state.h"

LRESULT CALLBACK WindowProcessMessage(HWND hwnd, UINT message, WPARAM wParam,
                                      LPARAM lParam);

#endif 
