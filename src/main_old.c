#define UNICODE
#define _UNICODE
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <windows.h>

#define PRINT_ERROR(a, args...)                                                \
  printf("ERROR %s() %s Line %d: " a "\n", __FUNCTION__, __FILE__, __LINE__,   \
         ##args);

// #define PRINT_ERROR(a, ...)                                                    \
//   printf("ERROR %s() %s Line %d: " a "\n", __FUNCTION__, __FILE__, __LINE__,   \
//          __VA_ARGS__)

bool quit = false;
HWND window_handle;
HDC device_context;
HBITMAP bitmap;
BITMAPINFO bitmap_info;

typedef struct {
  union {
    int width, w;
  };
  union {
    int height, h;
  };
  union {
    uint32_t *pixels, *p;
  };
} sprite_t;

sprite_t frame;

LRESULT CALLBACK WindowProcessMessage(HWND window_handle, UINT message,
                                      WPARAM wParam, LPARAM lParam);
bool LoadSprite(sprite_t *sprite, const char *filename);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nShowCmd) {
  const wchar_t window_class_name[] = L"Window Class";
  static WNDCLASS window_class = {0};
  window_class.lpfnWndProc = WindowProcessMessage;
  window_class.hInstance = hInstance;
  window_class.lpszClassName = window_class_name;
  RegisterClass(&window_class);

  bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
  bitmap_info.bmiHeader.biPlanes = 1;
  bitmap_info.bmiHeader.biBitCount = 32;
  bitmap_info.bmiHeader.biCompression = BI_RGB;
  device_context = CreateCompatibleDC(0);

  window_handle = CreateWindow(window_class_name, L"Learn to Program Windows",
                               WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT,
                               CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                               NULL, NULL, hInstance, NULL);
  if (window_handle == NULL) {
    PRINT_ERROR("CreateWindow failed");
    return -1;
  }

  static sprite_t sprite;
  if (!LoadSprite(&sprite, "test.bmp")) {
    PRINT_ERROR("Failed to load sprite: \"colorwheel.bmp\"");
    return -1;
  }
  int quit = 0;
  while (!quit == 1000000) {
    quit++;
    static MSG message = {0};
    while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
      DispatchMessage(&message);
    }

    static uint32_t *p;
    p = frame.pixels;
    for (int n = 0; n < frame.w * frame.h; ++n) {
      *(p++) = (float)n / (float)(frame.w * frame.h) * (uint32_t)(-1);
    }

#if 1
    static uint32_t *frame_pointer, *sprite_pointer;
    static int sprite_byte_width;
    frame_pointer = frame.pixels;
    sprite_pointer = sprite.pixels;
    sprite_byte_width = sprite.w * 4;
    for (int y = 0; y < sprite.h; ++y) {
      memcpy(frame_pointer, sprite_pointer, sprite_byte_width);
      frame_pointer += frame.w;
      sprite_pointer += sprite.w;
    }

#else
    for (int y = 0; y < sprite.h; ++y) {
      for (int x = 0; x < sprite.w; ++x) {
        static uint32_t source_index, target_index;
        static float alpha, anti_alpha;
        static uint32_t sr, sg, sb; // Source
        static uint32_t tr, tg, tb; // Target
        source_index = x + y * sprite.w;
        target_index = x + y * frame.w;
        alpha = (float)((sprite.p[source_index] & 0xff000000) >> 24) / 255.f;
        anti_alpha = 1.f - alpha;
        sr = (uint32_t)(((sprite.p[source_index] & 0x00ff0000) >> 16) * alpha)
             << 16;
        sg = (uint32_t)(((sprite.p[source_index] & 0x0000ff00) >> 8) * alpha)
             << 8;
        sb = (sprite.p[source_index] & 0x000000ff) * alpha;
        tr = (uint32_t)(((frame.p[target_index] & 0x00ff0000) >> 16) *
                        anti_alpha)
             << 16;
        tg =
            (uint32_t)(((frame.p[target_index] & 0x0000ff00) >> 8) * anti_alpha)
            << 8;
        tb = (frame.p[target_index] & 0x000000ff) * anti_alpha;
        frame.pixels[target_index] = sb + tb + sg + tg + sr + tr;
      }
    }
#endif

    InvalidateRect(window_handle, NULL, FALSE);
    UpdateWindow(window_handle);
  }

  return 0;
}

LRESULT CALLBACK WindowProcessMessage(HWND window_handle, UINT message,
                                      WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case WM_DESTROY: {
    PostQuitMessage(0);
    quit = true;
  } break;

  case WM_PAINT: {
    static PAINTSTRUCT paint;
    static HDC paint_device_context;
    paint_device_context = BeginPaint(window_handle, &paint);
    BitBlt(paint_device_context, paint.rcPaint.left, paint.rcPaint.top,
           paint.rcPaint.right - paint.rcPaint.left,
           paint.rcPaint.bottom - paint.rcPaint.top, device_context,
           paint.rcPaint.left, paint.rcPaint.top, SRCCOPY);
    EndPaint(window_handle, &paint);
  } break;

  case WM_SIZE: {
    frame.width = bitmap_info.bmiHeader.biWidth = LOWORD(lParam);
    frame.height = bitmap_info.bmiHeader.biHeight = HIWORD(lParam);
    if (bitmap)
      DeleteObject(bitmap);
    bitmap = CreateDIBSection(NULL, &bitmap_info, DIB_RGB_COLORS,
                              (void **)&frame.pixels, 0, 0);
    SelectObject(device_context, bitmap);
  } break;

  case WM_KEYDOWN: {
    if (wParam == VK_ESCAPE) {
      PostQuitMessage(0);
      quit = true;
    }
  } break;

  default:
    return DefWindowProc(window_handle, message, wParam, lParam);
  }
  return 0;
}

/* Bitmap file format
 *
 * SECTION
 * Address:Bytes	Name
 *
 * HEADER:
 *	  0:	2		"BM" magic number
 *	  2:	4		file size
 *	  6:	4		junk
 *	 10:	4		Starting address of image data
 * BITMAP HEADER:
 *	 14:	4		header size
 *	 18:	4		width  (signed)
 *	 22:	4		height (signed)
 *	 26:	2		Number of color planes
 *	 28:	2		Bits per pixel
 *	[...]
 * [OPTIONAL COLOR PALETTE, NOT PRESENT IN 32 BIT BITMAPS]
 * BITMAP DATA:
 *	DATA:	X	Pixels
 */
bool LoadSprite(sprite_t *sprite, const char *filename) {
  bool return_value = true;

  uint32_t image_data_address;
  int32_t width;
  int32_t height;
  uint32_t pixel_count;
  uint16_t bit_depth;
  uint8_t byte_depth;
  uint32_t *pixels;

  printf("Loading bitmap file: %s\n", filename);

  FILE *file;
  file = fopen(filename, "rb");
  if (file) {
    if (fgetc(file) == 'B' && fgetc(file) == 'M') {
      printf("BM read; bitmap file confirmed.\n");
      fseek(file, 8, SEEK_CUR);
      fread(&image_data_address, 4, 1, file);
      fseek(file, 4, SEEK_CUR);
      fread(&width, 4, 1, file);
      fread(&height, 4, 1, file);
      fseek(file, 2, SEEK_CUR);
      fread(&bit_depth, 2, 1, file);
      if (bit_depth != 32) {
        PRINT_ERROR("(%s) Bit depth expected %d is %d", filename, 32,
                    bit_depth);
        return_value = false;
      } else { // Image metadata correct
        printf("image data address:\t%d\nwidth:\t\t\t%d pix\nheight:\t\t\t%d "
               "pix\nbit depth:\t\t%d bpp\n",
               image_data_address, width, height, bit_depth);
        pixel_count = width * height;
        byte_depth = bit_depth / 8;
        pixels = malloc(pixel_count * byte_depth);
        if (pixels) {
          fseek(file, image_data_address, SEEK_SET);
          int pixels_read = fread(pixels, byte_depth, pixel_count, file);
          printf("Read %d pixels\n", pixels_read);
          if (pixels_read == pixel_count) {
            sprite->w = width;
            sprite->h = height;
            sprite->p = pixels;
          } else {
            PRINT_ERROR("(%s) Read pixel count incorrect. Is %d expected %d",
                        filename, pixels_read, pixel_count);
            free(pixels);
            return_value = false;
          }
        } else {
          printf("(%s) Failed to allocate %d pixels.\n", filename, pixel_count);
          return_value = false;
        }
      } // Done loading sprite
    } else {
      PRINT_ERROR("(%s) First two bytes of file are not \"BM\"", filename);
      return_value = false;
    }

    fclose(file);
  } else {
    PRINT_ERROR("(%s) Failed to open file", filename);
    return_value = false;
  }
  return return_value;
}
