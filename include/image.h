#ifndef IMAGE_H
#define IMAGE_H
// #define STB_IMAGE_IMPLEMENTATION
// #define STB_IMAGE_WRITE_IMPLEMENTATION

#include <string.h>
#include <windows.h>

#include "stb_image.h"
#include "stb_image_write.h"

// Image i metadane podane z stb_image.
//  NOTE: free the pixels arr za pomocą stbi_image_free(image_data.pixels);

typedef struct {
  int width;
  int channels; // Rgb, rgba etc.
  int height;
  unsigned char *pixels; // Pixele do manipulacji już skopiowane z pliku.
} Image;

typedef enum { PNG, JPG, BMP, TGA, HDR } ExportFormat;


Image *LoadImageFromFile(const wchar_t *filepath);
#endif // !IMAGE_H
