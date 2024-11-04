/*
    Copyright (C) Zer Team.
*/
#ifndef PREPROCESSOR
#define PREPROCESSOR

// ПРЕПРОЦЕССОР

#include <stdio.h>
#include <stdlib.h>
#include "working_with_files.h"

// Библиотеки ZRC
#define INPUT_ZRC_PATH "libraries/_input_.zrc"

#define false 0
#define true 1

typedef char* p_char;

// Библиотеки
// Первая это STDIO_H
// Вторая это STRING_H
// Третья это STDLIB_H
// Четвёртая это _INPUT_.ZRC
// 0 это нет, 1 это да

int preprocessor(p_char pathFileC, p_char pathFileP, FILE *fileC, unsigned short *libraries)
{
    if (libraries[0]==true)
    {
        addToFileStart(pathFileC, "#include <stdio.h>\n");
    }
    if (libraries[1]==true)
    {
        addToFileStart(pathFileC, "#include <string.h>\n");
    }
    if (libraries[2]==true)
    {
        addToFileStart(pathFileC, "#include <stdlib.h>\n");
    }
    if (libraries[3]==true)
    {
        addToFileStart(pathFileC, "void inputT(char *data, unsigned short maxLength);\n");
        addToFileStart(pathFileC, "void inputN(long double *data);\n");
        FILE *fileInputLib = fopen(INPUT_ZRC_PATH, "r");
        int *numLinesInputLib = (int*)malloc(sizeof(int));
        p_char dataInputLib = (p_char)malloc(255);
        if (numLinesInputLib==NULL)
        {
            printf("%sMemory allocation error.%s", RED_COLOR_TEXT, RESET_COLOR_TEXT);
            return -1;
        }
        *numLinesInputLib = countingNumberLines(INPUT_ZRC_PATH);

        if (*numLinesInputLib==-1)    return -1;

        if (libraries[0]==0)    addToFileStart(pathFileC, "#include <stdio.h>\n");
        if (libraries[1]==0)    addToFileStart(pathFileC, "#include <string.h>\n");
        if (libraries[2]==0)    addToFileStart(pathFileC, "#include <stdlib.h>\n");
        

        for (int _line = 0; _line < *numLinesInputLib; _line++)
        {
            fgets(dataInputLib, 255, fileInputLib);
            addToFile(fileC, dataInputLib);
        }
        
        free(numLinesInputLib);
        free(dataInputLib);
        fclose(fileInputLib);
    }
    
    // Запись кода из файла P
    FILE *fileP = fopen(pathFileP, "r");
    if (fileP==NULL)
    {
        printf("%sError opening file P for reading.%s", RED_COLOR_TEXT, RESET_COLOR_TEXT);
        return -1;
    }

    char *data = (char*)malloc(1000);
    if (data==NULL)
    {
        printf("%sMemory allocation error.%s", RED_COLOR_TEXT, RESET_COLOR_TEXT);
        return -1;
    }

    while (fgets(data, 1000, fileP)!=NULL)
    {
        addToFile(fileC, data);
        puts(data);
    }

    free(data);
    fclose(fileP);
    return 0;
}

#endif