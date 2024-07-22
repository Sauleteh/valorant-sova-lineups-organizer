cd .\src
gcc main.c -o main.exe -lgdi32 -luser32
if not exist "..\dist" mkdir "..\dist"
move main.exe ..\dist
cd ..\dist
.\main.exe