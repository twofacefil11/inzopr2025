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

  RECT rc;
  GetClientRect(hwnd, &rc);

  /// niezbędne
  HINSTANCE hInstance = GetModuleHandle(NULL);
  /// dzięki temu ui wygląda nowocześniej
  INITCOMMONCONTROLSEX icex = {sizeof(icex), ICC_WIN95_CLASSES};
  InitCommonControlsEx(&icex);

  // Drag and drop
  DragAcceptFiles(hwnd, TRUE);

  ui->display_buffer =
      make_display_buffer(hwnd); // możliwe że zniknie wiele z tego TODO

  ui->hComboBgBrush = CreateSolidBrush(RGB(30, 30, 30));

  ui->hDnd = CreateWindowExW(0, L"STATIC", L"Drag and drop an image to edit it.",
                                       WS_CHILD | WS_VISIBLE | DT_CENTER, (rc.right / 2) - 100, (rc.bottom / 2) - 10, 200, 20,
                                       hwnd, NULL, hInstance, NULL);


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

  ui->filter_controls.hMonochrome =
      CreateWindowEx(0, L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SW_HIDE, 10,
                     50, 180, 380, ui->hSidebar, NULL, hInstance, NULL);

  HWND hSliderMonochromeRed;
  HWND hSliderMonochromeBlue;
  HWND hSliderMonochromeGreen;

  {
    HWND hLabel1, hLabel2, hLabel3;

    hLabel1 = CreateWindowEx(0, L"STATIC", L"Red:", WS_CHILD | WS_VISIBLE, 10,
                             10, 80, 20, ui->filter_controls.hMonochrome, NULL,
                             hInstance, NULL);

    hSliderMonochromeRed = CreateWindowEx(
        0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS, 10, 35,
        160, 30, ui->filter_controls.hMonochrome, NULL, hInstance, NULL);

    hLabel2 = CreateWindowEx(0, L"STATIC", L"Green:", WS_CHILD | WS_VISIBLE, 10,
                             70, 80, 20, ui->filter_controls.hMonochrome, NULL,
                             hInstance, NULL);

    hSliderMonochromeGreen = CreateWindowEx(
        0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS, 10, 95,
        160, 30, ui->filter_controls.hMonochrome, NULL, hInstance, NULL);

    hLabel3 = CreateWindowEx(0, L"STATIC", L"Blue:", WS_CHILD | WS_VISIBLE, 10,
                             135, 80, 20, ui->filter_controls.hMonochrome, NULL,
                             hInstance, NULL);

    hSliderMonochromeBlue = CreateWindowEx(
        0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS, 10, 155,
        160, 30, ui->filter_controls.hMonochrome, NULL, hInstance, NULL);

    // ustaw czcionki labelom
    SendMessage(hLabel1, WM_SETFONT, (WPARAM)ui->hFont, TRUE);
    SendMessage(hLabel2, WM_SETFONT, (WPARAM)ui->hFont, TRUE);
    SendMessage(hLabel3, WM_SETFONT, (WPARAM)ui->hFont, TRUE);
  }

  // Set slider range and position
  SendMessage(hSliderMonochromeRed, TBM_SETRANGE, TRUE, MAKELPARAM(-100, 100));
  SendMessage(hSliderMonochromeRed, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)0);
  SendMessage(hSliderMonochromeGreen, TBM_SETRANGE, TRUE,
              MAKELPARAM(-100, 100));
  SendMessage(hSliderMonochromeGreen, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)0);
  SendMessage(hSliderMonochromeBlue, TBM_SETRANGE, TRUE, MAKELPARAM(-100, 100));
  SendMessage(hSliderMonochromeBlue, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)0);

  /// -----------SHARPEN--------------------------------------------------

  ui->filter_controls.hSharpen =
      CreateWindowEx(0, L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SW_HIDE, 10,
                     50, 180, 380, ui->hSidebar, NULL, hInstance, NULL);

  HWND hSliderSharpenConst;
  HWND hLabelSharpenConst;

  hLabelSharpenConst =
      CreateWindowEx(0, L"STATIC", L"Mix:", WS_CHILD | WS_VISIBLE, 10, 10, 80,
                     20, ui->filter_controls.hSharpen, NULL, hInstance, NULL);

  SendMessage(hLabelSharpenConst, WM_SETFONT, (WPARAM)ui->hFont, TRUE);

  hSliderSharpenConst = CreateWindowEx(
      0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS, 10, 35,
      160, 30, ui->filter_controls.hSharpen, NULL, hInstance, NULL);

  SendMessage(hSliderSharpenConst, TBM_SETRANGE, TRUE, MAKELPARAM(1, 100));
  SendMessage(hSliderSharpenConst, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)5);

  HWND hRadio1 = CreateWindowEx(
      0, L"BUTTON", L"Sobel",
      WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP, 10, 70, 100, 20,
      ui->filter_controls.hSharpen, (HMENU)6671, hInstance, NULL);

  HWND hRadio2 = CreateWindowEx(
      0, L"BUTTON", L"Prewitt", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 10,
      95, 100, 20, ui->filter_controls.hSharpen, (HMENU)6672, hInstance, NULL);

  HWND hSharpenCheckBox = CreateWindowEx(
      0, L"BUTTON", L"Show edges",
      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 120, 150, 20,
      ui->filter_controls.hSharpen, (HMENU)6673, hInstance, NULL);

  SendMessage(hRadio1, WM_SETFONT, (WPARAM)ui->hFont, TRUE);
  SendMessage(hRadio2, WM_SETFONT, (WPARAM)ui->hFont, TRUE);
  SendMessage(hSharpenCheckBox, WM_SETFONT, (WPARAM)ui->hFont, TRUE);

  SendMessage(hRadio1, BM_SETCHECK, BST_CHECKED, 0); // first defoult
  CheckDlgButton(ui->filter_controls.hAmplify, 6673, BST_CHECKED);
  // id do identyfikacji

  /// -----------AMPLIFY--------------------------------------------------

  ui->filter_controls.hAmplify =
      CreateWindowEx(0, L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SW_HIDE, 10,
                     50, 180, 380, ui->hSidebar, NULL, hInstance, NULL);

  HWND hSliderAmplifyRed;
  HWND hSliderAmplifyBlue;
  HWND hSliderAmplifyGreen;

  {
    HWND hLabel1, hLabel2, hLabel3;

    hLabel1 =
        CreateWindowEx(0, L"STATIC", L"Red:", WS_CHILD | WS_VISIBLE, 10, 10, 80,
                       20, ui->filter_controls.hAmplify, NULL, hInstance, NULL);

    hSliderAmplifyRed = CreateWindowEx(
        0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS, 10, 35,
        160, 30, ui->filter_controls.hAmplify, NULL, hInstance, NULL);

    hLabel2 = CreateWindowEx(0, L"STATIC", L"Green:", WS_CHILD | WS_VISIBLE, 10,
                             70, 80, 20, ui->filter_controls.hAmplify, NULL,
                             hInstance, NULL);

    hSliderAmplifyGreen = CreateWindowEx(
        0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS, 10, 95,
        160, 30, ui->filter_controls.hAmplify, NULL, hInstance, NULL);

    hLabel3 = CreateWindowEx(0, L"STATIC", L"Blue:", WS_CHILD | WS_VISIBLE, 10,
                             135, 80, 20, ui->filter_controls.hAmplify, NULL,
                             hInstance, NULL);

    hSliderAmplifyBlue = CreateWindowEx(
        0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS, 10, 155,
        160, 30, ui->filter_controls.hAmplify, NULL, hInstance, NULL);

    HWND hCheckbox = CreateWindowEx(
        0, L"BUTTON", L"Clamp",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 190, 150, 20,
        ui->filter_controls.hAmplify, (HMENU)6667, hInstance, NULL);

    CheckDlgButton(ui->filter_controls.hAmplify, 6667, BST_CHECKED);

    HWND hCheckbox_og = CreateWindowEx(
        0, L"BUTTON", L"Original",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 370, 150, 20,
        ui->hSidebar, (HMENU)6668, hInstance, NULL);
    // CheckDlgButton(ui->filter_controls.hAmplify, 6667, BST_CHECKED);
    // ustaw czcionki labelom
    SendMessage(hLabel1, WM_SETFONT, (WPARAM)ui->hFont, TRUE);
    SendMessage(hLabel2, WM_SETFONT, (WPARAM)ui->hFont, TRUE);
    SendMessage(hLabel3, WM_SETFONT, (WPARAM)ui->hFont, TRUE);
    SendMessage(hCheckbox, WM_SETFONT, (WPARAM)ui->hFont, TRUE);
    SendMessage(hCheckbox_og, WM_SETFONT, (WPARAM)ui->hFont, TRUE);
  }

  // Set slider range and position
  SendMessage(hSliderAmplifyRed, TBM_SETRANGE, TRUE, MAKELPARAM(1, 20));
  SendMessage(hSliderAmplifyRed, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)0);
  SendMessage(hSliderAmplifyGreen, TBM_SETRANGE, TRUE, MAKELPARAM(1, 20));
  SendMessage(hSliderAmplifyGreen, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)0);
  SendMessage(hSliderAmplifyBlue, TBM_SETRANGE, TRUE, MAKELPARAM(1, 20));
  SendMessage(hSliderAmplifyBlue, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)0);

  // id do identyfikacji
  SetWindowLongPtr(hSliderAmplifyRed, GWLP_USERDATA, 6661);
  SetWindowLongPtr(hSliderAmplifyGreen, GWLP_USERDATA, 6662);
  SetWindowLongPtr(hSliderAmplifyBlue, GWLP_USERDATA, 6663);

  SetWindowLongPtr(hSliderMonochromeRed, GWLP_USERDATA, 6664);
  SetWindowLongPtr(hSliderMonochromeGreen, GWLP_USERDATA, 6665);
  SetWindowLongPtr(hSliderMonochromeBlue, GWLP_USERDATA, 6666);

  SetWindowLongPtr(hSliderSharpenConst, GWLP_USERDATA, 6611);

  /// -------------------------------------------------------------

  /// COMBOBOX
  ui->hComboBox = CreateWindowExW(
      0, L"COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 10, 10,
      160, 120, ui->hSidebar, (HMENU)100, hInstance, NULL);

  SendMessage(ui->hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Blur");
  SendMessage(ui->hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Sharpen");
  SendMessage(ui->hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Sepia");
  SendMessage(ui->hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Amplify");
  SendMessage(ui->hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Negative");
  SendMessage(ui->hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Monochrome");
  SendMessage(ui->hComboBox, SW_HIDE, 0, (LPARAM)L"io");
  SendMessage(ui->hComboBox, CB_SETCURSEL, 0, 0); // Select first item
  //
  SendMessage(ui->hComboBox, CB_SETCURSEL, (WPARAM)-1,
              0); // TODO: is that needed?
  SendMessage(ui->hDnd, WM_SETFONT, (WPARAM)ui->hFont, TRUE);

  SendMessage(ui->hComboBox, WM_SETFONT, (WPARAM)ui->hFont,
              MAKELPARAM(TRUE, 0));

  SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)ui->hIcon);

  // --------------------------ABOUT----------------------

  ui->hAbout = CreateWindowExW(WS_EX_TOOLWINDOW,
                               L"#32770", // built-in dialog class!
                               L"About", WS_POPUP | WS_CAPTION,
                               ((rc.right - rc.left) / 2) + rc.left - 120,
                               ((rc.top - rc.bottom) / 2) + rc.bottom - 70, 240,
                               140, ui->hwnd_main, NULL, hInstance, NULL);

  HWND hAboutText =
      CreateWindowW(L"STATIC", L"FilterLAB v0.2.whatever\n(c) 2025 iipeq",
                    WS_CHILD | WS_VISIBLE, 20, 20, 180, 40, ui->hAbout, NULL,
                    hInstance, NULL);

  HWND hAboutOK =
      CreateWindowW(L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                    95, 70, 50, 22, ui->hAbout, (HMENU)5555, hInstance, NULL);

  ShowWindow(ui->hAbout, SW_HIDE); // hide at init

  SendMessage(hAboutText, WM_SETFONT, (WPARAM)ui->hFont, MAKELPARAM(TRUE, 0));
  SendMessage(hAboutOK, WM_SETFONT, (WPARAM)ui->hFont, MAKELPARAM(TRUE, 0));

  // --------------------------ABOUT----------------------

  switch_controls(&ui->filter_controls, NULL);

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

  ShowWindow(ui->hDnd, SW_HIDE);
}

// ----------------------------------------------------------------

/// this is SO illegal
void switch_controls(const Filter_contorl_group_handles *handles,
                     const HWND *window_to_show) {

  const HWND *p = &handles->hBlur;
  for (int i = 0; i < FILTER_TYPES_COUNT; ++i)
    ShowWindow(p[i], (&p[i] != window_to_show) ? SW_HIDE : SW_SHOW);
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
