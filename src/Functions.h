#include <windows.h>
#include <stdio.h>

#define OPTIONS_HEIGHT 2
typedef int (*FuncPtr)(int, int, UINT8);

// Mover el cursor a una posición específica en la consola
void movePointer(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Imprimir el menú
void printMenu(char** directories, UINT8 count) {
    movePointer(0, 0);
    printf("Click on a map to create a new lineup:");

    for (UINT8 i = 0; i < count; i++) {
        movePointer(0, i + OPTIONS_HEIGHT);
        printf("%d. %s", i + 1, directories[i]);
    }
    movePointer(0, count + OPTIONS_HEIGHT + 1);
    printf("* Note: To get more maps, create a new folder with the name of the map.");
}

int handleMenu(int x, int y, UINT8 count) {
    for (UINT8 i = 0; i < count; i++) {
        if (x >= 0 && x <= 30 && y == i + OPTIONS_HEIGHT) {
            return i;
        }
    }
    return -1;
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

int clickHandler(HANDLE hStdin, INPUT_RECORD* irInBuf, DWORD cNumRead, FuncPtr callback, UINT8 count) {
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
                    int option = callback(x, y, count);
                    if (option >= 0) return option;
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

char** listDirectories(const char* path, UINT8* count) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    char dirSpec[MAX_PATH];
    char** directories = NULL;
    int dirCount = 0;

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
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) {
                // Reallocar el arreglo de strings
                directories = realloc(directories, sizeof(char*) * (dirCount + 1));
                if (directories == NULL) {
                    printf("Error locating memory\n");
                    FindClose(hFind);
                    *count = 0;
                    return NULL;
                }
                // Almacenar el nombre del directorio
                directories[dirCount] = strdup(findFileData.cFileName);
                if (directories[dirCount] == NULL) {
                    printf("Error duplicating folder's name\n");
                    FindClose(hFind);
                    *count = 0;
                    return NULL;
                }
                dirCount++;
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
    *count = dirCount;
    return directories;
}