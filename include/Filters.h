#pragma once

#ifndef FILTERS_H
#define FILTERS_H

#include "Image.h"
#include <stdbool.h>

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


} Filter_params;

//-------------------------------------------------------------------------------

/// TODO: trzeba pożądnie przemyśleć jak reprezentowac dany filtr i jego opcje.
/// Podejrzewam, że przy drzewie zachowań bedzie trzebe "zhardocować" część.
// typedef struct {
//   Filter_type filter_type;

// } Filter;
// TODO
void apply_blur(Image *original_image, Image *current_image, Filter_params *filter_params);
void apply_sharpen(Image *original_image, Image *current_image,Filter_params *filter_params);
void apply_monochrome(Image *original_image, Image *current_image, Filter_params *filter_params);
void apply_negative(Image *original_image, Image *current_image);
void apply_amplify(Image *original_image, Image *current_image, Filter_params *filter_params, bool CLAMP_AMPLIFY);
void apply_sepia(Image *original_image, Image *current_image);

void reapply_effects(Image *original_image, Image *current_image, Filter_type type);

#endif // FILTERS_H
