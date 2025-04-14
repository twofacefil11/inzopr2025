// TODO: czytanie headerów z stb_image może mieć problem z unicodem, trzeba się
// tym zająć. przeczytać usage i remarks z headerów.
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define UNICODE
#define _UNICODE

#include <stdint.h>
#include <string.h>
#include <windows.h>

#include "stb_image.h"
#include "stb_image_write.h"
#include "filters.h"

// #include <stdbool.h>  //winkwink
typedef enum { false, true } bool;

static bool quit = false;

typedef enum { PNG, JPG, BMP, TGA, HDR } ExportFormat;

// frame prosto dla windowsa
struct Frame {
  int width;
  int height;
  uint32_t *pixels; // uwaga uwaga, od windowsa 10, windows alokuje miejsce
                    // samodzielnie na tą array.
} frame = {0};

typedef struct {
  Image *image_data;
  struct Frame *frame;
} AppState;

LRESULT CALLBACK WindowProcessMessage(HWND, UINT, WPARAM, LPARAM);

// formatowanie dla windowsa do rysowania
void blit_to_frame(Image *src, struct Frame *frame);

void export_image(char const *filename, int width, int height,
                  unsigned char *pixels, ExportFormat FORMAT);

static BITMAPINFO frame_bitmap_info;

static HBITMAP frame_bitmap = 0;
static HDC frame_device_context = 0;

// TEST
static Filter test_filter_type = NEGATIVE;
const char* input_image = "test_assets/test.jpg";
const char* output_image = "build/output/out";

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine,
                   int nCmdShow) {

  AppState app; // możliwe że należało by mallocowac pamięć zobaczymy
  app.frame = &frame;

  // deklaracja klasy okna głównego aplikacji. win32 stuff, nie dotykać raczej
  const wchar_t window_class_name[] = L"Main_screen";
  static WNDCLASS window_class = {0};
  window_class.lpfnWndProc = WindowProcessMessage;
  window_class.hInstance = hInstance;
  window_class.lpszClassName = window_class_name;
  RegisterClass(&window_class);

  // windowsowy sposób reprezentacja przestrzeni do rysowania
  // nie przejmujmy się tym dla naszego dobra
  frame_bitmap_info.bmiHeader.biSize = sizeof(frame_bitmap_info.bmiHeader);
  frame_bitmap_info.bmiHeader.biPlanes = 1;
  frame_bitmap_info.bmiHeader.biBitCount = 32;
  frame_bitmap_info.bmiHeader.biCompression = BI_RGB;
  frame_device_context = CreateCompatibleDC(0);

  static HWND window_handle;

  // trzeba poda
  SetWindowLongPtr(window_handle, GWLP_USERDATA, (LONG_PTR)&app);

  window_handle = CreateWindow(
      window_class_name,
      L"Test wyswietlania obrazu", // nazwa okna. najważniejsza żecz
      WS_OVERLAPPEDWINDOW | WS_VISIBLE, 640, 300, 400, 600, NULL, NULL,
      hInstance, NULL);

  if (window_handle == NULL)
    return -1;

  // Struct reprezentujący załadowany obraz, gd

  Image image_data;

  // Tutaj ładujemy do buffera info o obrazku trzeba to ędzie wyabstrachowac
  //
  // jakoś potem, jak będą UI calls
  image_data.pixels = stbi_load(input_image, &image_data.width,
                                &image_data.height, &image_data.channels, 4);

  // sztuczny resize na początku działania programu just in case, podobno dobre.
  RECT rect = {0, 0, image_data.width, image_data.height};

  UpdateWindow(window_handle);
  SendMessage(window_handle, WM_SIZE, 0,
              MAKELPARAM(image_data.width, image_data.height));
  app.image_data = &image_data;

  SetWindowPos(window_handle, NULL, 0, 0, rect.right - rect.left,
               rect.bottom - rect.top, SWP_NOZORDER | SWP_NOMOVE);
  // jest szansa że tego loopa nie będzie, jak będą już eventy od UI ale im

  MoveWindow(window_handle, 0, 0, rect.right - rect.left,
             rect.bottom - rect.top, TRUE);
  // TEST: Tę funckje powinny wywoływać elementy UI. puki co test, no filter.
  apply_filter(&image_data, test_filter_type); // TEST puki co tylko raz
  // po tym będzie też basic correction coś. ale to ogarniemy;

  while (!quit) {

    // ------- OFF - LIMITS -------
    static MSG message = {0};
    while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
      DispatchMessage(&message);
    }
    // ------- OFF - LIMITS -------

    // TEST: wiado dla windowsa żeby resizować jeżeli trzeba. puki co łatwiej
    // dać tyle pixeli ile ma obraz
    // SendMessage(window_handle, WM_SIZE, 0,
    //             MAKELPARAM(client_width, client_height));

    // translate image to frame
    blit_to_frame(&image_data, &frame);

    InvalidateRect(window_handle, NULL, FALSE);
    UpdateWindow(window_handle);
  }
  //ważne:
  //WARN: wprpwadzmy ścieżki z POZIOMU, Z KTÓREGO BĘDZIEMY ODPALAĆ PROGRAM. NIE Z "PUNKTU WIDZENIA" main.c
  export_image(output_image, image_data.width, image_data.height, image_data.pixels,
               JPG);

  // WARN: cleanup
  stbi_image_free(image_data.pixels);

  return 0;
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

LRESULT CALLBACK WindowProcessMessage(HWND window_handle, UINT message,
                                      WPARAM wParam, LPARAM lParam) {

  AppState *app = (AppState *)GetWindowLongPtr(window_handle, GWLP_USERDATA);

  switch (message) {

  case WM_QUIT: // jak jest puste to windows robi default
                // case WM_COMMAND: // TODO: tutaj będą sygnały z UI
                // case WM_CREATE: // TODO:inicjalizacja żeczy po stworzeniu
                // okna case WM_COMMAND: //TODO: messages z UI
                //   switch (LOWORD(wParam)) {
                //     case ID_FILTER_GRAYSCALE:
                //       apply_filter(pixel_array, FILTER_GRAYSCALE);
                //       InvalidateRect(hwnd, NULL, TRUE); // Trigger redraw
                //       break;
                //       // More filters...
                //   }
                //     break;

  case WM_DESTROY: {
    quit = true;
    free(app);
  } break; // kunic

  case WM_PAINT: {
    static PAINTSTRUCT paint;
    static HDC device_context;
    device_context = BeginPaint(window_handle, &paint);
    // blit_to_frame(app->image_data , &frame);

    // BitBlt to całkiem oldschoolowa nazwa look it up, its cool
    BitBlt(device_context, paint.rcPaint.left, paint.rcPaint.top,
           paint.rcPaint.right - paint.rcPaint.left,
           paint.rcPaint.bottom - paint.rcPaint.top, frame_device_context,
           paint.rcPaint.left, paint.rcPaint.top, SRCCOPY);

    EndPaint(window_handle, &paint);
  } break;

  // resize msg
  case WM_SIZE: {
    // TODO: centerowanie and all
    frame.width = LOWORD(lParam);
    frame.height = HIWORD(lParam);

    frame_bitmap_info.bmiHeader.biWidth = LOWORD(lParam);
    frame_bitmap_info.bmiHeader.biHeight = HIWORD(lParam);

    if (frame_bitmap)
      DeleteObject(frame_bitmap);

    frame_bitmap = CreateDIBSection(NULL, &frame_bitmap_info, DIB_RGB_COLORS,
                                    (void **)&frame.pixels, 0, 0);
    SelectObject(frame_device_context, frame_bitmap);

    // Te dwa redrawują. One does not call wm_print by hand.
    InvalidateRect(window_handle, NULL, FALSE);
    UpdateWindow(window_handle);

  } break; // DONT // ASK

  default: {
    return DefWindowProc(window_handle, message, wParam, lParam);
  }
  }
  return 0;
}

// coś jak będziemy debugować testować i logować to pozamieniać na result
// return type.
void apply_black_and_white(Image *image_data) {

  for (int y = 0; y < image_data->height; y++) {
    for (int x = 0; x < image_data->width; x++) {
      int i = (y * image_data->width + x) * 4; // forcujemy 4 channele

      // rozszczepiamy
      uint8_t r, g, b;
      r = image_data->pixels[i + 0];
      g = image_data->pixels[i + 1];
      b = image_data->pixels[i + 2];

      int bw_value = (r + g + b) / 3;

      image_data->pixels[i + 0] = bw_value;
      image_data->pixels[i + 1] = bw_value;
      image_data->pixels[i + 2] = bw_value;
      image_data->pixels[i + 3] = 255; // alpha i think

      // zapaku do komórki
      // image_data->pixels[i] = (a << 24) | (r << 16) | (g << 8) | b;
    }
  }
  // TODO
}

void apply_negative(Image *image_data) {

  for (int y = 0; y < image_data->height; y++) {
    for (int x = 0; x < image_data->width; x++) {
      int i = (y * image_data->width + x) * 4; // forcujemy 4 channele

      // rozszczepiamy
      uint8_t r, g, b;
      r = image_data->pixels[i + 0];
      g = image_data->pixels[i + 1];
      b = image_data->pixels[i + 2];

      image_data->pixels[i + 0] = 255 - r;
      image_data->pixels[i + 1] = 255 - g;
      image_data->pixels[i + 2] = 255 - b;
      image_data->pixels[i + 3] = 255; // alpha i think
    }
  }
}

void apply_blur(Image *image_data, unsigned char *ref_pixels) {
  // TODO:
  // blur powinien tez mieć zakres. gaussian chyba najłatwiejszy, itak nikogo
  // nie obhodzi
  // for (int y = 0; y < image_data->height; y++) {
  //   for (int x = 0; x < image_data->width; x++) {
  //     int i = (y * image_data->width + x) * 4;

  //     // matrix na obliczanie bluru typu gausian
  //     float kernel[3][3] = {{1 / 16.0f, 2 / 16.0f, 1 / 16.0f},
  //                           {2 / 16.0f, 4 / 16.0f, 2 / 16.0f},
  //                           {1 / 16.0f, 2 / 16.0f, 1 / 16.0f}};

  //     uint8_t r, g, b;
  //     for (int t = 0; t < 10; t++) {

  //       for (int ky = -1; ky <= 1; ++ky) {
  //         for (int kx = -1; kx <= 1; ++kx) {

  //           int sampleX = x + kx;
  //           int sampleY = y + ky;
  //           int sampleIndex = (sampleY * image_data->width + sampleX) * 4;

  //           float weight = kernel[ky + 1][kx + 1];
  //           r = ref_pixels[sampleIndex + 0] * weight;
  //           g = ref_pixels[sampleIndex + 1] * weight;
  //           b = ref_pixels[sampleIndex + 2] * weight;
  //         }
  //       }
  //     }

  //     // rozszczepiamy
  //     r = image_data->pixels[i + 0];
  //     g = image_data->pixels[i + 1];
  //     b = image_data->pixels[i + 2];

  //     // image_data->pixels[i + 0] = r * coef;
  //     // image_data->pixels[i + 1] = g * coef;
  //     // image_data->pixels[i + 2] = b * coef;
  //     // image_data->pixels[i + 3] = 255;// alpha i think
  //   }
  // }
}

void apply_amplify(Image *image_data, double coef) {

  for (int y = 0; y < image_data->height; y++) {
    for (int x = 0; x < image_data->width; x++) {
      int i = (y * image_data->width + x) * 4; // forcujemy 4 channele

      // rozszczepiamy
      uint8_t r, g, b;
      r = image_data->pixels[i + 0];
      g = image_data->pixels[i + 1];
      b = image_data->pixels[i + 2];

      image_data->pixels[i + 0] = r * coef;
      image_data->pixels[i + 1] = g * coef;
      image_data->pixels[i + 2] = b * coef;
      image_data->pixels[i + 3] = 255; // alpha i think
    }
  }

  // TODO
}

// jest szansa, że edytor krzyczy na liczbe argumentów w bmp, tga i hdr. compiles just fine so leave it or test it whatever.
void export_image(char const *filename, int width, int height,
                  unsigned char *pixels, ExportFormat FORMAT) {

  int quality =
      100; // jpg ma wybór quality od 0 do 100. puki co hardcoduje bo nie wiem
           // jak to info będzie podawane. stbi_write_jpg(filename, width,
           // height, 4, pixels, width * 4, quality);
  char *extention;

  switch (FORMAT) {
  case PNG:
    extention = ".png";
    stbi_write_png(strcat((char*)filename, extention), width, height, 4, pixels, width * 4);
    break;
  case JPG:
    extention = ".jpg";
    stbi_write_jpg(strcat((char*)filename, extention), width, height, 4, pixels,
                   quality);
    break;
  case BMP:
    extention = ".bmp";
    stbi_write_bmp(strcat((char*)filename, extention), width, height, 4, pixels);
    break;
  case TGA:
    extention = ".tga";
    stbi_write_tga(strcat((char*)filename, extention), width, height, 4, pixels);
    break;
  case HDR:
    extention = ".hdr";
    stbi_write_hdr(strcat((char*)filename, extention), width, height, 4, (float*)pixels);
    break;
  }
}
