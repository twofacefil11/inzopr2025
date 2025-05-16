#include "filters.h"


// Ta logika powinna być raczej w MW_COMMAND (jak będzie UI)
// lets call it debug for now.
void apply_filter(Image *image_data, Filter filter_type) {

  switch (filter_type) {

  case NO_FILTER: // do nothing of course
    break;

  case BW:
    apply_black_and_white(image_data); // TODO
    break;

  case NEGATIVE:
    apply_negative(image_data); // TODO
    break;

  case AMPLIFY:
    apply_amplify(image_data, 1.50); // TODO
    break;

  case BLUR:
    apply_blur(image_data, image_data->pixels);
    break;

  default:
    return;
  }

  return;
}




// coś jak będziemy debugować testować i logować to pozamieniać na result
// return type.
void apply_black_and_white(Image *image_data) {

  for (int y = 0; y < image_data->height; y++) {
    for (int x = 0; x < image_data->width; x++) {
      int i = (y * image_data->width + x) * 4; // forcujemy 4 channele

      // rozszczepiamy
      uint8_t r, g, b;
      r = image_data->pixels[i + 0];
      g = image_data->pixels[i + 1];
      b = image_data->pixels[i + 2];

      int bw_value = (r + g + b) / 3;

      image_data->pixels[i + 0] = bw_value;
      image_data->pixels[i + 1] = bw_value;
      image_data->pixels[i + 2] = bw_value;
      image_data->pixels[i + 3] = 255; // alpha i think

      // zapaku do komórki
      // image_data->pixels[i] = (a << 24) | (r << 16) | (g << 8) | b;
    }
  }
  // TODO
}

void apply_negative(Image *image_data) {

  for (int y = 0; y < image_data->height; y++) {
    for (int x = 0; x < image_data->width; x++) {
      int i = (y * image_data->width + x) * 4; // forcujemy 4 channele

      // rozszczepiamy
      uint8_t r, g, b;
      r = image_data->pixels[i + 0];
      g = image_data->pixels[i + 1];
      b = image_data->pixels[i + 2];

      image_data->pixels[i + 0] = 255 - r;
      image_data->pixels[i + 1] = 255 - g;
      image_data->pixels[i + 2] = 255 - b;
      image_data->pixels[i + 3] = 255; // alpha i think
    }
  }
}

void apply_blur(Image *image_data, unsigned char *ref_pixels) {
  // TODO:
  // blur powinien tez mieć zakres. gaussian chyba najłatwiejszy, itak nikogo
  // nie obhodzi
  // for (int y = 0; y < image_data->height; y++) {
  //   for (int x = 0; x < image_data->width; x++) {
  //     int i = (y * image_data->width + x) * 4;

  //     // matrix na obliczanie bluru typu gausian
  //     float kernel[3][3] = {{1 / 16.0f, 2 / 16.0f, 1 / 16.0f},
  //                           {2 / 16.0f, 4 / 16.0f, 2 / 16.0f},
  //                           {1 / 16.0f, 2 / 16.0f, 1 / 16.0f}};

  //     uint8_t r, g, b;
  //     for (int t = 0; t < 10; t++) {

  //       for (int ky = -1; ky <= 1; ++ky) {
  //         for (int kx = -1; kx <= 1; ++kx) {

  //           int sampleX = x + kx;
  //           int sampleY = y + ky;
  //           int sampleIndex = (sampleY * image_data->width + sampleX) * 4;

  //           float weight = kernel[ky + 1][kx + 1];
  //           r = ref_pixels[sampleIndex + 0] * weight;
  //           g = ref_pixels[sampleIndex + 1] * weight;
  //           b = ref_pixels[sampleIndex + 2] * weight;
  //         }
  //       }
  //     }

  //     // rozszczepiamy
  //     r = image_data->pixels[i + 0];
  //     g = image_data->pixels[i + 1];
  //     b = image_data->pixels[i + 2];

  //     // image_data->pixels[i + 0] = r * coef;
  //     // image_data->pixels[i + 1] = g * coef;
  //     // image_data->pixels[i + 2] = b * coef;
  //     // image_data->pixels[i + 3] = 255;// alpha i think
  //   }
  // }
}

void apply_amplify(Image *image_data, double coef) {

  for (int y = 0; y < image_data->height; y++) {
    for (int x = 0; x < image_data->width; x++) {
      int i = (y * image_data->width + x) * 4; // forcujemy 4 channele

      // rozszczepiamy
      uint8_t r, g, b;
      r = image_data->pixels[i + 0];
      g = image_data->pixels[i + 1];
      b = image_data->pixels[i + 2];

      image_data->pixels[i + 0] = r * coef;
      image_data->pixels[i + 1] = g * coef;
      image_data->pixels[i + 2] = b * coef;
      image_data->pixels[i + 3] = 255; // alpha i think
    }
  }

  // TODO
}
