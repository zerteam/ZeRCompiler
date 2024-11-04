/*
    Copyright (C) Zer Team.
*/
#ifndef WORKING_WITH_TEXT
#define WORKING_WITH_TEXT

#include <string.h>
#include <ctype.h>

// РАБОТА С ТЕКСТОМ

// Обрезание пробелов в начале и в конце текста
void trim(char *str) {
    char *start = str;  // указатель на начало строки
    char *end;

    // Находим первое непустое место
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    // Если строка состоит только из пробелов
    if (*start == 0) {
        *str = 0;
        return;
    }

    // Обрезаем пробелы в конце
    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }

    // Ставим новый завершающий нулевой символ
    *(end + 1) = 0;

    // Копируем обрезанную строку в начало
    memmove(str, start, end - start + 2);
}

// Привод текста к нижнему регистру
void toLowerCase(char* str) {
    while (*str) {
        *str = tolower(*str);
        str++;
    }
}

#endif