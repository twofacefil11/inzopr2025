// TODO: czytanie headerów z stb_image może mieć problem z unicodem, trzeba się
//
// tym zająć. przeczytać usage i remarks z headerów.
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define UNICODE
#define _UNICODE


#include <stdint.h>
#include <windows.h>
#include <stdbool.h> //winkwink i had myown for a secnd itmight have broken sth
//
// #include "stb_image.h"
// #include "stb_image_write.h"

// #include "image.h"
#include "app_state.h"
#include "filters.h"
#include "io.h"

LRESULT CALLBACK WindowProcessMessage(HWND, UINT, WPARAM, LPARAM);

// formatowanie dla windowsa do rysowania
void blit_to_frame(Image *src, struct Frame *frame);

// void export_image(char *filename, int width, int height, unsigned char *pixels,
//                   ExportFormat FORMAT);

// static BITMAPINFO frame_bitmap_info;
// static HBITMAP frame_bitmap = 0;
// static HDC frame_device_context = 0;
//
// WŁAŚNIE PRÓBUJE PRZENIEŚĆ TO DO STATE


void LoadImageFromFile(AppState *app, const wchar_t *filepath) {
  if (!app)
    return;

  // Free previous image if exists
  if (app->image_data && app->image_data->pixels) {
    stbi_image_free(app->image_data->pixels);
    app->image_data->pixels = NULL;
  }

  // Convert wchar_t filepath to UTF-8 for stbi_load (stb_image expects char*)
  int len = WideCharToMultiByte(CP_UTF8, 0, filepath, -1, NULL, 0, NULL, NULL);
  if (len == 0)
    return; // conversion failed

  char *utf8path = malloc(len);
  if (!utf8path)
    return;

  WideCharToMultiByte(CP_UTF8, 0, filepath, -1, utf8path, len, NULL, NULL);

  int width, height, channels;
  unsigned char *pixels = stbi_load(utf8path, &width, &height, &channels, 4);
  free(utf8path);

  if (!pixels) {
    MessageBox(app->hwnd_main, L"Failed to load image.", L"Error",
               MB_ICONERROR);
    return;
  }

  // Update image data in app state
  if (!app->image_data) {
    app->image_data = malloc(sizeof(Image));
    if (!app->image_data) {
      stbi_image_free(pixels);
      return;
    }
  }

  app->image_data->width = width;
  app->image_data->height = height;
  app->image_data->channels = 4; // forcing 4 channels output
  app->image_data->pixels = pixels;

  // Resize frame (send WM_SIZE to main window)
  SendMessage(app->hwnd_main, WM_SIZE, 0, MAKELPARAM(width, height));

  // Redraw window
  InvalidateRect(app->hwnd_main, NULL, TRUE);
  UpdateWindow(app->hwnd_main);

// ------------------------------------------------
}


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
    LoadImageFromFile(app, ofn.lpstrFile);
  }
}
// ------------------------------------------------
// TEST
static Filter test_filter_type = NEGATIVE;
char *input_image = "test_assets/test.jpg";
char *output_image = "build/output/out";

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine,
                   int nCmdShow) {

  AppState app = {0}; // możliwe że należało by mallocowac pamięć zobaczymy
  struct Frame fr;
  app.frame = &fr;
  app.frame->frame_bitmap = 0;
  app.frame->frame_device_context = 0;


  app.shouldQuit = false;

  // deklaracja klasy okna głównego aplikacji. win32 stuff, nie dotykać raczej
  const wchar_t window_class_name[] = L"Main_screen";
  static WNDCLASS window_class = {0};
  window_class.lpfnWndProc = WindowProcessMessage;
  window_class.hInstance = hInstance;
  window_class.lpszClassName = window_class_name;
  RegisterClass(&window_class);

  // windowsowy sposób reprezentacja przestrzeni do rysowania
  // nie przejmujmy się tym dla naszego dobra
  app.frame->frame_bitmap_info.bmiHeader.biSize = sizeof(app.frame->frame_bitmap_info.bmiHeader);
  app.frame->frame_bitmap_info.bmiHeader.biPlanes = 1;
  app.frame->frame_bitmap_info.bmiHeader.biBitCount = 32;
  app.frame->frame_bitmap_info.bmiHeader.biCompression = BI_RGB;
  app.frame->frame_device_context = CreateCompatibleDC(0);

  static HWND window_handle;

  // trzeba poda

  window_handle = CreateWindow(
      window_class_name,
      L"Test wyswietlania obrazu", // nazwa okna. najważniejsza żecz
      WS_OVERLAPPEDWINDOW | WS_VISIBLE, 640, 300, 400, 600, NULL, NULL,
      hInstance, &app);

  if (window_handle == NULL)
    return -1;

  app.hwnd_main = window_handle;

  // SIDEBAR
  app.hwnd_sidebar = CreateWindowEx(
      0, L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_NOTIFY | WS_BORDER, 0, 0,
      200, 100, // position + width (height will be set later)
      window_handle, NULL, hInstance, &app);

  /// MENU NA GÓRZE
  HMENU hMenubar = CreateMenu();
  HMENU hFile = CreateMenu();
  AppendMenu(hFile, MF_STRING, 1, L"Open...");
  AppendMenu(hFile, MF_STRING, 2, L"Save");
  AppendMenu(hFile, MF_SEPARATOR, 0, NULL);
  AppendMenu(hFile, MF_STRING, 3, L"Exit");
  AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hFile, L"File");

  HMENU hEdit = CreateMenu();
  AppendMenu(hEdit, MF_STRING, 4, L"Undo");
  AppendMenu(hEdit, MF_STRING, 5, L"Redo");
  AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hEdit, L"Edit");

  HMENU hInfo = CreateMenu();
  AppendMenu(hInfo, MF_STRING, 6, L"About");
  AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hInfo, L"Info");

  SetMenu(window_handle, hMenubar);
  // MENU NA GÓRZE

  HWND hwndDropdown = CreateWindowEx(
      0, L"COMBOBOX", NULL, CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE, 10, 10,
      150, 100, window_handle, (HMENU)10, hInstance, NULL);

  SendMessage(hwndDropdown, CB_ADDSTRING, 0, (LPARAM)L"Negative");
  SendMessage(hwndDropdown, CB_ADDSTRING, 0, (LPARAM)L"Grayscale");
  SendMessage(hwndDropdown, CB_ADDSTRING, 0, (LPARAM)L"Brightness");
  SendMessage(hwndDropdown, CB_SETCURSEL, 0, 0); // Select first item

  SetWindowLongPtr(window_handle, GWLP_USERDATA, (LONG_PTR)&app);
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

  while (!app.shouldQuit) {

    // ------- OFF - LIMITS -------
    static MSG message = {0};
    while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&message);
      DispatchMessage(&message);
    }
    // ------- OFF - LIMITS -------

    // TEST: wiado dla windowsa żeby resizować jeżeli trzeba. puki co łatwiej
    // dać tyle pixeli ile ma obraz
    // SendMessage(window_handle, WM_SIZE, 0,
    //             MAKELPARAM(client_width, client_height));

    // translate image to frame
    // blit_to_frame(&image_data, &frame);
    blit_to_frame(app.image_data, app.frame);

    InvalidateRect(app.hwnd_main, NULL, FALSE);
    UpdateWindow(app.hwnd_main);
  }
  // ważne:
  // WARN: wprpwadzmy ścieżki z POZIOMU, Z KTÓREGO BĘDZIEMY ODPALAĆ PROGRAM. NIE
  // Z "PUNKTU WIDZENIA" main.c
  export_image(output_image, image_data.width, image_data.height,
               image_data.pixels, JPG);

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


