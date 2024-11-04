/*
    Copyright (C) Zer Team.
*/
#ifndef RETRIEVING_CONTENT_LINE_BY_LINE
#define RETRIEVING_CONTENT_LINE_BY_LINE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "working_with_files.h"
#include "decoding.h"
#include "preprocessor.h"

#if defined(_WIN32) || defined(_WIN64)
    #define RED_COLOR_TEXT     ""
    #define RESET_COLOR_TEXT   ""
#else
    #define RED_COLOR_TEXT     "\033[1;31m"
    #define RESET_COLOR_TEXT   "\033[0m"
#endif

#define false 0
#define true 1

#define MAX_VARS 5000
#define MAX_VARS_NAME 500

#define MAX_FUNCTIONS 5000
#define MAX_FUNCTIONS_NAME 500

#define MAX_FUNCTIONS_ARG 500
#define MAX_FUNCTIONS_ARG_NAME 400

typedef unsigned int u_int;
typedef unsigned short u_short;
typedef char * p_char;

// Получение контента построчно
int getContentsStrings(p_char filePathZr, p_char pathFileC, p_char pathFileP, u_int sizeString){
    // Переменная для ошибок
    short Error = false;
    // Место нахождения (в функции или нет)
    u_short entryFR = false;
    // Переменные
    char **vars = (char **)malloc(MAX_VARS * MAX_VARS_NAME);
    u_short varsMode[MAX_VARS];
    u_short varsQuantity = 0;
    // Функции
    char **functions = (char **)malloc(MAX_FUNCTIONS * MAX_FUNCTIONS_NAME);
    u_short functionsMode[MAX_FUNCTIONS];
    u_short functionsQuantity = 0;
    // Аргументы в функции
    char **funcArg = (char **)malloc(MAX_FUNCTIONS_ARG * MAX_FUNCTIONS_ARG_NAME);
    u_short funcArgMode[MAX_FUNCTIONS_ARG];
    u_short funcArgQuantity = 0;

    if(vars==NULL||functions==NULL||funcArg==NULL)
    {
        printf("%sMemory allocation error.%s", RED_COLOR_TEXT, RESET_COLOR_TEXT);
        return -1;
    }

    for (u_int index = 0; index < MAX_VARS || index < MAX_FUNCTIONS || index < MAX_FUNCTIONS_ARG; index++)
    {
        if (index<MAX_VARS)
        {
            vars[index] = (char*)malloc(MAX_VARS_NAME);
            if (vars[index] == NULL)
            {
                printf("%sMemory allocation error.%s", RED_COLOR_TEXT, RESET_COLOR_TEXT);
                return -1;
            }
        }
        if (index<MAX_FUNCTIONS)
        {
            functions[index] = (char*)malloc(MAX_FUNCTIONS_NAME);
            if (functions[index] == NULL)
            {
                printf("%sMemory allocation error.%s", RED_COLOR_TEXT, RESET_COLOR_TEXT);
                return -1;
            }
        }
        if (index<MAX_FUNCTIONS_ARG)
        {
            funcArg[index] = (char*)malloc(MAX_FUNCTIONS_ARG_NAME);
            if (funcArg[index] == NULL)
            {
                printf("%sMemory allocation error.%s", RED_COLOR_TEXT, RESET_COLOR_TEXT);
                return -1;
            }
        }
        
    }
    
    // Библиотеки
    // Первая это STDIO_H
    // Вторая это STRING_H
    // Третья это STDLIB_H
    // Четвёртая это _INPUT_.ZRC
    // 0 это нет, 1 это да
    unsigned short libraries[5] = {0, 0, 0, 0, 0};

    // Открыть файла C
    FILE *fileC = fopen(pathFileC, "a");
    Error = addToFile(fileC, "int main(){\n");
    if (Error==true) return -1;

    // Открытия файла P
    FILE *fileP = fopen(pathFileP, "a");

    // Открытия файла zr
    FILE *fileZr = fopen(filePathZr,"r");
    if (fileZr==NULL||filePathZr==NULL) 
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
        Error = decodingString(content, number, fileC, pathFileC, pathFileP, strlen(content), vars, varsMode, &varsQuantity, functions, functionsMode, &functionsQuantity, funcArg, funcArgMode, &funcArgQuantity, &entryFR, libraries);
        if (Error==-1) return -1;
    }
    if (Error==-1) return -1;

    // Очистка памяти
    free(content);

    // Проверка закрытия функций
    if (entryFR==2)
    {
        printf("%sError: the function in the code is not closed!%s\n", RED_COLOR_TEXT, RESET_COLOR_TEXT);
        return -1;
    }
    

    for (u_int index = 0; index < MAX_VARS || index < MAX_FUNCTIONS || index < MAX_FUNCTIONS_ARG; index++)
    {
        if (index<MAX_VARS)          free(vars[index]);
        if (index<MAX_FUNCTIONS)     free(functions[index]);
        if (index<MAX_FUNCTIONS_ARG) free(funcArg[index]);
    }
    
    free(vars);
    free(functions);
    free(funcArg);

    // Закрытия файла zr
    fclose(fileZr);

    // Дописывания в файл C и его закрытие 
    addToFile(fileC, "return 0;}\n");
    fclose(fileC);
    
    // Препроцессор
    fileC = fopen(pathFileC, "a");
    preprocessor(pathFileC, pathFileP, fileC, libraries);

    addToFileStart(pathFileC, "/* This file is compiled by the ZeRCompiler compiler from the Zer language. More details http://zer-lang.org. */\n");
    addToFile(fileC,          "/* This file is compiled by the ZeRCompiler compiler from the Zer language. More details http://zer-lang.org. */\n");

    fclose(fileP);
    fclose(fileC);

    return 0;
} 
#endif