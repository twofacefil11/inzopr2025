#include "image.h"

// void LoadImageFromFile(const wchar_t *filepath, Image *out_image) {
//   if (!out_image)
//     return;

//   // Free previous image if exists
//   if (app->image_data && app->image_data->pixels) {
//     stbi_image_free(app->image_data->pixels);
//     app->image_data->pixels = NULL;
//   }

//   // Convert wchar_t filepath to UTF-8 for stbi_load (stb_image expects char*)
//   int len = WideCharToMultiByte(CP_UTF8, 0, filepath, -1, NULL, 0, NULL, NULL);
//   if (len == 0)
//     return; // conversion failed

//   char *utf8path = malloc(len);
//   if (!utf8path)
//     return;

//   WideCharToMultiByte(CP_UTF8, 0, filepath, -1, utf8path, len, NULL, NULL);

//   int width, height, channels;
//   unsigned char *pixels = stbi_load(utf8path, &width, &height, &channels, 4);
//   free(utf8path);

//   if (!pixels) {
//     MessageBox(app->hwnd_main, L"Failed to load image.", L"Error",
//                MB_ICONERROR);
//     return;
//   }

//   // Update image data in app state
//   if (!app->image_data) {
//     app->image_data = malloc(sizeof(Image));
//     if (!app->image_data) {
//       stbi_image_free(pixels);
//       return;
//     }
//   }

//   app->image_data->width = width;
//   app->image_data->height = height;
//   app->image_data->channels = 4; // forcing 4 channels output
//   app->image_data->pixels = pixels;

//   // Resize frame (send WM_SIZE to main window)
//   SendMessage(app->hwnd_main, WM_SIZE, 0, MAKELPARAM(width, height));

//   // Redraw window
//   InvalidateRect(app->hwnd_main, NULL, TRUE);
//   UpdateWindow(app->hwnd_main);
// }



// ------------------------------------------------
