/*
    Copyright (©) 2024 ZerTeam.

    ZeRCompiler(ZRC) is the official compiler for the Zer programming language.

    You can modify, distribute and embed ZeRCompiler(ZRC) into your programs, BUT ONLY IF THEY ARE FREE AND AVAILABLE TO EVERYONE.

    More information: https://zer-lang.org.
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

#define VERSION "alpha 0.0.0 l"

#define VERSION_TEXT    "\
###     ##   ##              #  #            ###     ###     ### \n\
  # ### # # #   ### ### ###     #  ### ###   # #     # #     # # \n\
 #  ##  ##  #   # # ### # #  #  #  ##  #     # #     # #     # # \n\
#   ### # # #   ### # # ###  #  ## ### #     # #     # #     # # \n\
###     # #  ##         #                    ###  #  ###  #  ### \n"

#define TEMP_FILE_C_NAME "programFileC.c\0"


#if defined(_WIN32) || defined(_WIN64)
    #define RED_COLOR_TEXT     ""
    #define RESET_COLOR_TEXT   ""
#else
    #define RED_COLOR_TEXT     "\033[1;31m"
    #define RESET_COLOR_TEXT   "\033[0m"
#endif

#define SIZE_PATH 226
#define SIZE_ARG 4

typedef unsigned short u_short;

char filePathZr[SIZE_PATH] = "0";

int main(u_short numberArg, char *arg[SIZE_ARG]) {
    u_short deleteFileExist = 0;////////////////////////////////////////////////////////////////////////////// 1
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
                printf(
                "\033[37moperators:\n"
                "\033[36m-v\033[37m - Show version information.\n"
                "\033[36m-h\033[37m - Help.\n"
                "\033[36m\033[36m-n\033[37m - Do not delete temporary file.\n"
                "\033[36m[pathFile]\033[37m - path to the file.\n");
                return 0;
            }
            else if (!strcmp(arg[elem], "-n"))
            {
                deleteFileExist=0;
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
        if (filePathZr[0]=='0')
        {
            printf("Enter the path to the zr file: ");
            // Получения данных от пользователя
            receivingLine(filePathZr, SIZE_PATH-1);
        }
    }
    else {
        printf("Enter the path to the zr file: ");
        // Получения данных от пользователя
        receivingLine(filePathZr, SIZE_PATH-1);
    }
    
    // Получения пути без имени файла
    char *pathFile = getPathFile(filePathZr, strlen(filePathZr));
    if (pathFile=="-1") return 1;

    // Получения имени файла
    char *nameFile = getNameFile(filePathZr, strlen(filePathZr));
    if (nameFile=="-1") return 1;

    // Проверка расширения файла является ли оно .zer или .zr
    if (CheckFileExtension(filePathZr)==-1) return 1;

    // Подсчёт количества строк в файле
    unsigned int numberOfLines = countingNumberLines(filePathZr);
    if (numberOfLines==-1) return 1;

    // Путь к temp файлу 
    char FilePathC[256];
    if (pathFile!="0")
    {
        strcpy(FilePathC, pathFile);
        strcat(FilePathC, "/");
        strcat(FilePathC, TEMP_FILE_C_NAME);
    } else{
        strcpy(FilePathC, TEMP_FILE_C_NAME);
    }

    // Получение контента построчно и расшифровка zr и запись в C файл
    if (getContentsStrings(filePathZr, FilePathC, numberOfLines)==-1) 
    {   
        if(deleteFileExist==1)
        {
            remove(FilePathC); 
        }
        return 1;
    };
    // Компиляция кода C
    if (compilingExecutableFile(nameFile,FilePathC)==-1) 
    {   
        if(deleteFileExist==1){
            remove(FilePathC);
        }
        return 1;
    };

    // Финальное удаления файла
    if(deleteFileExist==1){
        remove(FilePathC);
    }

    return 0;
}