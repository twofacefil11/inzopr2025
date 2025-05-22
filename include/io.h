#pragma once
#ifndef IO_H
#define IO_H

#include "image.h"
// jest szansa, że edytor krzyczy na liczbe argumentów w bmp, tga i hdr.
// compiles just fine so leave it or test it whatever.
void export_image
(char *filename, int width, int height, unsigned char *pixels,
                  ExportFormat FORMAT) {

  int quality =
      100; // jpg ma wybór quality od 0 do 100. puki co hardcoduje bo nie wiem
           // jak to info będzie podawane. stbi_write_jpg(filename, width,
           // height, 4, pixels, width * 4, quality);
  char *extention;

  switch (FORMAT) {
  case PNG:
    extention = ".png";
    stbi_write_png(strcat((char *)filename, extention), width, height, 4,
                   pixels, width * 4);
    break;
  case JPG:
    extention = ".jpg";
    stbi_write_jpg(strcat((char *)filename, extention), width, height, 4,
                   pixels, quality);
    break;
  case BMP:
    extention = ".bmp";
    stbi_write_bmp(strcat((char *)filename, extention), width, height, 4,
                   pixels);
    break;
  case TGA:
    extention = ".tga";
    stbi_write_tga(strcat((char *)filename, extention), width, height, 4,
                   pixels);
    break;
  case HDR:
    extention = ".hdr";
    stbi_write_hdr(strcat((char *)filename, extention), width, height, 4,
                   (float *)pixels);
    break;
  }
}
#endif
