// #include "Image.h"

#include "window_procedure.h"
#include "Image.h"
#include <windef.h>
#include <windows.h>
#define DEBUG_EXPORT_NAME "output"
#define UNICODE
#define _UNICODE

LRESULT CALLBACK WindowProcessMessage(HWND hwnd, UINT message, WPARAM wParam,
                                      LPARAM lParam) {

  State *app = (State *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

  switch (message) {

    // ----------------------------------------------------

  case WM_CREATE: {
    // dzięki temu dziwactwu jest dostęp do app w messages
    CREATESTRUCT *cs = (CREATESTRUCT *)lParam;
    app = (State *)cs->lpCreateParams;
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)app);

    app->UI_handles.hSidebar =
        CreateWindowExW(0, L"STATIC", NULL,
                        WS_CHILD | WS_BORDER, // debug visible
                        10, 10, 200, 400, hwnd, (HMENU)200, NULL, (LPVOID)app);

    SetWindowSubclass(app->UI_handles.hSidebar, PanelProc, 0, (DWORD_PTR)app);

    init_UI(hwnd, &app->UI_handles);

    /// init side panel może do funkcji

    break;
  }
  case WM_CTLCOLORLISTBOX: { // MOżliwe że pozmieniamy kiedyś kolory. będzie
    HDC hdc = (HDC)wParam;
    SetBkColor(hdc, RGB(30, 30, 30));
    SetTextColor(hdc, RGB(220, 220, 220));
    return (INT_PTR)app->UI_handles.hComboBgBrush;
  }
  // ----------------------------------------------------
  case WM_DESTROY: {
    // TODO
    PostQuitMessage(0);
    free_state(app);
    break;
  }
  // ----------------------------------------------------
  case WM_KEYDOWN:
    if (wParam == VK_ESCAPE) {
      PostQuitMessage(0);
    }
    break;
  // ----------------------------------------------------
  case WM_QUIT: {
    // TODO
    break;
  }
  // ----------------------------------------------------
  case WM_COMMAND:

    switch (LOWORD(wParam)) {
    // ==========================================
    case 1: { // Open

      char file_path[MAX_PATH] = {0};
      OPENFILENAMEA ofn = {0};

      RECT client_rect;
      GetClientRect(hwnd, &client_rect);

      ofn.lStructSize = sizeof(ofn);
      ofn.hwndOwner = hwnd;
      ofn.lpstrFile = file_path;
      ofn.nMaxFile = MAX_PATH;
      ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST |
                  OFN_HIDEREADONLY;
      ofn.lpstrTitle = "Wybierz nowy obraz: ";

      if (GetOpenFileNameA(&ofn)) {

        free_image(&app->current_image);
        free_image(&app->original_image);
        set_current_image_path(app, file_path);
        if (load_image(&app->current_image, app->current_image_path)) {
          app->flags.IMAGE_LOADED = true;
          enable_export(app); // state mashine hmm może nie.
          ShowWindow(app->UI_handles.hSidebar,
                     SW_SHOW); // maybe out of here later if i made a checker
          load_image(&app->original_image, app->current_image_path);
          InvalidateRect(hwnd, &client_rect, TRUE);
        }
      }

    } break;
    // =========EXPORT=DROPDOWN=================================
    case 21: { // Export PNG
      if (show_save_dialog(hwnd, app->export_file_path))
        export_image(app->export_file_path, &app->current_image, PNG);
    } break;
    case 22: { // Export JPNG
      if (show_save_dialog(hwnd, app->export_file_path))
        export_image(app->export_file_path, &app->current_image, JPG);
    } break;
    case 23: { // Export BMP
      if (show_save_dialog(hwnd, app->export_file_path))
        export_image(app->export_file_path, &app->current_image, BMP);
    } break;
    case 24: { // Export TGA
      if (show_save_dialog(hwnd, app->export_file_path))
        export_image(app->export_file_path, &app->current_image, TGA);
    } break;
    case 25: { // Export HDR
      if (show_save_dialog(hwnd, app->export_file_path))
        export_image(app->export_file_path, &app->current_image, HDR);
    } break;
    // ==========================================
    case 3: // Exit
      PostMessage(hwnd, WM_CLOSE, 0, 0);
      break;
    // ==========================================
    case 4: { // Undo
    } break;
    // ==========================================
    case 5: { // Redo
    } break;
    // ==========================================
    case 6: { // About
    } break;
      // -------------------EFFECTS-DROPDOWN--------------------
    // case 100: {
    //   if (HIWORD(wParam) == CBN_SELCHANGE) {
    //     HWND hComboBox = (HWND)lParam;
    //     int sel = (int)SendMessage(hComboBox, CB_GETCURSEL, 0, 0);

    //     // React to selection
    //     switch (sel) {
    //     case 0:
    //       // apply_blur(&app->current_image);
    //       break;
    //     case 1:
    //       // apply_sharpen(&app->current_image);
    //       break;
    //     case 2:
    //       // apply_sepia(&app->current_image);
    //       break;
    //     case 3:
    //       apply_amplify(&app->current_image);
    //       break;
    //     case 4:
    //       apply_negative(&app->current_image);
    //       break;
    //     case 5:
    //       apply_monochrome(&app->current_image);
    //       break;
    //     }

    //     InvalidateRect(hwnd, NULL, TRUE); // force redraw
    //   }
    // } break;
    }
    // ==========================================
  case WM_SIZE: {
    InvalidateRect(hwnd, NULL, FALSE);
    break;
  }
  // ----------------------------------------------------
  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    Image *img = &app->current_image; // For preview
    if (app->flags.PREVIEW_ORIGINAL)
      img = &app->original_image;

    if (!app->flags.IMAGE_LOADED) {
      FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
      EndPaint(hwnd, &ps);
      break;
    }

    RECT client_rect;
    GetClientRect(hwnd, &client_rect);

    app->UI_handles.display_buffer.frame_bitmap_info.bmiHeader.biSize =
        sizeof(BITMAPINFOHEADER);
    app->UI_handles.display_buffer.frame_bitmap_info.bmiHeader.biWidth =
        app->current_image.width;
    app->UI_handles.display_buffer.frame_bitmap_info.bmiHeader.biHeight =
        -app->current_image.height;
    app->UI_handles.display_buffer.frame_bitmap_info.bmiHeader.biPlanes = 1;
    app->UI_handles.display_buffer.frame_bitmap_info.bmiHeader.biBitCount = 32;
    app->UI_handles.display_buffer.frame_bitmap_info.bmiHeader.biCompression =
        BI_RGB;

    FillRect(hdc, &client_rect, (HBRUSH)(COLOR_WINDOW + 1));

    SetDIBitsToDevice(
        hdc, 0, 0, //
        img->width, img->height, 0, 0, 0, img->height, img->pixels,
        &app->UI_handles.display_buffer.frame_bitmap_info, DIB_RGB_COLORS);

  } break;
  // ----------------------------------------------------
  default: {
    return DefWindowProc(hwnd, message, wParam, lParam);
  }
    // ----------------------------------------------------
  }
  return 0;
}

LRESULT CALLBACK PanelProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                           UINT_PTR wp, DWORD_PTR lp) {
  State *app = (State *)lp;

  switch (msg) {
  // case WM_CREATE: {
  //   CREATESTRUCT *cs = (CREATESTRUCT *)lParam;
  //   app = (State *)cs->lpCreateParams;
  //   SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)app);
  //   return 0;
  // }
  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    RECT rc;
    GetClientRect(hwnd, &rc);

    // Fill with light gray
    HBRUSH hBrush = CreateSolidBrush(RGB(230, 230, 230));
    FillRect(hdc, &rc, hBrush);
    DeleteObject(hBrush);

    EndPaint(hwnd, &ps);
    return 0;
  }
  case WM_NCDESTROY:
    RemoveWindowSubclass(hwnd, PanelProc, wp);
    break;
  // In side panel WndProc:
  case WM_COMMAND: {
    // -------------------EFFECTS-DROPDOWN--------------------
    switch (LOWORD(wParam)) {
    case 100: {
      if (HIWORD(wParam) == CBN_SELCHANGE) {
        HWND hComboBox = (HWND)lParam;
        int sel = (int)SendMessage(hComboBox, CB_GETCURSEL, 0, 0);

        // React to selection
        switch (sel) {
        case 0:
          // apply_blur(&app->current_image);
          break;
        case 1:
          // apply_sharpen(&app->current_image);
          break;
        case 2:
          // apply_sepia(&app->current_image);
          break;
        case 3:
          apply_amplify(&app->current_image);
          break;
        case 4:
          apply_negative(&app->current_image);
          break;
        case 5:
          apply_monochrome(&app->current_image);
          break;
        }

        InvalidateRect(app->UI_handles.hwnd_main, NULL, TRUE); // force redraw
      }
      break;
    }
    // case rn SendMessage(GetParent(hwnd), msg, wParam, lParam);
    default:
      app = (State *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
      break;
    }
  }
  }
  return DefSubclassProc(hwnd, msg, wParam, lParam);
}
