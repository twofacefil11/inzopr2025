#ifndef FILTERS_H
#define FILTERS_H

#include "image.h"
#include <stdint.h>

// Typy filtrów
typedef enum {

  NO_FILTER, /*!< Nic nie robi */
  BW,        /*!< Black and White */
  NEGATIVE,  /*!< Odwraca kolory */
  AMPLIFY,
  BLUR

} Filter;

void apply_filter(Image *image_data, Filter filter_type);
// możliwe że to zniknie na poczet MW_COMMAND
// specificfilter functions

void apply_black_and_white(Image *image_data);
void apply_negative(Image *image_data);
void apply_blur(Image *image_data, unsigned char *ref_pixels);
void apply_amplify(Image *image_data, double coef); // todo

#endif
