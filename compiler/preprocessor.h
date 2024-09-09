/*
    Copyright (C) ZerTeam.
*/
#ifndef PREPROCESSOR
#define PREPROCESSOR

#include <stdio.h>
#include <stdlib.h>
#include "working_with_files.h"

// Библиотеки ZRC
#define INPUT_ZRC_PATH "libraries/_input_.zrc"

#define false 0
#define true 1


//Библиотеки
//Первая это STDIO_H
//Вторая это STRING_H
//Третья это STDLIB_H
//Четвёртая это _INPUT_.ZRC
//0 это нет, 1 это да

int preprocessor(char *pathFileC, FILE *fileC, unsigned short *libraries)
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
        char *dataInputLib = (char*)malloc(255);
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
    
    return 0;
}

#endif