#include <windows.h>
#include <stdio.h>

//! Función interna
void saveBitmapToFile(HBITMAP hBitmap, HDC hDC, int width, int height, const char* filename) {
    BITMAPFILEHEADER bfHeader;
    BITMAPINFOHEADER biHeader;
    BITMAPINFO bInfo;
    HBITMAP hOldBitmap;
    HDC hMemDC;
    HANDLE hFile;
    DWORD dwBytesWritten = 0;
    unsigned char* pBitmapBits;
    int rowSize = ((width * 32 + 31) & ~31) >> 3; // Align to DWORD boundary

    pBitmapBits = (unsigned char*)malloc(rowSize * height);

    // Set the BITMAPINFO structure
    bInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bInfo.bmiHeader.biWidth = width;
    bInfo.bmiHeader.biHeight = height;
    bInfo.bmiHeader.biPlanes = 1;
    bInfo.bmiHeader.biBitCount = 32;
    bInfo.bmiHeader.biCompression = BI_RGB;
    bInfo.bmiHeader.biSizeImage = 0;
    bInfo.bmiHeader.biXPelsPerMeter = 0;
    bInfo.bmiHeader.biYPelsPerMeter = 0;
    bInfo.bmiHeader.biClrUsed = 0;
    bInfo.bmiHeader.biClrImportant = 0;

    // Create a compatible device context
    hMemDC = CreateCompatibleDC(hDC);
    hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

    // Copy the bits from the bitmap into the pBitmapBits buffer
    GetDIBits(hMemDC, hBitmap, 0, height, pBitmapBits, &bInfo, DIB_RGB_COLORS);

    // Create the bitmap file
    hFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("Could not create file for writing\n");
        return;
    }

    // Set up the file header
    bfHeader.bfType = 0x4D42; // 'BM'
    bfHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + rowSize * height;
    bfHeader.bfReserved1 = 0;
    bfHeader.bfReserved2 = 0;
    bfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // Write the file header
    WriteFile(hFile, &bfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);

    // Write the info header
    WriteFile(hFile, &bInfo.bmiHeader, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);

    // Write the bitmap bits
    WriteFile(hFile, pBitmapBits, rowSize * height, &dwBytesWritten, NULL);

    // Clean up
    SelectObject(hMemDC, hOldBitmap);
    DeleteDC(hMemDC);
    CloseHandle(hFile);
    free(pBitmapBits);
}

void makeScreenshot() {
    HDC hScreenDC, hMemoryDC;
    HBITMAP hBitmap;
    int width, height;

    // Get the screen device context
    hScreenDC = GetDC(NULL);
    // Get the dimensions of the screen
    width = GetDeviceCaps(hScreenDC, HORZRES);
    height = GetDeviceCaps(hScreenDC, VERTRES);

    // Create a compatible device context and bitmap
    hMemoryDC = CreateCompatibleDC(hScreenDC);
    hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);
    SelectObject(hMemoryDC, hBitmap);

    // Copy the screen to the bitmap
    BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);

    // Save the bitmap to a file
    saveBitmapToFile(hBitmap, hScreenDC, width, height, "screenshot.bmp");

    // Clean up
    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(NULL, hScreenDC);

    printf("Screenshot saved as screenshot.bmp\n");
}