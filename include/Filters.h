#pragma once

#ifndef FILTERS_H
#define FILTERS_H

#include "Image.h"

typedef enum {
  BLUR,
  SHARPEN,
  SEPIA,
  AMPLIFY,
  NEGATIVE,
  BW,       
  NO_FILTER
} Filter_type;

/// TODO: trzeba pożądnie przemyśleć jak reprezentowac dany filtr i jego opcje.
/// Podejrzewam, że przy drzewie zachowań bedzie trzebe "zhardocować" część.
typedef struct {
  Filter_type filter_type;

} Filter;
// TODO
// void apply_blur(Image *image_data, unsigned char *ref_pixels);
void apply_sharpen(Image *image_data);
void apply_monochrome(Image *image_data);
void apply_negative(Image *image_data);
void apply_amplify(Image *image_data); // todo
void apply_sepia(Image *image_data);

#endif // FILTERS_H
