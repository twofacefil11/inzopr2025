// ---------------------------------------------------------------------

#include "Image.h"

// ---------------------------------------------------------------------

int free_image(Image *image) {

  if (!image->pixels)
    return -1; // already freed

  stbi_image_free(image->pixels);
  image->pixels = NULL;
  return 0;
}

// ---------------------------------------------------------------------

   

// ---------------------------------------------------------------------

int load_image(Image *image, char *path) {

  if (image->pixels)
    return 0; // not freed. free it first.
   
  image->pixels =
      stbi_load(path, &image->width, &image->height, &image->channels, 4);

  // zamiaa rgba na bgra czy jakoś tak
  for (int i = 0; i < image->width * image->height; ++i) {

    unsigned char *px = &image->pixels[i * 4];
    unsigned char tmp = px[0];

    px[0] = px[2];
    px[2] = tmp;

  }
  return 1;
}
// ---------------------------------------------------------------------

/// NOTE: jest szansa ze to za nas zrobi file picker lets see
int export_image(char *filename, Image *image, ExportFormat FORMAT) {
  int quality = 100;
  char full_path[512]; //nazwa + rozszeżenie

  size_t img_size = image->height * image->width * 4;
  unsigned char *copy = malloc(img_size);
  memcpy(copy, image->pixels, img_size);

  for (int i = 0; i < image->width * image->height; ++i) {
    unsigned char *px = &copy[i * 4];
    unsigned char tmp = px[0];
    px[0] = px[2];
    px[2] = tmp;
  }

  switch (FORMAT) {
    case PNG:
      snprintf(full_path, sizeof(full_path), "%s.png", filename);
      stbi_write_png(full_path, image->width, image->height, 4, copy,
                     image->width * 4);
      break;

    case JPG:
      snprintf(full_path, sizeof(full_path), "%s.jpg", filename);
      stbi_write_jpg(full_path, image->width, image->height, 4, copy, quality);
      break;

    case BMP:
      snprintf(full_path, sizeof(full_path), "%s.bmp", filename);
      stbi_write_bmp(full_path, image->width, image->height, 4, copy);
      break;

    case TGA:
      snprintf(full_path, sizeof(full_path), "%s.tga", filename);
      stbi_write_tga(full_path, image->width, image->height, 4, copy);
      break;

    case HDR:
      snprintf(full_path, sizeof(full_path), "%s.hdr", filename);
      stbi_write_hdr(full_path, image->width, image->height, 4, (float *)copy);
      break;
  }
  free(copy);
  return 1;
}
