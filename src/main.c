// ----------------------------------------------------------------------------
// main.c
// Responsibilities:
// boilerplate...
// init(); -> main window class/register and state init
// while {
//   msg processing. ONLY
// }
// ----------------------------------------------------------------------------

// NOTE(s)
// - nic nie dawać po loopie oprócz returna. cleanup ma być w odpowiednijej msg
//


// TODO: czytanie headerów z stb_image może mieć problem z unicodem, trzeba się
// tym zająć. przeczytać usage i remarks z headerów.

// stbi będzie zupełnie wywalone z tąd. pui co powoduje problemy
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define UNICODE
#define _UNICODE

#include <stdbool.h> //winkwink i had myown for a secnd itmight have broken sth
#include <stdint.h>
#include <windows.h>

#include "app_state.h"
#include "winproc.h"
#include "filters.h"
#include "io.h"

LRESULT CALLBACK WindowProcessMessage(HWND, UINT, WPARAM, LPARAM);


// PONIŻSZY BLOK DZIAŁAŁ I ŁADOWAŁ OBRAZKI POPRAWNIE ALE PRZENIOSŁĘM GO
// STATELESS DO IMAGE.C
// TODO TODO TODO TODO trzeba to będzie debagować w sensie to co jest w image.
// void LoadImageFromFile(AppState *app, const wchar_t *filepath) {
//   if (!app)
//     return;

//   // Free previous image if exists
//   if (app->image_data && app->image_data->pixels) {
//     stbi_image_free(app->image_data->pixels);
//     app->image_data->pixels = NULL;
//   }

//   // Convert wchar_t filepath to UTF-8 for stbi_load (stb_image expects
//   char*) int len = WideCharToMultiByte(CP_UTF8, 0, filepath, -1, NULL, 0,
//   NULL, NULL); if (len == 0)
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

// TEST DEBUG WHATEVER
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
  app.frame->frame_bitmap_info.bmiHeader.biSize =
      sizeof(app.frame->frame_bitmap_info.bmiHeader);
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
  /// ask why...
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
  // MARK
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
  MSG message;

  while (GetMessage(&message, NULL, 0, 0)) {
    TranslateMessage(&message);
    DispatchMessage(&message);

    // TEST: wiado dla windowsa żeby resizować jeżeli trzeba. puki co łatwiej
    // dać tyle pixeli ile ma obraz
    // SendMessage(window_handle, WM_SIZE, 0,
    //             MAKELPARAM(client_width, client_height));

    // translate image to frame
    //// To ważne: tego tu nie może być, należy zrobić system który powiadamia
    /// że coś się zmieniło i należy redrawawać
    blit_to_frame(app.image_data, app.frame); 
    // mark

    InvalidateRect(app.hwnd_main, NULL, FALSE);
    UpdateWindow(app.hwnd_main);
  }
  // ważne:
  // WARN: wprpwadzmy ścieżki z POZIOMU, Z KTÓREGO BĘDZIEMY ODPALAĆ PROGRAM. NIE
  // Z "PUNKTU WIDZENIA" main.c
  export_image(output_image, image_data.width, image_data.height,
               image_data.pixels, JPG);

  // WARN: cleanup
  // MARK
  stbi_image_free(image_data.pixels);

  return 0;
}

