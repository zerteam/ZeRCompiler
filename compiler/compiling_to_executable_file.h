/*
    Copyright (C) ZerTeam.
*/
#ifndef COMPILING_TO_EXECUTABLE_FILE
#define COMPILING_TO_EXECUTABLE_FILE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Компиляция файла C в исполняемый файл
int compilingExecutableFile(char *nameFile, char *PathFileC){
    char commandCompiling[256] = "gcc ";
    commandCompiling[256] = '\0';
    strcat(commandCompiling, PathFileC);
    strcat(commandCompiling, " -o ");
    strcat(commandCompiling, nameFile);
    int answerConsole = system(commandCompiling);
    return 0; 
}
#endif