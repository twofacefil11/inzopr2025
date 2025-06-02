#include "Filters.h"

// coś jak będziemy debugować testować i logować to pozamieniać na result
// return type.
void apply_monochrome(Image *original_image, Image *current_image,
                      Filter_params *filter_params) {

  for (int y = 0; y < original_image->height; y++) {
    for (int x = 0; x < original_image->width; x++) {
      int i = (y * original_image->width + x) * 4; // forcujemy 4 channele

      // rozszczepiamy
      uint8_t r, g, b;
      r = original_image->pixels[i + 0];
      g = original_image->pixels[i + 1];
      b = original_image->pixels[i + 2];

      int ir = r + filter_params->mono_r;
      int ig = g + filter_params->mono_g;
      int ib = b + filter_params->mono_b;

      int bw_value = (ir + ig + ib) / 3;

      // image_data->pixels[i + 0] = (uint8_t)(ib < 0 ? 0: ib > 255 ? 255 : ib);
      // image_data->pixels[i + 1] = (uint8_t)(ig < 0 ? 0: ig > 255 ? 255 : ig);
      // image_data->pixels[i + 2] =(uint8_t)(ir < 0 ? 0: ir > 255 ? 255 : ir);

      current_image->pixels[i + 0] = (uint8_t)bw_value;
      current_image->pixels[i + 1] = (uint8_t)bw_value;
      current_image->pixels[i + 2] = (uint8_t)bw_value;
      current_image->pixels[i + 3] = 255; // alpha i think
    }
  }
  // TODO
}

void apply_negative(Image *original_image, Image *current_image) {

  for (int y = 0; y < original_image->height; y++) {
    for (int x = 0; x < original_image->width; x++) {
      int i = (y * original_image->width + x) * 4; // forcujemy 4 channele

      // rozszczepiamy
      uint8_t r, g, b;
      r = original_image->pixels[i + 0];
      g = original_image->pixels[i + 1];
      b = original_image->pixels[i + 2];

      current_image->pixels[i + 0] = 255 - r;
      current_image->pixels[i + 1] = 255 - g;
      current_image->pixels[i + 2] = 255 - b;
      current_image->pixels[i + 3] = 255; // alpha i think
    }
  }
}

void apply_blur(Image *original_image, Image *current_image,
                Filter_params *filter_params) {

  // matrix wag do bluru typu gausian
  float kernel_weights[9] = {1 / 16.0f, 2 / 16.0f, 1 / 16.0f,
                             2 / 16.0f, 4 / 16.0f, 2 / 16.0f,
                             1 / 16.0f, 2 / 16.0f, 1 / 16.0f};
  // matrix indexów w kernelu 3x3
  int kernel_indexes[9] = {-4 - (4 * original_image->width),
                           0 - (4 * original_image->width),
                           4 - (4 * original_image->width),
                           -4,
                           0,
                           4,
                           -4 - (4 * original_image->width),
                           0 - (4 * original_image->width),
                           4 - (4 * original_image->width)};
  // NOTE: jeżeli chcemy zwiększać moc rozmycia, trzeba zaimplementować dynamiczny kernel size i obliczanie indexów i wag
  // NIEPARZYSTY!! rozmiar kernela podawać w filter_params jakby co, dodam do stae, nie wiem czy użyjemy

  for (int y = 0; y < original_image->height; y++) {
    for (int x = 0; x < original_image->width; x++) {
      int i = (y * original_image->width + x) * 4;

      // member to clamp it later
      //dla pixela na który patrzymy zliczamy średnią z ważonych przez kernel pixeli wokoło i ustawiamy to co wyjdzie dla r g i b;
      if (y == 0 || y == original_image->height - 1 || x == 0 || x == original_image->width - 1) continue;

      uint8_t r = 0, g = 0, b = 0;
      
      for (int k = 0; k < 10; k++) {
        b += original_image->pixels[i + kernel_indexes[k]] * kernel_weights[k];
        g += original_image->pixels[i + 1 + kernel_indexes[k]] *
             kernel_weights[k];
        r += original_image->pixels[i + 2 + kernel_indexes[k]] *
             kernel_weights[k];
      }

      current_image->pixels[i + 0] = b;
      current_image->pixels[i + 1] = g;
      current_image->pixels[i + 2] = r;

    }
  }
}
void apply_amplify(Image *original_image, Image *current_image,
                   Filter_params *filter_params, bool CLAMP_AMPLIFY) {

  for (int y = 0; y < original_image->height; y++) {
    for (int x = 0; x < original_image->width; x++) {
      int i = (y * original_image->width + x) * 4; // forcujemy 4 channele

      // rozszczepiamy
      uint8_t r, g, b;
      b = original_image->pixels[i + 0];
      g = original_image->pixels[i + 1];
      r = original_image->pixels[i + 2];

      if (CLAMP_AMPLIFY) {
        int ib = b * filter_params->amplify_b;
        int ig = g * filter_params->amplify_g;
        int ir = r * filter_params->amplify_r;

        current_image->pixels[i + 0] = (uint8_t)(ib < 0     ? 0
                                                 : ib > 255 ? 255
                                                            : ib);
        current_image->pixels[i + 1] = (uint8_t)(ig < 0     ? 0
                                                 : ig > 255 ? 255
                                                            : ig);
        current_image->pixels[i + 2] = (uint8_t)(ir < 0     ? 0
                                                 : ir > 255 ? 255
                                                            : ir);
        continue;
      }

      current_image->pixels[i + 0] = (b * filter_params->amplify_b);
      current_image->pixels[i + 1] = (g * filter_params->amplify_g);
      current_image->pixels[i + 2] = (r * filter_params->amplify_r);
      current_image->pixels[i + 3] = 255;
    }
  }
}

void apply_sepia(Image *original_image, Image *current_image) {
  double weight_red, weight_green, weight_blue;

  for (int x = 0; x < original_image->height * original_image->width; x++) {
    int i = x * 4;

    uint8_t r, g, b;
    r = original_image->pixels[i + 2];
    g = original_image->pixels[i + 1];
    b = original_image->pixels[i + 0];

    weight_red = (r * 0.393) + (g * 0.769) + (b * 0.189);
    weight_green = (r * 0.349) + (g * 0.686) + (b * 0.168);
    weight_blue = (r * 0.272) + (g * 0.534) + (b * 0.131);

    weight_red = (weight_red < 0) ? 0 : weight_red;
    weight_green = (weight_green < 0) ? 0 : weight_green;
    weight_blue = (weight_blue < 0) ? 0 : weight_blue;

    weight_red = (weight_red > 255) ? 255 : weight_red;
    weight_green = (weight_green > 255) ? 255 : weight_green;
    weight_blue = (weight_blue > 255) ? 255 : weight_blue;

    current_image->pixels[i + 2] = (uint8_t)weight_red;
    current_image->pixels[i + 1] = (uint8_t)weight_green;
    current_image->pixels[i + 0] = (uint8_t)weight_blue;
    current_image->pixels[i + 3] = 255; // alpha i think
  }
}

// ------------------------------------------------------------------------

void reapply_effects(Image *original_image, Image *current_image,
                     Filter_type type) {}
