#include "app_state.h"

void AppState_SetImage(AppState *app, Image *new_image) {
  if (app->image_data) {
    stbi_image_free(app->image_data->pixels);
    free(app->image_data);
  }
  app->image_data = new_image;
}
