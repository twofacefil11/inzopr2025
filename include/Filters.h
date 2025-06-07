#pragma once

#ifndef FILTERS_H
#define FILTERS_H

#include "Image.h"
#include <stdbool.h>
#include <math.h>
typedef enum {
  BLUR,
  SHARPEN,
  SEPIA,
  AMPLIFY,
  NEGATIVE,
  BW,       
  NO_FILTER
} Filter_type;

//-------------------------------------------------------------------------------

typedef struct {
  double amplify_r;
  double amplify_g;
  double amplify_b;
  bool clamp_amplify;

  double mono_r; // -255 -255
  double mono_g;
  double mono_b;

  int blur_times;

  bool sharpen_sobel;
  bool sharpen_prewitt;
  bool sharpen_show_edges;
  int sharpen_mix_const;

} Filter_params;

//-------------------------------------------------------------------------------

/// TODO: trzeba pożądnie przemyśleć jak reprezentowac dany filtr i jego opcje.
/// Podejrzewam, że przy drzewie zachowań bedzie trzebe "zhardkować" część.
// typedef struct {
//   Filter_type filter_type;

// } Filter;
// TODO
void apply_blur(Image *original_image, Image *current_image, Filter_params *filter_params);
void apply_sharpen(Image *original_image, Image *current_image,Filter_params *filter_params);
void apply_monochrome(Image *original_image, Image *current_image, Filter_params *filter_params);
void apply_negative(Image *original_image, Image *current_image);
void apply_amplify(Image *original_image, Image *current_image, Filter_params *filter_params);
void apply_sepia(Image *original_image, Image *current_image);

void reapply_effects(Image *original_image, Image *current_image, Filter_type type);
static inline uint8_t clamp_int(int value);
#endif // FILTERS_H
