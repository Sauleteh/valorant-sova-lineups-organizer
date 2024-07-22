#include <windows.h>
#include <stdio.h>

HHOOK hKeyHook;
DWORD vkCode = 0;

// Función que maneja las pulsaciones de teclas
//! Función interna
LRESULT CALLBACK KeyEvent(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
        if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) && vkCode == 0) {
            vkCode = p->vkCode;
            PostQuitMessage(0);
        }
    }
    return CallNextHookEx(hKeyHook, nCode, wParam, lParam);
}

//! Función interna
static void SetHook() {
    hKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyEvent, GetModuleHandle(NULL), 0);
    if (hKeyHook == NULL) printf("Hook installation failed!\n");
}

//! Función interna
void ReleaseHook() {
    UnhookWindowsHookEx(hKeyHook);
}

DWORD detectKeyPress() {
    MSG msg;
    SetHook();
    
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (vkCode != 0) break;
    }
    
    ReleaseHook();
    return vkCode;
}