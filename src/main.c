
// ----------------------------------------------------------------------------
// main.c
// Responsibilities:
// boilerplate...
// - state initialization();
// - main window class/register and create + check
// while {
//   msg processing. ONLY
// }
// ----------------------------------------------------------------------------

#define UNICODE
#define _UNICODE

#define MAIN_WINDOW_CLASS_NAME L"Main_window"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <windows.h>

#include "State.h"

// ----------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine,
                   int nCmdShow) {

  State *appState = init_state(hInstance);
  if (!appState)
    return -1; // ERR

  //icon
  appState->UI_handles.hIcon = (HICON)LoadImage(NULL, L"FilterLAB.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED);

  register_class(appState, hInstance, MAIN_WINDOW_CLASS_NAME);

  Window_initial_position wip = getWindowInfo();

  appState->UI_handles.hwnd_main = CreateWindowW(
      MAIN_WINDOW_CLASS_NAME, (LPCWSTR)("FilterLAB"),
      WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN, wip.x, wip.y, wip.width, wip.height,
      NULL, NULL, hInstance, appState);

  if (!appState->UI_handles.hwnd_main) {
    return -1; // ERR
  }

  // ----------------------------------------------------------------

  MSG message;
  while (GetMessage(&message, NULL, 0, 0)) {
    TranslateMessage(&message);
    DispatchMessage(&message);
  }

  // ----------------------------------------------------------------
  return 0;
}
