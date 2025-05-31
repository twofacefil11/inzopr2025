#include "Filters.h"

// coś jak będziemy debugować testować i logować to pozamieniać na result
// return type.
void apply_monochrome(Image *image_data) {

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

void apply_amplify(Image *image_data) {
  double coef = 0.2;
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

void apply_sepia(Image *image_data) {
  double weight_red, weight_green, weight_blue;

  for (int y = 0; y < image_data->height; y++) {
    for (int x = 0; x < image_data->width; x++) {
      int i = (y * image_data->width + x) * 4;
      // tr = 0.393R + 0.769G + 0.189B
      // tg = 0.349R + 0.686G + 0.168B
      // tb = 0.272R + 0.534G + 0.131B

      uint8_t r, g, b;
      r = image_data->pixels[i + 0];
      g = image_data->pixels[i + 1];
      b = image_data->pixels[i + 2];

      weight_red = (r * 0.393) * (g * 0.769) * (b * 0.189);
      weight_green = (r * 0.349) * (g * 0.686) * (b * 0.168);
      weight_blue = (r * 0.272) * (g * 0.534) * (b * 0.131);
      
      weight_red = (weight_red < 0) ? 0 : weight_red;
      weight_green= (weight_green < 0) ? 0 : weight_green;
      weight_blue= (weight_blue < 0) ? 0 : weight_blue;

      weight_red = (weight_red > 255) ? 255 : weight_red;
      weight_green= (weight_green > 255) ? 255 : weight_green;
      weight_blue= (weight_blue > 255) ? 255 : weight_blue;


      image_data->pixels[i + 0] =
      image_data->pixels[i + 1] = weight_green;
      image_data->pixels[i + 2] = weight_blue;
      image_data->pixels[i + 3] = 255; // alpha i think
    }
  }
}
