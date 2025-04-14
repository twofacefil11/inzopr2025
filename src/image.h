#ifndef IMAGE_H
#define IMAGE_H


// Image i metadane podane z stb_image.
//  NOTE: free the pixels arr za pomocą stbi_image_free(image_data.pixels);
typedef struct {
  int width;
  int channels; // Rgb, rgba etc.
  int height;
  unsigned char *pixels; // Pixele do manipulacji już skopiowane z pliku.
} Image;
#endif // !IMAGE_H
