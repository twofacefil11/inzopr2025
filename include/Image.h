#pragma once

#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>
#include <windows.h>

#include "stb_image.h"
#include "stb_image_write.h"

// -------------------------------------------------------------

typedef enum { PNG, JPG, BMP, TGA, HDR } ExportFormat;

/// Image reprezentuje załadowany w pamięci obraz i informacje o nim.
typedef struct {
  int width;
  int height;
  int channels;
  unsigned char *pixels;
} Image;

// -------------------------------------------------------------

int load_image(Image *image, char* path);
int load_image_from_current_path(char* current_image_path);
int free_image(Image *image);
int export_image(char* export_image_path, Image* image, ExportFormat FORMAT);
// -------------------------------------------------------------
#endif // IMAGE_H
