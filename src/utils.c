#include "utils.h"

void OpenFileDialog(HWND hwnd, AppState *app) {
  OPENFILENAME ofn = {0};
  wchar_t file_name[MAX_PATH] = L"";

  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = hwnd;
  ofn.lpstrFile = file_name;
  ofn.nMaxFile = MAX_PATH;
  ofn.lpstrFilter =
      L"Image Files\0*.png;*.jpg;*.bmp;*.tga;*.hdr\0All Files\0*.*\0";
  ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

  if (GetOpenFileName(&ofn)) {
    AppState_SetImage(app, LoadImageFromFile(ofn.lpstrFile));
  }
}

void blit_to_frame(Image *src, struct Frame *dst) {
  // TODO: logika kontroli wielkości obrazu i okna i środkowania and all that

  // puki co są takie półśrodki, żeby na pewno nie nabroić
  int width = (src->width < dst->width) ? src->width : dst->width;
  int height = (src->height < dst->height) ? src->height : dst->height;

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {

      // trzeba odliczać index, bo array jest jedno wymiarowa.
      int src_index = (y * src->width + x) * 4; // forcujemy 4 channele

      // trzeba obliczyć index jak w src_inde i przeżucić
      // veritally bo windows tak chce.
      int dst_index = ((dst->height - 1 - y) * dst->width + x);

      // WARN: Jest szansa, że jakieś kolory będą czasem zamienione?
      // Jeżeli tak to: dst->pixels[dst_index] = (a << 24) | (b << 16) | (g <<
      // 8) | r;

      uint8_t a, r, g, b;
      r = src->pixels[src_index + 0];
      g = src->pixels[src_index + 1];
      b = src->pixels[src_index + 2];
      a = 255;

      dst->pixels[dst_index] = (a << 24) | (r << 16) | (g << 8) | b;
    }
  }
}
