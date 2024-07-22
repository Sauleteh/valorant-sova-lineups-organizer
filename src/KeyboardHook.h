#include <windows.h>
#include <stdio.h>

HHOOK hKeyHook;
DWORD vkCode = 0;
UINT8 vkToCompare = 0;

// Función que maneja las pulsaciones de teclas
//! Función interna
LRESULT CALLBACK KeyEvent(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
        if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) && ((vkCode == 0 && vkToCompare == 0) || (p->vkCode == vkToCompare && vkToCompare != 0))) {
            vkCode = p->vkCode;
            PostQuitMessage(0);
        }
            
    }
    return CallNextHookEx(hKeyHook, nCode, wParam, lParam);
}

//! Función interna
static void SetHook(UINT8 comparing) {
    vkCode = 0;
    vkToCompare = comparing;
    hKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyEvent, GetModuleHandle(NULL), 0);
    if (hKeyHook == NULL) printf("Hook installation failed!\n");
}

//! Función interna
void ReleaseHook() {
    UnhookWindowsHookEx(hKeyHook);
}

DWORD getNewKeyPress() {
    MSG msg;
    SetHook(0);
    
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (vkCode != 0) break;
    }
    
    ReleaseHook();
    return vkCode;
}

void detectKeyPress(DWORD vk) {
    MSG msg;
    SetHook(vk);
    
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (vkCode == vk) break;
    }

    ReleaseHook();
}