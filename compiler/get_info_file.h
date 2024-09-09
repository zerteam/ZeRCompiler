/*
    Copyright (C) ZerTeam.
*/
#ifndef GET_INFO_FILE
#define GET_INFO_FILE
#if defined(_WIN32) || defined(_WIN64)
    #define RED_COLOR_TEXT     ""
    #define RESET_COLOR_TEXT   ""
#else
    #define RED_COLOR_TEXT     "\033[1;31m"
    #define RESET_COLOR_TEXT   "\033[0m"
#endif
#include <string.h>
#include <stdio.h>
#include "working_with_text.h"

typedef char* p_char;

//Получения только пути файла из пути к нему с именем
char* getPathFile(p_char filePathZr, unsigned short sizeString){
    static char TempData[191];
    strcpy(TempData, filePathZr);
    p_char pSlash = strrchr(TempData, '\\');
    if (pSlash==NULL) 
    {
        pSlash = strrchr(TempData, '/');
        if (pSlash==NULL) 
        {
            return "0";
        }
        *pSlash = '\0';
    }
    else{
        *pSlash = '\0';
    }
    return TempData;
}

//Получения имени файла из пути к нему
char* getNameFile(p_char filePathZr, unsigned short sizeString){
    static char nameFile[100];
    char path[235];
    strcpy(path, filePathZr);
    p_char sawn1 = strrchr(path, '\\');
    if (sawn1==NULL) sawn1 = strrchr(path, '/');
    if (sawn1==NULL) {   
    p_char sawn2= strrchr(path, '.');
    if (sawn2==NULL)
    {
        printf("%sFile processing error. This is not a .zr file.%s\n", RED_COLOR_TEXT, RESET_COLOR_TEXT);
        return "-1";
    }
        *sawn2='\0';
        strcpy(nameFile, path);

    }
    else{
    p_char sawn2 = strrchr(sawn1, '.');
    if (sawn2==NULL)
    {
        printf("%sFile processing error. This is not a .zr file.%s\n", RED_COLOR_TEXT, RESET_COLOR_TEXT);
        return "-1";
    }
    
    *sawn2 = '\0';
    
    for (int i = 0; i < sizeString; i++)
    {
        sawn1[i]=sawn1[i+1];
    }
    strcpy(nameFile, sawn1);
    }
    return nameFile;
}

// Проверка расширения файла
int CheckFileExtension(p_char filePathZr){
    char expansion[5];
    p_char dot = strrchr(filePathZr, '.');
    if (dot==NULL)
    {
        printf("%sFile processing error. This is not a .zr file.%s\n", RED_COLOR_TEXT, RESET_COLOR_TEXT);
        return -1;
    }
    unsigned short position = 0;
    while (*dot!='\0'){
        expansion[position++] = *dot++;
    }

    toLowerCase(expansion);

    if (strcmp(expansion, ".zer")!=0&&strcmp(expansion, ".zr")!=0)
    {
        printf("%sThis is not a .zer or .zr file.%s\n", RED_COLOR_TEXT, RESET_COLOR_TEXT);
        return -1;
    }
    
    return 0;
}

#endif