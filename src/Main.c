#include <windows.h>
#include <stdio.h>
#include "Functions.h"

int main() {
    HANDLE hStdin;
    DWORD fdwMode, fdwSaveOldMode;
    INPUT_RECORD irInBuf[128];
    DWORD cNumRead;

    SetConsoleOutputCP(CP_UTF8); // Habilitar la escritura de caracteres UTF-8

    // Obtener el manejador de la entrada estándar
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin == INVALID_HANDLE_VALUE) {
        printf("Error al obtener el manejador de la consola\n");
        return 1;
    }

    // Guardar el modo de entrada original para restaurarlo al final
    if (!GetConsoleMode(hStdin, &fdwSaveOldMode)) {
        printf("Error al obtener el modo de la consola\n");
        return 1;
    }

    // Configurar el modo de entrada para habilitar la entrada del ratón y procesar Ctrl+C
    fdwMode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT;
    if (!SetConsoleMode(hStdin, fdwMode)) {
        printf("Error al configurar el modo de la consola\n");
        return 1;
    }

    clearConsole();
    printMenu();

    // Loop principal
    while (1) {
        // Esperar a que ocurra un evento en la consola
        if (!ReadConsoleInput(hStdin, irInBuf, 128, &cNumRead)) {
            printf("Error al leer la entrada de la consola\n");
            return 1;
        }

        // Procesar los eventos de la consola
        for (DWORD i = 0; i < cNumRead; i++) {
            if (irInBuf[i].EventType == MOUSE_EVENT) {
                MOUSE_EVENT_RECORD mer = irInBuf[i].Event.MouseEvent;

                // Detectar un clic del ratón
                if (mer.dwEventFlags == 0) {
                    int x = mer.dwMousePosition.X;
                    int y = mer.dwMousePosition.Y;

                    movePointer(0, 10);
                    printf("Clic detectado en (%d, %d)", x, y);

                    // Aquí puedes manejar los clics según las coordenadas
                    if (x >= 0 && x <= 30 && y == 2) {
                        movePointer(0, 11);
                        printf("Opción 1 seleccionada");
                    }
                    else if (x >= 0 && x <= 30 && y == 3) {
                        movePointer(0, 11);
                        printf("Opción 2 seleccionada");
                    }
                    else if (x >= 0 && x <= 30 && y == 4) {
                        movePointer(0, 11);
                        printf("Opción 3 seleccionada");
                    }
                    else {
                        movePointer(0, 11);
                        printf("Clic fuera del menú");
                    }
                }
            }
        }
    }

    // Restaurar el modo de entrada original antes de salir
    SetConsoleMode(hStdin, fdwSaveOldMode);

    return 0;
}