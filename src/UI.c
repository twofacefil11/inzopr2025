#include "UI.h"
//-------------------------------------------------------------------------------

/// This should resize lazily. empty, but sized on startup
/// filled with a cutout when image loaded.

/// NOTE: realocate on resize,
DisplayBuffer make_display_buffer(HWND hwnd) {

  RECT client_rect;
  GetClientRect(hwnd, &client_rect);

  DisplayBuffer db = {0};

  // helpers below, might concat
  db.width = client_rect.right - client_rect.left;
  db.height = client_rect.bottom - client_rect.top;

  ZeroMemory(&db.frame_bitmap_info, sizeof(BITMAPINFO));
  return db;
}
//-------------------------------------------------------------------------------

/// NOTE: F: helper to tidy main.
Window_initial_position getWindowInfo() {
  Window_initial_position wip;
  wip.system_width = GetSystemMetrics(SM_CXSCREEN);
  wip.system_height = GetSystemMetrics(SM_CYSCREEN);
  wip.width = wip.system_width - (wip.system_width / 4);
  wip.height = wip.system_height - (wip.system_height / 4);
  wip.x = (wip.system_width - wip.width) / 2;
  wip.y = (wip.system_height - wip.height) / 2;
  return wip;
}

/// This might have to be here since it might know what stasate is
int init_UI(HWND hwnd, UI *ui) {

  /// niezbędne
  HINSTANCE hInstance = GetModuleHandle(NULL);
  /// dzięki temu ui wygląda nowocześniej
  INITCOMMONCONTROLSEX icex = {sizeof(icex), ICC_WIN95_CLASSES};
  InitCommonControlsEx(&icex);

  ui->display_buffer =
      make_display_buffer(hwnd); // możliwe że zniknie wiele z tego TODO
  ui->hComboBgBrush = CreateSolidBrush(RGB(30, 30, 30));
  /// MENU
  HMENU hMenubar = CreateMenu();
  HMENU hFile = CreateMenu();
  HMENU hEdit = CreateMenu();
  HMENU hInfo = CreateMenu();
  HMENU hExport = CreateMenu();

  AppendMenu(hFile, MF_STRING, 1, L"Open...");
  AppendMenu(hFile, MF_POPUP | MF_GRAYED, (UINT_PTR)hExport, L"Export");
  {
    AppendMenu(hExport, MF_STRING | MF_GRAYED, 21, L"PNG");
    AppendMenu(hExport, MF_STRING | MF_GRAYED, 22, L"JPG");
    AppendMenu(hExport, MF_STRING | MF_GRAYED, 23, L"BMP");
    AppendMenu(hExport, MF_STRING | MF_GRAYED, 24, L"TGA");
    AppendMenu(hExport, MF_STRING | MF_GRAYED, 25, L"HDR");
  }
  AppendMenu(hFile, MF_SEPARATOR, 0, NULL);
  AppendMenu(hFile, MF_STRING, 3, L"Exit");

  AppendMenu(hEdit, MF_STRING | MF_GRAYED, 4, L"Undo");
  AppendMenu(hEdit, MF_STRING | MF_GRAYED, 5, L"Redo");

  AppendMenu(hInfo, MF_STRING, 6, L"About");

  AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hFile, L"File");
  AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hEdit, L"Edit");
  AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hInfo, L"Info");

  SetMenu(hwnd, hMenubar);
  //-0------------------FONT-----------------------
  ui->hFont =
      CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                 DEFAULT_PITCH | FF_SWISS, L"Segoe UI");

  //-----------------------------------------------
  // ustawienie czcionki dla combobox
  ui->hMenubar = hMenubar;
  ui->hExportMenu = hExport;

  /// -----------MONO--------------------------------------------------
  /// R
  ui->filter_controls.hMonochrome = CreateWindowEx(
      0, L"STATIC", NULL,
      WS_CHILD | WS_VISIBLE, // Add WS_VISIBLE only if showing by default
      10, 50, 180, 380, ui->hSidebar, NULL, hInstance, NULL);

  HWND hSliderMonochromeRed = CreateWindowEx(
      0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS, 10, 35,
      160, 30, ui->filter_controls.hMonochrome, NULL, hInstance, NULL);
  HWND hLabel1 = CreateWindowEx(0, L"STATIC", L"Red:", WS_CHILD | WS_VISIBLE,
                                10, 10, 80, 20, ui->filter_controls.hMonochrome,
                                NULL, hInstance, NULL);
  // G
  HWND hSliderMonochromeGreen = CreateWindowEx(
      0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS, 10, 95,
      160, 30, ui->filter_controls.hMonochrome, NULL, hInstance, NULL);

  HWND hLabel2 = CreateWindowEx(0, L"STATIC", L"Green:", WS_CHILD | WS_VISIBLE,
                                10, 70, 80, 20, ui->filter_controls.hMonochrome,
                                NULL, hInstance, NULL);
  // B
  HWND hSliderMonochromeBlue = CreateWindowEx(
      0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS, 10, 155,
      160, 30, ui->filter_controls.hMonochrome, NULL, hInstance, NULL);

  HWND hLabel3 = CreateWindowEx(
      0, L"STATIC", L"Blue:", WS_CHILD | WS_VISIBLE, 10, 135, 80, 20,
      ui->filter_controls.hMonochrome, NULL, hInstance, NULL);

  // Set slider range and position
  SendMessage(hSliderMonochromeRed, TBM_SETRANGE, TRUE, MAKELPARAM(-100, 100));
  SendMessage(hSliderMonochromeRed, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)0);
  SendMessage(hSliderMonochromeGreen, TBM_SETRANGE, TRUE,
              MAKELPARAM(-100, 100));
  SendMessage(hSliderMonochromeGreen, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)0);
  SendMessage(hSliderMonochromeBlue, TBM_SETRANGE, TRUE, MAKELPARAM(-100, 100));
  SendMessage(hSliderMonochromeBlue, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)0);
  // ustaw czcionki labelom
  SendMessage(hLabel1, WM_SETFONT, (WPARAM)ui->hFont, TRUE);
  SendMessage(hLabel2, WM_SETFONT, (WPARAM)ui->hFont, TRUE);
  SendMessage(hLabel3, WM_SETFONT, (WPARAM)ui->hFont, TRUE);

  /// -------------------------------------------------------------

  /// SIDEBAR
  ///  TEST to należey dać do init_UI oraz przemyśleć tego miejsce w state.
  // hSidebar =
  //     CreateWindowEx(0,
  //                    "STATIC", // Class name (built-in for static controls)
  //                    NULL,     // No text
  //                    WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_WHITEFRAME, 0, 0,
  //                    200, 500,  // x, y, width, height
  //                    hwnd,      // Parent window
  //                    (HMENU)10, // ID
  //                    ((LPCREATESTRUCT)lParam)->hInstance, NULL);

  /// SIDEBAR WOULD Be  NICE HERE BUT IT CANT;

  /// COMBOBOX
  HWND hComboBox = CreateWindowExW(
      0, L"COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 10, 10,
      160, 120, ui->hSidebar, (HMENU)100, hInstance, NULL);

  SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Blur");
  SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Sharpen");
  SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Sepia");
  SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Amplify");
  SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Negative");
  SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Monochrome");
  SendMessage(hComboBox, CB_SETCURSEL, 0, 0); // Select first item

  SendMessage(hComboBox, WM_SETFONT, (WPARAM)ui->hFont, MAKELPARAM(TRUE, 0));

  return 0;
}

int show_save_dialog(HWND hwnd, char *out_path) {
  char file_path[MAX_PATH] = {0};
  char initial_dir[MAX_PATH] = {0};

  OPENFILENAMEA ofn = {0};

  ExpandEnvironmentStringsA("%USERPROFILE%\\Documents", initial_dir, MAX_PATH);

  // Use default filename
  strncpy(file_path, "output", MAX_PATH - 1);

  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = hwnd;
  ofn.lpstrFile = file_path;
  ofn.nMaxFile = MAX_PATH;
  ofn.lpstrTitle = "Zapisz jako: ";
  ofn.lpstrFilter =
      "PNG Files\0*.png\0JPEG Files\0*.jpg;*.jpeg\0BMP Files\0*.bmp\0TGA "
      "Files\0*.tga\0HDR Files\0*.hdr\0All Files\0*.*\0";
  ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
  // ofn.lpstrFilter = "PNG Files\0*.png\0JPEG Files\0*.jpg;*.jpeg\0BMP "
  // "Files\0*.bmp\0All Files\0*.*\0";

  if (GetSaveFileNameA(&ofn)) {
    strncpy(out_path, file_path, MAX_PATH);
    return 1;
  }

  return 0;
}

// ----------------------------------------------------------------

void enable_export(UI *ui) {
  EnableMenuItem(ui->hMenubar, (UINT_PTR)ui->hExportMenu, MF_ENABLED);
  EnableMenuItem(ui->hExportMenu, 21, MF_ENABLED);
  EnableMenuItem(ui->hExportMenu, 22, MF_ENABLED);
  EnableMenuItem(ui->hExportMenu, 23, MF_ENABLED);
  EnableMenuItem(ui->hExportMenu, 24, MF_ENABLED);
  EnableMenuItem(ui->hExportMenu, 25, MF_ENABLED);
}

// ----------------------------------------------------------------

/// this is SO illegal and unnecessary
void switch_controls(const Filter_contorl_group_handles *handles,
                    const HWND window_to_show) {

  const HWND *p = &handles->hBlur;
  for (int i = 0; i < FILTER_TYPES_COUNT; ++i)
    ShowWindow(p[i], (p[i] != window_to_show) ? SW_HIDE : SW_SHOW);
  //  (/¯◡ ‿ ◡) /¯
}

// ----------------------------------------------------------------

// void show_filter_controls(HWND filter_control_handle, Filter_type filter) {
//   switch (filter) {
//   case BLUR:
//         // ShowWindow(HWND hWnd, int nCmdShow)
//         break;
//   case SHARPEN:
//     // ShowWindow(HWND hWnd, int nCmdShow)
//     break;
//   case SEPIA:
//     // ShowWindow(HWND hWnd, int nCmdShow)
//     break;
//   case AMPLIFY:
//     ShowWindow(filter_control_handle, SW_SHOW);
//     // ShowWindow(HWND hWnd, int nCmdShow)
//     break;
//   case NEGATIVE:
//     // ShowWindow(HWND hWnd, int nCmdShow)
//     break;
//   case BW:
//     // ShowWindow(HWND hWnd, int nCmdShow)
//     break;
//   case NO_FILTER:
//     // ShowWindow(HWND hWnd, int nCmdShow)
//     break;
//   }
//   return;
// }
