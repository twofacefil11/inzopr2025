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
