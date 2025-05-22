#ifndef UTILS_H
#define UTILS_H

// #define STB_IMAGE_IMPLEMENTATION
// #define STB_IMAGE_WRITE_IMPLEMENTATION


#include <windows.h>
#include <commdlg.h>
#include <stdbool.h> //winkwink i had myown for a secnd itmight have broken sth
#include <stdint.h>
#include <string.h>

// #include "stb_image_write.h"
// #include "stb_image.h"

#include "filters.h"
#include "app_state.h"


void OpenFileDialog(HWND hwnd, AppState *app);
// void LoadImageFromFile(AppState *app, const wchar_t *filepath);
void blit_to_frame(Image *src, struct Frame *frame);
#endif
