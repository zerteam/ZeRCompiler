/*
    Copyright (C) Zer Team.
*/
#ifndef READING_LINE
#define READING_LINE
#include <stdio.h>
#include <string.h>

// Получения данных от пользователя
void receivingLine(char *data, unsigned short maxLength)
{
    char tempData;    
    short numChar;
    while ((tempData=getchar())!='\n'&&numChar<=maxLength)
    {
        data[numChar++]=tempData;
    }
}
#endif