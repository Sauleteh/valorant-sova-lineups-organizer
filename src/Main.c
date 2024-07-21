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
        printf("Error obtaining input handle\n");
        return 1;
    }

    // Guardar el modo de entrada original para restaurarlo al final
    if (!GetConsoleMode(hStdin, &fdwSaveOldMode)) {
        printf("Error obtaining console mode\n");
        return 1;
    }

    // Configurar el modo de entrada para habilitar la entrada del ratón y procesar Ctrl+C
    fdwMode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT;
    if (!SetConsoleMode(hStdin, fdwMode)) {
        printf("Error setting console mode\n");
        return 1;
    }

    clearConsole();
    printMenu();

    // Loop principal
    while (1) {
        // Esperar a que ocurra un evento en la consola
        if (!ReadConsoleInput(hStdin, irInBuf, 128, &cNumRead)) {
            printf("Error reading console input\n");
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

/**
 * TODO list:
 * - [ ] Detectar qué carpetas existen en el directorio actual para mostrarlas como opciones en el menú
 * - [ ] Si se detecta más de un monitor, se debe preguntar cuál es el monitor que se está utilizando para el Valorant (esto se pregunta al principio del programa y se guardará en un archivo de configuración)
 * - [ ] Al seleccionar un mapa, se debe guardar qué se seleccionó
 * - [ ] Después de escoger un mapa, se debe elegir en qué parte del mapa se realizará el lineup (A, B, C u Other)
 * - [ ] Si no está guardado en la config, se debe preguntar al usuario qué tecla utilizar para realizar la captura de pantalla y se guarda posteriormente en el archivo de configuración
 * - [ ] Una vez seleccionado el mapa y la parte, se pedirá al usuario que saque tres capturas de pantalla
 *       - [ ] La primera captura será mirar hacia dónde hay que colocarse
 *       - [ ] La segunda captura será mirar hacia dónde se debe lanzar la habilidad
 *       - [ ] La tercera captura será ver dónde cae la habilidad
 *       - [ ] Las capturas de pantalla NO se cogen del portapapeles; se detecta cuándo se presiona imprimir pantalla y entonces se procesa la pantalla (esto se hace porque si hay más de un monitor, el portapapeles tendrá la captura de todos los monitores)
 */