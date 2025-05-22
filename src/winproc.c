#include "winproc.h"


LRESULT CALLBACK WindowProcessMessage(HWND hwnd, UINT message, WPARAM wParam,
                                      LPARAM lParam) {

  AppState *app = (AppState *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

  switch (message) {

  case WM_QUIT: // jak jest puste to windows robi default
                // case WM_COMMAND: // TODO: tutaj będą sygnały z UI
  case WM_CREATE: {
    CREATESTRUCT *cs = (CREATESTRUCT *)lParam;
    app = (AppState *)cs->lpCreateParams;
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)app);

    // Create UI controls here now that hwnd_main is known:
    HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);

    app->hwnd_combobox = CreateWindowEx(
        0, L"COMBOBOX", NULL, CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE, 10, 10,
        150, 100, hwnd, (HMENU)ID_COMBOBOX, hInstance, NULL);

    SendMessage(app->hwnd_combobox, CB_ADDSTRING, 0, (LPARAM)L"Negative");
    SendMessage(app->hwnd_combobox, CB_ADDSTRING, 0, (LPARAM)L"Grayscale");
    SendMessage(app->hwnd_combobox, CB_ADDSTRING, 0, (LPARAM)L"Brightness");
    SendMessage(app->hwnd_combobox, CB_SETCURSEL, 0, 0);

    app->hwnd_sidebar = CreateWindowEx(
        0, L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_NOTIFY | WS_BORDER, 0, 0,
        200, 100, hwnd, NULL, hInstance, NULL);

    break;
  }
    // TODO:inicjalizacja żeczy po stworzeniu
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
    app->shouldQuit = true;
    PostQuitMessage(0);
    break; // kunic
  }
  case WM_COMMAND: { /// TODO
    int control_id = LOWORD(wParam);
    int event = HIWORD(wParam);

    switch (control_id) {
    case 1:
      OpenFileDialog(app->hwnd_main, app);
    }
    // if (control_id == 10 && event == CBN_SELCHANGE) {
    //   int selected = SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);
    //   // show/hide input fields depending on `selected`
    // }
  } break;

  case WM_PAINT: {
    static PAINTSTRUCT paint;
    static HDC device_context;
    device_context = BeginPaint(hwnd, &paint);

    // blit_to_frame(app->image_data, app->frame);// to będzie tu musiało być eventually 
    // blit_to_frame(app->image_data , &frame);

    // BitBlt to całkiem oldschoolowa nazwa look it up, its cool
    BitBlt(device_context, paint.rcPaint.left, paint.rcPaint.top,
           paint.rcPaint.right - paint.rcPaint.left,
           paint.rcPaint.bottom - paint.rcPaint.top,
           app->frame->frame_device_context, paint.rcPaint.left,
           paint.rcPaint.top, SRCCOPY);

    EndPaint(hwnd, &paint);
  } break;

  // resize msg
  case WM_SIZE: {
    // TODO: centerowanie and all
    app->frame->width = LOWORD(lParam);
    app->frame->height = HIWORD(lParam);

    app->frame->frame_bitmap_info.bmiHeader.biWidth = app->frame->width;
    app->frame->frame_bitmap_info.bmiHeader.biHeight = app->frame->height;

    if (app->frame->frame_bitmap)
      DeleteObject(app->frame->frame_bitmap);

    app->frame->frame_bitmap =
        CreateDIBSection(NULL, &app->frame->frame_bitmap_info, DIB_RGB_COLORS,
                         (void **)&app->frame->pixels, 0, 0);

    SelectObject(app->frame->frame_device_context, app->frame->frame_bitmap);

    // Te dwa redrawują. One does not call wm_print by hand.
    InvalidateRect(hwnd, NULL, FALSE);
    UpdateWindow(hwnd);

  } break; // DONT // ASK

  default: {
    return DefWindowProc(hwnd, message, wParam, lParam);
  }
  }
  return 0;
}
