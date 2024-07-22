#include <windows.h>
#include <stdio.h>

#define OPTIONS_HEIGHT 2
#define SITES_ARRAY_DEFINITION { "A", "B", "C", "Other" }

const char* SITES_ARRAY[] = SITES_ARRAY_DEFINITION;
typedef int (*FuncPtr)(int, int, UINT8);

// Mover el cursor a una posición específica en la consola
//! Función interna
void movePointer(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Imprimir el menú
void printMap(char** directories, UINT8 count) {
    movePointer(0, 0);
    printf("Click on a map to create a new lineup:");

    for (UINT8 i = 0; i < count; i++) {
        movePointer(0, i + OPTIONS_HEIGHT);
        printf("%d. %s", i + 1, directories[i]);
    }
    movePointer(0, count + OPTIONS_HEIGHT + 1);
    printf("* Note: To get more maps, create a new folder with the name of the map.");
}

void printSite() {
    movePointer(0, 0);
    printf("Pick a site where the lineup will be used:");
    movePointer(0, 2);
    printf("1. A site");
    movePointer(0, 3);
    printf("2. B site");
    movePointer(0, 4);
    printf("3. C site");
    movePointer(0, 5);
    printf("4. Other");
}

void printPressKey() {
    movePointer(0, 0);
    printf("Press the key you want to use to take a screenshot");
    movePointer(0, 2);
    printf("* Note: This will be requested only this time, the key will be saved in the configuration file.");
}

void printFirstScreenshot() {
    movePointer(0, 0);
    printf("Go to Valorant and press the key you have chosen to take a screenshot of the place where you want the player to stand");
}

void printSecondScreenshot() {
    movePointer(0, 0);
    printf("Go to Valorant and press the key you have chosen to take a screenshot of the place where the player should aim (set up the bounce and power too)");
}

void printThirdScreenshot() {
    movePointer(0, 0);
    printf("Go to Valorant and press the key you have chosen to take a screenshot of the place where the arrow should land");
}

//! Función interna
int handleClick(int x, int y, UINT8 count) {
    for (UINT8 i = 0; i < count; i++) {
        if (x >= 0 && x <= 30 && y == i + OPTIONS_HEIGHT) {
            return i;
        }
    }
    return -1;
}

int clickHandler(HANDLE hStdin, INPUT_RECORD* irInBuf, DWORD cNumRead, UINT8 count) {
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

                    int option = handleClick(x, y, count);
                    if (option >= 0) {
                        Sleep(100);
                        FlushConsoleInputBuffer(hStdin);
                        return option;
                    }
                }
            }
        }
    }
}

void clearConsole() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD count;
    DWORD cellCount;
    COORD homeCoords = { 0, 0 };

    if (hConsole == INVALID_HANDLE_VALUE) return;

    // Obtener información de la consola
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;
    cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    // Rellenar la consola con espacios en blanco
    if (!FillConsoleOutputCharacter(hConsole, (TCHAR)' ', cellCount, homeCoords, &count)) return;

    // Rellenar atributos de color
    if (!FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, homeCoords, &count)) return;

    // Mover el cursor a la posición inicial
    SetConsoleCursorPosition(hConsole, homeCoords);
}

// Si solo se desea filtrar los directorios, se debe pasar el valor 1 en el parámetro filterOnlyDirectories, 0 en caso contrario
char** listFiles(const char* path, UINT8* count, UINT8 filterOnlyDirectories) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    char dirSpec[MAX_PATH];
    char** files = NULL;
    int fileCount = 0;

    // Construir el path para buscar
    snprintf(dirSpec, MAX_PATH, "%s\\*", path);

    // Encontrar el primer archivo en el directorio
    hFind = FindFirstFile(dirSpec, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Path not found: [%s]\n", path);
        *count = 0;
        return NULL;
    } 

    // Iterar sobre todos los archivos y directorios encontrados
    do {
        // Filtrar sólo los directorios
        if (findFileData.dwFileAttributes & (filterOnlyDirectories == 1 ? FILE_ATTRIBUTE_DIRECTORY : 255)) {
            if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) {
                // Reallocar el arreglo de strings
                files = realloc(files, sizeof(char*) * (fileCount + 1));
                if (files == NULL) {
                    printf("Error locating memory\n");
                    FindClose(hFind);
                    *count = 0;
                    return NULL;
                }
                // Almacenar el nombre del directorio
                files[fileCount] = strdup(findFileData.cFileName);
                if (files[fileCount] == NULL) {
                    printf("Error duplicating folder's name\n");
                    FindClose(hFind);
                    *count = 0;
                    return NULL;
                }
                fileCount++;
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
    *count = fileCount;
    return files;
}