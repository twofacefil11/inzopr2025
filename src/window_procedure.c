// #include "Image.h"

#include "window_procedure.h"
#include "Filters.h"
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

      app->UI_handles.hSidebar = CreateWindowExW(
          0, L"STATIC", NULL,
          WS_CHILD | WS_BORDER, // debug visible
          10, 10, 200, 400, hwnd, (HMENU)200, NULL, (LPVOID)app);

      SetWindowSubclass(app->UI_handles.hSidebar, PanelProc, 0, (DWORD_PTR)app);

      init_UI(hwnd, &app->UI_handles);

      SetWindowSubclass(app->UI_handles.filter_controls.hMonochrome, PanelProc,
                        1, (DWORD_PTR)app);
      SetWindowSubclass(app->UI_handles.filter_controls.hAmplify, PanelProc, 1,
                        (DWORD_PTR)app);
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

    case WM_DROPFILES: {

      RECT client_rect;
      GetClientRect(hwnd, &client_rect);

      HDROP hDrop = (HDROP)wParam;

      char file_path[MAX_PATH];

      if (DragQueryFileA(hDrop, 0, file_path, MAX_PATH)) {
        // if (app->flags.IMAGE_LOADED) {
        //   // TODO ASK CZY NA PEWNO
        // }

        load_image(&app->current_image, file_path); // DEBUG
        load_image(&app->original_image, file_path);

        app->flags.IMAGE_LOADED = true;
        enable_export(&app->UI_handles); // state mashine hmm może nie.
        ShowWindow(app->UI_handles.hSidebar, SW_SHOW);
        // maybe out of here later if i made a checkerS
        InvalidateRect(app->UI_handles.hwnd_main, &client_rect, TRUE);
      }
      DragFinish(hDrop);

      break;
    }
    // ----------------------------------------------------
    case WM_QUIT: {
      // TODO
      break;
    }

      // ----------------------------------------------------

    case WM_MOUSEWHEEL: {
      int delta = GET_WHEEL_DELTA_WPARAM(wParam); // +120 or -120 per przedział
      short zDelta = (short)HIWORD(wParam);       // same here
      if (delta > 0) {
        app->zoom *= 1.01f; // zoom in ?
      } else {
        app->zoom /= 1.01f; // zoom out ?
      }

      InvalidateRect(hwnd, NULL, TRUE); // trigger repaint
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
          ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST |
                      OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
          ofn.lpstrTitle = "Wybierz nowy obraz: ";

          if (GetOpenFileNameA(&ofn)) {

            free_image(&app->current_image);
            free_image(&app->original_image);
            set_current_image_path(app, file_path);
            if (load_image(&app->current_image, app->current_image_path)) {
              app->flags.IMAGE_LOADED = true;
              enable_export(&app->UI_handles); // state mashine hmm może nie.
              ShowWindow(app->UI_handles.hSidebar, SW_SHOW);
              // maybe out of here later if i made a checker
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

      // fprintf(stderr, "paint");

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
      app->UI_handles.display_buffer.frame_bitmap_info.bmiHeader.biBitCount =
          32;
      app->UI_handles.display_buffer.frame_bitmap_info.bmiHeader.biCompression =
          BI_RGB;

      HDC backDC = CreateCompatibleDC(hdc); // ← offscreen memory DC
      HBITMAP backBmp = CreateCompatibleBitmap(
          hdc, client_rect.right, client_rect.bottom); // ← offscreen bitmap
      HBITMAP oldBmp = SelectObject(backDC, backBmp);  // ← attach bitmap to DC
                                                       //
      // --- CLEAR BACKBUFFER ---
      HBRUSH black = (HBRUSH)GetStockObject(BLACK_BRUSH);
      FillRect(backDC, &client_rect, black);

      int scaled_width = (int)(img->width * app->zoom);
      int scaled_height = (int)(img->height * app->zoom);

      int target_x = (client_rect.right - scaled_width) / 2;
      int target_y = (client_rect.bottom - scaled_height) / 2;

      SetStretchBltMode(backDC, HALFTONE);
      SetBrushOrgEx(backDC, 0, 0, NULL); //
      // te dwa dsą potrzebne do ładniejszego samplowania przyzoomie
      
      /// STARY stretch, bez doublebuffering
      // StretchDIBits(hdc, target_x, target_y, // Destination X, Y on screen
      //               scaled_width, scaled_height, 0, 0, img->width,
      //               img->height, // Source width & height
      //               img->pixels,
      //               &app->UI_handles.display_buffer.frame_bitmap_info,
      //               DIB_RGB_COLORS,
      //               SRCCOPY // Raster operation
      // );

      StretchDIBits(backDC,       
                    target_x, target_y,
                    scaled_width, scaled_height,
                    0, 0,                       
                    img->width, img->height,   
                    img->pixels,
                    &app->UI_handles.display_buffer.frame_bitmap_info,
                    DIB_RGB_COLORS, SRCCOPY);

      BitBlt(hdc, 0, 0, client_rect.right, client_rect.bottom, backDC, 0, 0,
             SRCCOPY);

      SelectObject(backDC, oldBmp);
      DeleteObject(backBmp);
      DeleteDC(backDC);

      EndPaint(hwnd, &ps);
    } break;
    // ----------------------------------------------------
    default: {
      return DefWindowProc(hwnd, message, wParam, lParam);
    }
    case WM_USER + 123: {
      InvalidateRect(hwnd, NULL, TRUE);
      return 0;
    }
      // ----------------------------------------------------
  }

  return 0;
}

// ----------------------------------------------------------------

LRESULT CALLBACK PanelProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                           UINT_PTR wp, DWORD_PTR lp) {
  State *app = (State *)lp;

  HWND wm = GetParent(hwnd);

  switch (msg) {

    case WM_CREATE: {
      CREATESTRUCT *cs = (CREATESTRUCT *)lParam;
      app = (State *)cs->lpCreateParams;
      SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)app);
      return 0;
    }
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
    case WM_HSCROLL: {

      HWND hSlider = (HWND)lParam;

      if ((HWND)lParam != NULL) {
        // if (TB_ENDTRACK == LOWORD(wParam)) { // NO

        int pos = (int)SendMessage(hSlider, TBM_GETPOS, 0, 0);
        LONG_PTR id = GetWindowLongPtr(hSlider, GWLP_USERDATA);

        switch (id) {
          case 6661:
            app->filter_params.amplify_r = pos;
            apply_amplify(&app->original_image, &app->current_image,
                          &app->filter_params,
                          app->filter_params.clamp_amplify);
            break;
          case 6662:
            app->filter_params.amplify_g = pos;
            apply_amplify(&app->original_image, &app->current_image,
                          &app->filter_params,
                          app->filter_params.clamp_amplify);
            break;
          case 6663:
            app->filter_params.amplify_b = pos;
            apply_amplify(&app->original_image, &app->current_image,
                          &app->filter_params,
                          app->filter_params.clamp_amplify);
            break;

            // MONOCHROMES
          case 6664:
            app->filter_params.mono_r = pos;
            apply_monochrome(&app->original_image, &app->current_image,
                             &app->filter_params);
            break;
          case 6665:
            app->filter_params.mono_g = pos;
            apply_monochrome(&app->original_image, &app->current_image,
                             &app->filter_params);
            break;
          case 6666:
            app->filter_params.mono_b = pos;
            apply_monochrome(&app->original_image, &app->current_image,
                             &app->filter_params);
            break;
        }

        InvalidateRect(app->UI_handles.hwnd_main, NULL, FALSE);
      }
      // InvalidateRect(((UI*)lp)->hwnd_main, NULL      }
      // fprintf(stderr, "before invalidate\n");
      // InvalidateRect(app->UI_handles.hwnd_main, NULL, FALSE);
      // UpdateWindow(app->UI_handles.hwnd_main);
      // PostMessage(((UI *)lp)->hwnd_main, WM_COMMAND,
      //             MAKEWPARAM(100, CBN_SELCHANGE), (LPARAM)app);
    } break;
    case WM_NCDESTROY:
      RemoveWindowSubclass(hwnd, PanelProc, wp);
      break;
      // In side panel WndProc:

    case WM_COMMAND: {

      // -------------------EFFECTS-DROPDOWN--------------------

      switch (LOWORD(wParam)) {
        case 6667:
          app->filter_params.clamp_amplify = IsDlgButtonChecked(hwnd, 6667);

          apply_amplify(&app->original_image, &app->current_image,
                        &app->filter_params, app->filter_params.clamp_amplify);

          InvalidateRect(app->UI_handles.hwnd_main, NULL, TRUE);
          break;

        case 6668:
          app->flags.PREVIEW_ORIGINAL = IsDlgButtonChecked(hwnd, 6668);
          InvalidateRect(app->UI_handles.hwnd_main, NULL, TRUE);
          break;

        case 100: {
          if (HIWORD(wParam) == CBN_SELCHANGE) {
            HWND hComboBox = (HWND)lParam;
            int sel = (int)SendMessage(hComboBox, CB_GETCURSEL, 0, 0);

            // React to selection
            /// SHOW FILTER CONTROLS INSTNIEJHE NIE WIME  CZY POWINNO
            switch (sel) {

              case 0:
                apply_blur(&app->original_image, &app->current_image,
                           &app->filter_params);
                switch_controls(&app->UI_handles.filter_controls,
                                &app->UI_handles.filter_controls.hBlur);
                break;

              case 1:
                // apply_sharpen(&app->current_image);
                switch_controls(&app->UI_handles.filter_controls,
                                &app->UI_handles.filter_controls.hSharpen);
                break;

              case 2:
                apply_sepia(&app->original_image, &app->current_image);
                switch_controls(&app->UI_handles.filter_controls,
                                &app->UI_handles.filter_controls.hSepia);
                break;

              case 3:
                apply_amplify(&app->original_image, &app->current_image,
                              &app->filter_params,
                              app->filter_params.clamp_amplify);
                switch_controls(&app->UI_handles.filter_controls,
                                &app->UI_handles.filter_controls.hAmplify);
                break;

              case 4:
                apply_negative(&app->original_image, &app->current_image);
                switch_controls(&app->UI_handles.filter_controls,
                                &app->UI_handles.filter_controls.hNegative);
                break;

              case 5:
                apply_monochrome(&app->original_image, &app->current_image,
                                 &app->filter_params);
                switch_controls(&app->UI_handles.filter_controls,
                                &app->UI_handles.filter_controls.hMonochrome);
                break;
            }
            InvalidateRect(app->UI_handles.hwnd_main, NULL, TRUE);

            // if (app->UI_handles.hwnd_main == NULL)
            //   fprintf(stderr, "\ndupa\n");
          }
          break;
        }

        // SendMessage(GetParent(hwnd), msg, wParam, lParam);
        default:
          app = (State *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
          break;
      }
    }
  }
  return DefSubclassProc(hwnd, msg, wParam, lParam);
}
