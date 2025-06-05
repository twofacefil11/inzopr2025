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
      bw_value = bw_value < 0 ? 0 : bw_value > 255 ? 255 : bw_value;
      current_image->pixels[i + 0] = bw_value;
      current_image->pixels[i + 1] = bw_value;
      current_image->pixels[i + 2] = bw_value;
      current_image->pixels[i + 2] = (uint8_t)bw_value;
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
// mark

void apply_blur(Image *original_image, Image *current_image,
                Filter_params *filter_params) {

  int width = original_image->width;
  int height = original_image->height;
  int n = width * height;
  uint8_t *mid_image = malloc((size_t)n * 4);

  // Manual copy from original_image->pixels to mid_image
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int i = (y * width + x) * 4;
      mid_image[i + 0] = original_image->pixels[i + 0];
      mid_image[i + 1] = original_image->pixels[i + 1];
      mid_image[i + 2] = original_image->pixels[i + 2];
      mid_image[i + 3] = original_image->pixels[i + 3]; // alpha if present
    }
  }

  float kernel_weights[9] = {1 / 16.0f, 2 / 16.0f, 1 / 16.0f,
                             2 / 16.0f, 4 / 16.0f, 2 / 16.0f,
                             1 / 16.0f, 2 / 16.0f, 1 / 16.0f};

  int kernel_indexes[9] = {
      -4 - (4 * width), 0 - (4 * width), 4 - (4 * width), -4, 0, 4,
      -4 + (4 * width), 0 + (4 * width), 4 + (4 * width)};

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int i = (y * width + x) * 4;
      for (int t = 0; t < filter_params->blur_times; t++) {

        if (y == 0 || y == height - 1 || x == 0 || x == width - 1)
          continue;

        uint8_t r = 0, g = 0, b = 0;

        for (int k = 0; k < 9; k++) {
          b += mid_image[i + kernel_indexes[k]] * kernel_weights[k];
          g += mid_image[i + 1 + kernel_indexes[k]] * kernel_weights[k];
          r += mid_image[i + 2 + kernel_indexes[k]] * kernel_weights[k];
        }

        mid_image[i + 0] = b;
        mid_image[i + 1] = g;
        mid_image[i + 2] = r;
      }
    }
  }

  // Manual copy from mid_image back to current_image->pixels
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int i = (y * width + x) * 4;
      current_image->pixels[i + 0] = mid_image[i + 0];
      current_image->pixels[i + 1] = mid_image[i + 1];
      current_image->pixels[i + 2] = mid_image[i + 2];
      current_image->pixels[i + 3] = mid_image[i + 3];
    }
  }

  free(mid_image);
}

void apply_amplify(Image *original_image, Image *current_image,
                   Filter_params *filter_params) {

  for (int y = 0; y < original_image->height; y++) {
    for (int x = 0; x < original_image->width; x++) {
      int i = (y * original_image->width + x) * 4;

      uint8_t r, g, b;
      b = original_image->pixels[i + 0];
      g = original_image->pixels[i + 1];
      r = original_image->pixels[i + 2];

      if (filter_params->clamp_amplify) {
      float original_lum = 0.2126f * r + 0.7152f * g + 0.0722f * b;

      float new_r = r * filter_params->amplify_r * 0.2f;
      float new_g = g * filter_params->amplify_g * 0.2f;
      float new_b = b * filter_params->amplify_b * 0.2f;

      float new_lum = 0.2126f * new_r + 0.7152f * new_g + 0.0722f * new_b;
      float lum_ratio = (new_lum > 0.0f) ? (original_lum / new_lum) : 1.0f;

      new_r *= lum_ratio;
      new_g *= lum_ratio;
      new_b *= lum_ratio;

      current_image->pixels[i + 0] = clamp_int((int)new_b);
      current_image->pixels[i + 1] = clamp_int((int)new_g);
      current_image->pixels[i + 2] = clamp_int((int)new_r);
      current_image->pixels[i + 3] = 255;
        continue;
      }

        int ib = b * filter_params->amplify_b;
        int ig = g * filter_params->amplify_g;
        int ir = r * filter_params->amplify_r;

        current_image->pixels[i + 0] = (uint8_t)ib;
        current_image->pixels[i + 1] = (uint8_t)ig;
        current_image->pixels[i + 2] = (uint8_t)ir;
        current_image->pixels[i + 3] = 255;
    }
  }
}
// ------------------------------------------------------------------------

static inline uint8_t clamp_int(int value) {
  return value < 0 ? 0 : value > 255 ? 255 : value;
}

void apply_sharpen(Image *original_image, Image *current_image,
                   Filter_params *filter_params) {
  int *weights_x;
  int *weights_y;

  // różne weights dla dwuch opcji
  int sobel_x[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
  int sobel_y[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};

  int prewitt_x[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
  int prewitt_y[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};

  if (filter_params->sharpen_sobel) {
    weights_x = sobel_x;
    weights_y = sobel_y;
  } else if (filter_params->sharpen_prewitt) {
    weights_x = prewitt_x;
    weights_y = prewitt_y;
  }

  int kernel_indexes[9] = {-4 - (4 * original_image->width),
                           0 - (4 * original_image->width),
                           4 - (4 * original_image->width),
                           -4,
                           0,
                           4,
                           -4 + (4 * original_image->width),
                           0 + (4 * original_image->width),
                           4 + (4 * original_image->width)};

  float alpha = filter_params->sharpen_mix_const * 0.004f;

  for (int y = 1; y < original_image->height - 1; y++) {
    for (int x = 1; x < original_image->width - 1; x++) {
      int i = (y * original_image->width + x) * 4;

      int hb = 0, hg = 0, hr = 0;
      int vb = 0, vg = 0, vr = 0;

      for (int f = 0; f < 9; f++) {
        int offset = i + kernel_indexes[f];
        hb += original_image->pixels[offset + 0] * weights_x[f];
        hg += original_image->pixels[offset + 1] * weights_x[f];
        hr += original_image->pixels[offset + 2] * weights_x[f];
        vb += original_image->pixels[offset + 0] * weights_y[f];
        vg += original_image->pixels[offset + 1] * weights_y[f];
        vr += original_image->pixels[offset + 2] * weights_y[f];
      }

      float edge_strength =
          sqrtf(hr * hr + hg * hg + hb * hb + vr * vr + vg * vg + vb * vb) /
          sqrtf(3.0f);

      uint8_t r = original_image->pixels[i + 2];
      uint8_t g = original_image->pixels[i + 1];
      uint8_t b = original_image->pixels[i + 0];

      if (!filter_params->sharpen_show_edges) {
        int new_r = r - (int)(edge_strength * alpha);
        int new_g = g - (int)(edge_strength * alpha);
        int new_b = b - (int)(edge_strength * alpha);

        current_image->pixels[i + 2] = clamp_int(new_r);
        current_image->pixels[i + 1] = clamp_int(new_g);
        current_image->pixels[i + 0] = clamp_int(new_b);
        continue;
      }

      current_image->pixels[i + 2] = clamp_int((int)edge_strength);
      current_image->pixels[i + 1] = clamp_int((int)edge_strength);
      current_image->pixels[i + 0] = clamp_int((int)edge_strength);
    }
  }
}

// ------------------------------------------------------------------------

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
