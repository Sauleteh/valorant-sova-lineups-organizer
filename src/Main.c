#include <windows.h>
#include <stdio.h>
#include "Functions.h"
#include "Screenshot.h"
#include "KeyboardHook.h"
#include "Config.h"

int main() {
    HANDLE hStdin;
    DWORD fdwMode, fdwSaveOldMode;
    INPUT_RECORD irInBuf[128];
    DWORD cNumRead;
    UINT8 directoryCount = 0;
    char** directories = listFiles(".", &directoryCount, 1);
    UINT8 selectedMapIndex = -1;
    UINT8 selectedSiteIndex = -1;
    Config config;

    loadConfig(&config);

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

    // Menú de selección de mapa
    clearConsole();
    printMap(directories, directoryCount);
    selectedMapIndex = clickHandler(hStdin, irInBuf, cNumRead, directoryCount);
    if (selectedMapIndex < 0) { printf("Error handling click\n"); return 1; }

    // Selección de site
    clearConsole();
    printSite();
    selectedSiteIndex = clickHandler(hStdin, irInBuf, cNumRead, 4);
    if (selectedSiteIndex < 0) { printf("Error handling click\n"); return 1; }

    // Si no hay una tecla guardada en la configuración, preguntar al usuario qué tecla utilizar
    if (config.vkCode == 0) {
        clearConsole();
        printPressKey();
        config.vkCode = getNewKeyPress();
        saveConfig(&config);
    }

    // Tomar la primera captura de pantalla
    clearConsole();
    printFirstScreenshot();
    detectKeyPress(config.vkCode);
    HBITMAP firstScreenshot = makeScreenshot();

    // Tomar la segunda captura de pantalla
    clearConsole();
    printSecondScreenshot();
    detectKeyPress(config.vkCode);
    HBITMAP secondScreenshot = makeScreenshot();

    // Tomar la tercera captura de pantalla
    clearConsole();
    printThirdScreenshot();
    detectKeyPress(config.vkCode);
    HBITMAP thirdScreenshot = makeScreenshot();

    // Obtener el número de imágenes que ya existen en la carpeta con el mismo site
    UINT8 imageCount = 0;
    char** images = listFiles(directories[selectedMapIndex], &imageCount, 0);
    UINT8 imageCountWithSameSite = 0;
    for (int i = 0; i < imageCount; i++) {
        if (strstr(images[i], SITES_ARRAY[selectedSiteIndex]) != NULL) imageCountWithSameSite++;
    }

    // Generar la imagen final
    generateFinalImage(firstScreenshot, secondScreenshot, thirdScreenshot, directories[selectedMapIndex], SITES_ARRAY[selectedSiteIndex], imageCountWithSameSite + 1);

    DeleteObject(thirdScreenshot);
    DeleteObject(secondScreenshot);
    DeleteObject(firstScreenshot);
    SetConsoleMode(hStdin, fdwSaveOldMode); // Restaurar el modo de entrada original antes de salir
    for (int i = 0; i < directoryCount; i++) free(directories[i]);
    free(directories);
    for (int i = 0; i < imageCount; i++) free(images[i]);
    free(images);

    return 0;
}

/**
 * TODO list:
 * - [X] Detectar qué carpetas existen en el directorio actual para mostrarlas como opciones en el menú
 * - [-] Si se detecta más de un monitor, se debe preguntar cuál es el monitor que se está utilizando para el Valorant (esto se pregunta al principio del programa y se guardará en un archivo de configuración)
 *       - REMOVED: No es necesario ya que automáticamente se detecta el monitor principal y de forma general, se asume que el monitor principal es el que se utiliza para jugar
 * - [X] Al seleccionar un mapa, se debe guardar qué se seleccionó
 * - [X] Después de escoger un mapa, se debe elegir en qué parte del mapa se realizará el lineup (A, B, C u Other)
 * - [X] Si no está guardado en la config, se debe preguntar al usuario qué tecla utilizar para realizar la captura de pantalla y se guarda posteriormente en el archivo de configuración
 * - [X] Una vez seleccionado el mapa y la parte, se pedirá al usuario que saque tres capturas de pantalla
 *       - [X] La primera captura será mirar hacia dónde hay que colocarse
 *       - [X] La segunda captura será mirar hacia dónde se debe lanzar la habilidad
 *       - [X] La tercera captura será ver dónde cae la habilidad
 *       - [X] Las capturas de pantalla NO se cogen del portapapeles; se detecta cuándo se presiona imprimir pantalla y entonces se procesa la pantalla (esto se hace porque si hay más de un monitor, el portapapeles tendrá la captura de todos los monitores)
 * - [X] El nombre de la imagen final será el site seleccionado y el número de imagen (por ejemplo, "A_001.bmp")
 */