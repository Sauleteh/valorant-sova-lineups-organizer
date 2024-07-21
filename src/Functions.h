#include <windows.h>
#include <stdio.h>

// Mover el cursor a una posición específica en la consola
void movePointer(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Imprimir el menú
void printMenu() {
    movePointer(0, 0);
    printf("Haz click en un mapa para crear un lineup nuevo:");
    movePointer(0, 2);
    printf("1. Opción 1");
    movePointer(0, 3);
    printf("2. Opción 2");
    movePointer(0, 4);
    printf("3. Opción 3");
    movePointer(0, 6);
    printf("* Nota: Para que salgan más mapas, crea una carpeta nueva con el nombre del mapa.");
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