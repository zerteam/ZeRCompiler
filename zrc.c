/*
    Copyright (©) 2024 Zer Team.

    ZeRCompiler(ZRC) is the official compiler for the Zer programming language.

    License: ./LICENSE.txt and http://zer-lang.org/LICENSE.html

    More information: http://zer-lang.org.
*/

// Для работы с текстом
#include "compiler/working_with_text.h"
// Для получения пути к файл от пользователя
#include "compiler/receiving_line.h"
// Для получения имени файла
#include "compiler/get_info_file.h"
// Для подсчёта строк
#include "compiler/working_with_files.h"
// Для получение контента построчно
#include "compiler/retrieving_content_line_by_line.h"
// Для компиляции файла C в исполняемый файл
#include "compiler/compiling_to_executable_file.h"
#include <string.h>
#include <stdlib.h>

// Версия компилятора
#if defined(_WIN32) || defined(_WIN64)//_______ Windows
    #define VERSION "0.0.1 w"
#elif defined(__linux__)//_____________________ Linux
    #define VERSION "0.0.1 l"
#elif defined(__APPLE__) || defined(__MACH__)// MacOS
    #define VERSION "0.0.1 m"
#elif defined(__ANDROID__)//___________________ Android
    #define VERSION "0.0.1 a"
#elif defined(__unix__) || defined(__unix)//___ Unix
    #define VERSION "0.0.1 u"
#else //_______________________________________ Unsupported platform
    #define VERSION "Error: Unsupported platform"
#endif


#define VERSION_TEXT "\
###     ##   ##              #  #            ###     ###      # \n\
  # ### # # #   ### ### ###     #  ### ###   # #     # #     ## \n\
 #  ##  ##  #   # # ### # #  #  #  ##  #     # #     # #      # \n\
#   ### # # #   ### # # ###  #  ## ### #     # #     # #      # \n\
###     # #  ##         #                    ###  #  ###  #  ###\n"


// Названия файлов
#define FILE_C_NAME "programFileC.c\0"
#define FILE_P_NAME ".tempFileForPrep.zrc\0"

// Цвета для вывода в консоль
#if defined(_WIN32) || defined(_WIN64)
    #define RED_COLOR_TEXT     ""
    #define WHITE_COLOR_TEXT   ""
    #define BLUE_COLOR_TEXT    ""
    #define RESET_COLOR_TEXT   ""
#else
    #define RED_COLOR_TEXT     "\033[1;31m"
    #define WHITE_COLOR_TEXT   "\033[37m"
    #define BLUE_COLOR_TEXT    "\033[36m"
    #define RESET_COLOR_TEXT   "\033[0m"
#endif

#define SIZE_PATH 226
#define SIZE_ARG 4

typedef unsigned short u_short;

char filePathZr[SIZE_PATH] = "69";

int main(u_short numberArg, char *arg[SIZE_ARG]) {
    u_short deleteFileExistC = 0; //========================================================== 1
    u_short deleteFileExistP = 1;
    // Проверка, что передан хотя бы один аргумент
    if (numberArg > 1) {
        u_short pathExist = 0;
        for (u_short elem = 1; arg[elem]!=NULL||elem>SIZE_ARG; elem++)
        {
            toLowerCase(arg[elem]);
            if (!strcmp(arg[elem], "-v"))
            {
                printf(VERSION_TEXT);

                printf("\nZeRCompiler version %s\n", VERSION);
                return 0;
            }
            else if (!strcmp(arg[elem], "-h"))
            {
                printf("\
%soperators:\n\
 %s-v%s - Show version information.\n\
 %s-h%s - Help.\n\
 %s-n%s - Do not delete the temporary preprocessor file.\n\
 %s[pathFile]%s - path to the file.%s\n", WHITE_COLOR_TEXT, BLUE_COLOR_TEXT, WHITE_COLOR_TEXT, BLUE_COLOR_TEXT, WHITE_COLOR_TEXT, BLUE_COLOR_TEXT, WHITE_COLOR_TEXT, BLUE_COLOR_TEXT, WHITE_COLOR_TEXT, RESET_COLOR_TEXT);
                return 0;
            }
            
            else if (!strcmp(arg[elem], "-n"))
            {
                deleteFileExistP=0;
            }

            else if (pathExist!=1)
            {
                strcpy(filePathZr, arg[elem]);
                pathExist=1;
            }
            
            else
            {
                printf("%sYou cannot specify two file paths(\"%s\").%s\n",RED_COLOR_TEXT, arg[elem], RESET_COLOR_TEXT);
                return 1;
            }
        }
        // Проверка ввел ли пользователь путь к файлу
        if (strcmp(filePathZr, "69"))
        {
            printf("Enter the path to the zr file: ");
            // Получения данных от пользователя
            receivingLine(filePathZr, SIZE_PATH-1);
        }
    }
    // Получения пути к файлу в ручную
    else {
        printf("Enter the path to the zr file: ");
        // Получения данных от пользователя
        receivingLine(filePathZr, SIZE_PATH-1);
    }
    
    // Получения пути без имени файла
    char pathFile[101];
    if (getPathFile(filePathZr, strlen(filePathZr), pathFile)==-1) return 1;

    // Получения имени файла
    char nameFile[30];
    if (getNameFile(filePathZr, strlen(filePathZr), nameFile)==-1) return 1;

    // Проверка расширения файла является ли оно .zer или .zr
    if (CheckFileExtension(filePathZr)==-1) return 1;

    // Подсчёт количества строк в файле
    unsigned int numberOfLines = countingNumberLines(filePathZr);
    if (numberOfLines==-1) return 1;

    // Путь к C и P файлу 
    char filePathC[SIZE_PATH];
    char filePathP[SIZE_PATH];

    if (pathFile[0]!='\0')
    {
        strcpy(filePathC, pathFile);
        strcat(filePathC, "/");
        strcat(filePathC, FILE_C_NAME);

        strcpy(filePathP, pathFile);
        strcat(filePathP, "/");
        strcat(filePathP, FILE_P_NAME);
    } else{
        strcpy(filePathC, FILE_C_NAME);
        strcpy(filePathP, FILE_P_NAME); 
    }

    // Создание C и P файлов
    if (createFile(filePathC)==-1) return -1;
    if (createFile(filePathP)==-1) return -1;

    // Получение контента построчно и расшифровка zer и запись в C файл
    if (getContentsStrings(filePathZr, filePathC, filePathP, numberOfLines)==-1) 
    {   
        if(deleteFileExistC==1) remove(filePathC);
        if(deleteFileExistP==1) remove(filePathP);
        return 1;
    }
    // Удаление временного файла P
    if(deleteFileExistP==1) remove(filePathP);

    // Компиляция кода C
    if (compilingExecutableFile(nameFile,filePathC)==-1) 
    {   
        if(deleteFileExistC==1) remove(filePathC);
        return 1;
    }

    // Финальное удаления файла
    if(deleteFileExistC==1) remove(filePathC);

    return 0;
}

/*
  =============ПОЯСНЕНИЕ=============
    zr/zer файл - это файл с кодом zer который компилируется в С код.
    файл C - это файл в который компилируется код из zr/zer файла.
    файл P - это временный файл для препроцессора, он удаляется после компиляции.
    файлы с расширениям .zrc (ZeRCompiler file) - это файлы для компилятора (пример: _input_.zrc).
*/

/* ID: HL3000, Version: 0.0.1, Date: 2024-11-01, Author: Zer Team. */