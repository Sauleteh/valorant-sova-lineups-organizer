#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONFIG_FILE "config.cfg"
#define MAX_CONFIG_LINE_LENGTH 256

typedef struct {
    DWORD vkCode;
} Config;

void loadConfig(Config* config) {
    FILE* file = fopen(CONFIG_FILE, "r");
    if (file == NULL) {
        // Archivo no encontrado, usar valores predeterminados
        config->vkCode = 0;
        return;
    }

    char line[MAX_CONFIG_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        char key[MAX_CONFIG_LINE_LENGTH];
        char value[MAX_CONFIG_LINE_LENGTH];
        
        if (sscanf(line, "%[^=]=%s", key, value) == 2) {
            if (strcmp(key, "vkCode") == 0) {
                config->vkCode = atoi(value);
            }
        }
    }

    fclose(file);
}

void saveConfig(const Config* config) {
    FILE* file = fopen(CONFIG_FILE, "w");
    if (file == NULL) {
        printf("Cannot open config file in write mode\n");
        return;
    }

    fprintf(file, "vkCode=%d\n", config->vkCode);
    fclose(file);
}