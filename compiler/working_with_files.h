/*
    Copyright (C) Zer Team.
*/
#ifndef WORKING_WITH_FILES
#define WORKING_WITH_FILES

#if defined(_WIN32) || defined(_WIN64)
    #define RED_COLOR_TEXT     ""
    #define RESET_COLOR_TEXT   ""
#else
    #define RED_COLOR_TEXT     "\033[1;31m"
    #define RESET_COLOR_TEXT   "\033[0m"
#endif

// РАБОТА С ФАЙЛАМИ

#include <stdio.h>
typedef char* p_char;

// Подсчёт количества строк
int countingNumberLines(char *pathFile) {
    FILE *openFile = fopen(pathFile, "r");
    char letter;
    unsigned int lines = 0;
    if (openFile == NULL) {
        printf("%sFile not found.%s\n", RED_COLOR_TEXT,RESET_COLOR_TEXT);
        return -1;
    }

    while ((letter = fgetc(openFile)) != EOF) {
        if (letter == '\n') {
            lines++;
        }
    }
    fseek(openFile, -1, SEEK_END);
    if (fgetc(openFile) != '\n') {
        lines++;
    }
    fclose(openFile);
    return lines;
}

// Создания файла C
int createFile(p_char name){
    FILE *file = fopen(name,"w");
    if (file==NULL) 
    {
        perror("Error creating file:");
        return -1;
    }
    fclose(file);
    return 0;
}

// Дописывания строки в файл
int addToFile(FILE *file, p_char content){    
    if (file==NULL) {
        printf("%sCompilation error: file not found.%s", RED_COLOR_TEXT, RESET_COLOR_TEXT);
        return 1;
    }
    fprintf(file, content);
    return 0;
}

// Дописывания строки в начало файла
int addToFileStart(p_char filePath, p_char content) {
    FILE *originalFile = fopen(filePath, "r");
    if (!originalFile) {
        printf("%sError opening original file.%s", RED_COLOR_TEXT, RESET_COLOR_TEXT);
        return -1;
    }

    FILE *tempFile = tmpfile();
    if (!tempFile) {
        printf("%sError opening temporary file.%s", RED_COLOR_TEXT, RESET_COLOR_TEXT);
        fclose(originalFile);
        return -1;
    }

    fputs(content, tempFile);

    char buffer[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), originalFile)) > 0) {
        fwrite(buffer, 1, bytesRead, tempFile);
    }

    fclose(originalFile);
    rewind(tempFile);

    FILE *outputFile = fopen(filePath, "w");
    if (!outputFile) {
        perror("Error reopening original file for writing");
        fclose(tempFile);
        return -1;
    }

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), tempFile)) > 0) {
        fwrite(buffer, 1, bytesRead, outputFile);
    }

    fclose(tempFile);
    fclose(outputFile);
    return 0;
}

#endif