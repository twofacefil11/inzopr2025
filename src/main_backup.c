
#define STB_IMAGE_IMPLEMENTATION
#define UNICODE
#define _UNICODE

#include "../include/stb_image.h"
#include <stdint.h>
#include <windows.h>

// #include <stdbool.h>  //winkwink
typedef enum { false, true } bool;

static bool quit = false;

// typy filtrów
typedef enum { NO_FILTER, BW, REVERSED } Filter;

struct Frame {
  int width;
  int height;
  uint32_t* pixels;
} frame = {0};

typedef struct {
  int width;
  int height;
  int channels;
  unsigned char *pixels;
} Image;

LRESULT CALLBACK WindowProcessMessage(HWND, UINT, WPARAM, LPARAM);

void apply_filter(Image *image_data, Filter filter_type);

//specificfilter functions 
void apply_black_and_white(Image *image_data);
void apply_reversed(Image *image_data);

//formatowanie dla windowsa do rysowania
void blit_to_frame(Image *src, struct Frame* frame);

static BITMAPINFO frame_bitmap_info;

static HBITMAP frame_bitmap = 0;
static HDC frame_device_context = 0;

// TEST
static Filter test_filter_type = NO_FILTER;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine,
                   int nCmdShow) {

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
  window_handle = CreateWindow(
      window_class_name,
      L"Test wyswietlania obrazu", // nazwa okna. najważniejsza żecz
      WS_OVERLAPPEDWINDOW | WS_VISIBLE, 640, 300, 200, 200, NULL, NULL,
      hInstance, NULL);


  if (window_handle == NULL)
    return -1;

  // sztuczny resize na początku działania programu just in case, podobno dobre.
  RECT rect;
  int client_width = rect.right - rect.left;
  int client_height = rect.bottom - rect.top;

  GetClientRect(window_handle, &rect);
  UpdateWindow(window_handle);

  SendMessage(window_handle, WM_SIZE, 0, MAKELPARAM(client_width, client_height));

  // Struct reprezentujący załadowany obraz, gd
  Image image_data;

  // Tutaj ładujemy do buffera info o obrazku
  image_data.pixels = stbi_load("test.bmp", &image_data.width,
                                &image_data.height, &image_data.channels, 4);

  // jest szansa że tego loopa nie będzie, jak będą już eventy od UI ale im trippin, nw
  while (!quit) {

    // ------- OFF - LIMITS -------
    static MSG message = {0};
    while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
      DispatchMessage(&message);
    }
    // ------- OFF - LIMITS -------


    // TEST: wiado dla windowsa żeby resizować jeżeli trzeba. puki co łatwiej dać tyle pixeli ile ma obraz
    SendMessage(window_handle, WM_SIZE, 0, MAKELPARAM(client_width, client_height));


    // TEST: Tę funckje powinny wywoływać elementy UI. puki co test, no filter.
    apply_filter(&image_data, test_filter_type); // TEST
  

    blit_to_frame(&image_data, &frame);
    InvalidateRect(window_handle, NULL, FALSE);
    UpdateWindow(window_handle);
  }

  // WARN: cleanup
  stbi_image_free(image_data.pixels);

  return 0;
}

void blit_to_frame(Image *src, struct Frame *dst) {
  // TODO: logika kontroli wielkości obrazu i okna i środkowania and all that
  // puki co są takie półśrodki
    int width = (src->width < dst->width) ? src->width : dst->width;
    int height = (src->height < dst->height) ? src->height : dst->height;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            int src_index = (y * src->width + x) * 4;
            int dst_index = ((dst->height - 1 - y) * dst->width + x); // trzeba przeżucić veritally bo windows tak chce.

            uint8_t r = src->pixels[src_index + 0];
            uint8_t g = src->pixels[src_index + 1];
            uint8_t b = src->pixels[src_index + 2];
            uint8_t a = src->pixels[src_index + 3];

            dst->pixels[dst_index] = (a << 24) | (r << 16) | (g << 8) | b;
        }
    }
}

void apply_filter(Image *image_data, Filter filter_type) {

  switch (filter_type) {

  case NO_FILTER: // do nothing of course
    break;

  case BW:
    apply_black_and_white(image_data); //TODO
    break;

  case REVERSED:
    apply_reversed(image_data); //TODO
    break;

  default:
    return; 
  }

  return; 
}

LRESULT CALLBACK WindowProcessMessage(HWND window_handle, UINT message,
                                      WPARAM wParam, LPARAM lParam) {
  switch (message) {

  case WM_QUIT: // jak jest puste to windows robi default

  case WM_DESTROY: {
    quit = true;
  } break; // dont ask

  case WM_PAINT: {
    static PAINTSTRUCT paint;
    static HDC device_context;
    device_context = BeginPaint(window_handle, &paint);
    // BitBlt to całkiem oldschoolowa nazwa look it up, its cool
    BitBlt(device_context, paint.rcPaint.left, paint.rcPaint.top,
           paint.rcPaint.right - paint.rcPaint.left,
           paint.rcPaint.bottom - paint.rcPaint.top, frame_device_context,
           paint.rcPaint.left, paint.rcPaint.top, SRCCOPY);
    EndPaint(window_handle, &paint);
  } break;

  case WM_SIZE: {
    frame_bitmap_info.bmiHeader.biWidth = LOWORD(lParam);
    frame_bitmap_info.bmiHeader.biHeight = HIWORD(lParam);

    if (frame_bitmap)
      DeleteObject(frame_bitmap);

    frame_bitmap = CreateDIBSection(NULL, &frame_bitmap_info, DIB_RGB_COLORS,
                                    (void **)&frame.pixels, 0, 0);
    SelectObject(frame_device_context, frame_bitmap);

    frame.width = LOWORD(lParam);
    frame.height = HIWORD(lParam);
  } break; // DONT // ASK

  default: {
    return DefWindowProc(window_handle, message, wParam, lParam);
  }

  }
  return 0;
}


//coś jak będziemy debugować testować i logować to pozamieniać na result return type.
void apply_black_and_white(Image *image_data) {
  //TODO
} 

void apply_reversed(Image *image_data) {
  //TODO
}


