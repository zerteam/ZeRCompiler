/*
    Copyright (C) ZerTeam.
*/
#ifndef RETRIEVING_CONTENT_LINE_BY_LINE
#define RETRIEVING_CONTENT_LINE_BY_LINE

#if defined(_WIN32) || defined(_WIN64)
    #define RED_COLOR_TEXT     ""
    #define RESET_COLOR_TEXT   ""
#else
    #define RED_COLOR_TEXT     "\033[1;31m"
    #define RESET_COLOR_TEXT   "\033[0m"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "working_with_files.h"
#include "decoding.h"
#include "preprocessor.h"

typedef unsigned int u_int;
typedef char * p_char;

//Получение контента построчно
int getContentsStrings(p_char filePathZr, p_char pathFileC, u_int sizeString){
    short Error = 0;
    char vars[5000][1201];
    short varsMode[5000];
    u_int varsQuantity = 0;

    //Библиотеки
    //Первая это STDIO_H
    //Вторая это STRING_H
    //Третья это STDLIB_H
    //Четвёртая это _INPUT_.ZRC
    //0 это нет, 1 это да
    unsigned short libraries[5] = {0, 0, 0, 0, 0};

    //Создания временного файла
    createFile(pathFileC);
    FILE *fileC = fopen(pathFileC, "a");
    Error = addToFile(fileC, "int main(){\n");
    if (Error==1) return -1;

    //Открытия файла zr
    FILE *fileZr = fopen(filePathZr,"r");
    if (fileZr==NULL) 
    {
        printf("%sFile not found.%s", RED_COLOR_TEXT, RESET_COLOR_TEXT);
        return -1;
    }
    
    // Выделения памяти для хранения строки
    p_char content = malloc(1001);
    if (content==NULL)
    {
        printf("%sMemory allocation error.%s", RED_COLOR_TEXT, RESET_COLOR_TEXT);
        return -1;
    }

    // Цикл для перебора строк
    for (u_int number = 0; number < sizeString; number++)
    {
        // Получения строки
        fgets(content, 1001, fileZr);
        if (content[0]=='\n'||content[0]=='\r') continue;// Пропуск пустых строк
        if (content[0]==0x0) break;// Остановка если файл пустой
        Error = decodingString(content, number, fileC, pathFileC, strlen(content), vars, varsMode, &varsQuantity, libraries);
        if (Error==-1) return -1;
    }
    if (Error==-1) return -1;

    // Очистка памяти
    free(content);

    // Закрытия файла zr
    fclose(fileZr);

    // Дописывания в файл C и его закрытие 
    addToFile(fileC, "return 0;}\n");
    fclose(fileC);
    
    //Препроцессор
    fileC = fopen(pathFileC, "a");
    preprocessor(pathFileC, fileC, libraries);
    fclose(fileC);

    addToFileStart(pathFileC, "/* This file is compiled by the ZRC compiler from the Zer language. More details http://zer-lang.org. */\n");
    addToFile(pathFileC, "/* This file is compiled by the ZRC compiler from the Zer language. More details http://zer-lang.org. */\n");

    return 0;
} 
#endif