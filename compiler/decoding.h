/*
    Copyright (C) Zer Team.
*/
#ifndef DECODING_FILE_ZRC
#define DECODING_FILE_ZRC

#define CONTENT_VAR_SIZE   401
#define ARG_FUNC_SIZE      700
#define MAX_VARS_NAME      500
#define SIZE_TEXT_CONTENT  552
#define MAX_LINE_FUNCTION  1000
#define MAX_FUNCTIONS      5000
#define MAX_FUNCTIONS_NAME 500

// РАСШИФРОВКА КОД ZER

#include "working_with_files.h"
#include "working_with_text.h"
#include <string.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(_WIN64)
    #define RED_COLOR_TEXT     ""
    #define RESET_COLOR_TEXT   ""
#else
    #define RED_COLOR_TEXT     "\033[1;31m"
    #define RESET_COLOR_TEXT   "\033[0m"
#endif

#define _INPUT_ZRC "Ext_C_Lib/_input_.zrc"


typedef unsigned short u_short;
typedef char * p_char;

int decodingString(p_char string, u_short line, FILE *file, p_char pathFileC, p_char pathFileP, u_short sizeString, p_char *vars, short *varsMode, u_short *varsQuantity, p_char *functions, short *functionsMode, u_short *functionsQuantity, p_char *functionArg, short *functionArgMode, u_short *functionArgQuantity, u_short *entryFR, u_short *libraries)
{
    char command[16] = "";
    command[16] = '\0';
    int location = 0;
    int _location = 0;
    int outputLineIndex = 0; 
    int _outputLineIndex = 0; 
    if (*entryFR==2) file = fopen(pathFileP, "a");

    if (*entryFR==1 && strchr(string, '{'))
    {
        *entryFR = 2;
        return 0;
    }
    else if (*entryFR==2 && strchr(string, '}'))
    {
        *entryFR = 0;
        fclose(file);
        *functionArgQuantity=0;
        FILE *Temp = fopen(pathFileP, "a");
        addToFile(file, "}\n");
        fclose(Temp);
        return 0;
    }

    // Уничтожения знаков пробела
    trim(string);

    // Проверка существования команд
    if(strchr(string, ':')==NULL&&strchr(string, '@')==NULL&&strchr(string, '$')==NULL&&(strchr(string, '<')==NULL||strchr(string, '>')==NULL)) {
        printf("%sError reading \"%s\" on line %d.%s\n", RED_COLOR_TEXT, string, (line + 1), RESET_COLOR_TEXT);        
        return -1;
    }

    if ((strchr(string, '@')!=NULL&&strchr(string, '(')!=NULL&&strchr(string, ')')!=NULL&&strchr(string, ':')==NULL)||(strchr(string, '$')!=NULL&&strchr(string, '(')!=NULL&&strchr(string, ')')!=NULL&&strchr(string, ':')==NULL))
    {
        char funcName[MAX_FUNCTIONS_NAME];
        char funcArg[ARG_FUNC_SIZE];
        // Имя
        for (int numberChar = 0; numberChar < sizeString+1; numberChar++)
        {
            // Пропуск комментария
            if (string[numberChar] == '/' && string[numberChar + 1] == '/')
                return 0;

            if (string[numberChar] == '@'||string[numberChar] == '$')
            {
                location = !location;
                if (!location)
                {
                    location = 0;
                    outputLineIndex = 0;
                    break;
                }
            }
            else if (location)
            {
                funcName[outputLineIndex++] = string[numberChar];
                funcName[outputLineIndex] = '\000';
            }
        }
        // Проверка на существования
        for (int i = 0; i < *functionsQuantity; i++)
        {
            if (strcmp(functions[i], funcName) == 0)
            {
                // Аргументы
                for (int numberChar = 0; numberChar < sizeString+1; numberChar++)
                {
                    if (string[numberChar] == '('||string[numberChar] == ')')
                    {
                        location = !location;
                        if (!location&&string[numberChar] == ')')
                        {
                            location = 0;
                            outputLineIndex = 0;
                            break;
                        }
                    }
                    else if (location)
                    {
                        if (string[numberChar]=='<'||string[numberChar]=='>') funcArg[outputLineIndex++]=' ';
                        else if (string[numberChar]=='`') funcArg[outputLineIndex++]='\"';
                        else if (string[numberChar]=='~') funcArg[outputLineIndex++]=' ';
                        else funcArg[outputLineIndex++] = string[numberChar];
                    }
                }
                // Запись в файл
                addToFile(file, funcName);
                addToFile(file, "(");
                addToFile(file, funcArg);
                addToFile(file, ");\n");
                return 0;
            }   
        }
        printf("%sError in line %d: Function named %s not found.%s", RED_COLOR_TEXT, (line+1), funcName, RESET_COLOR_TEXT);
        return -1;
    }

    if (strchr(string, '<')!=NULL&&strchr(string, '>')!=NULL&&(strchr(string, '=')!=NULL||strchr(string, '-')!=NULL||strchr(string, '+')!=NULL)&&strchr(string, ':')==NULL)
    {
        char varName[MAX_VARS_NAME];
        // Получения имя
        for (int numberChar = 0; numberChar<sizeString+1; numberChar++)
        {
            if (string[numberChar] == '<' || string[numberChar] == '>')
            {
                location = !location;
                if (string[numberChar] == '>')
                {
                    location = 0;
                    outputLineIndex = 0;
                    break;
                }
            }
            else if (location)
            {
                varName[outputLineIndex++] = string[numberChar];
                varName[outputLineIndex] = '\000';
            }
        }

        trim(varName);

        // Получение режима
        for (u_int i = 0; i < *varsQuantity; i++)
        {
            if (strcmp(vars[i], varName) == 0)
            {
                if (varsMode[i]==1||varsMode[i]==11||varsMode[i]==10)
                {
                    printf("%sError in line %d: You are trying to modify a constant or string variable.%s", RED_COLOR_TEXT, (line+1), RESET_COLOR_TEXT);
                    return -1;
                }
                
                // Выделение памяти
                p_char _content = (p_char)malloc(CONTENT_VAR_SIZE);

                if (strchr(string, '=')!=NULL)
                {
                    for (int numberChar = 0; numberChar<sizeString+1; numberChar++)
                    {
                        // Проверка контента
                        if (numberChar>sizeString)
                        {
                            printf("%sError at line %d: symbols \"=\" not found.%s", RED_COLOR_TEXT, (line + 1), RESET_COLOR_TEXT);
                            free(_content);
                            return -1;
                        }
                        // Начало и конец записи контента из ==
                        if (string[numberChar] == '=')
                        {
                            location = !location;
                            if (!location)
                            {
                                _content[outputLineIndex] = '\000';
                                break;
                            }
                        }
                        // Запись контента из == в _content
                        else if (location)
                        {
                            _content[outputLineIndex++] = string[numberChar];
                        }
                    }
                    if (!strcmp(_content, ""))
                    {
                        printf("%sError in line %d: You haven't specified the values.%s", RED_COLOR_TEXT, (line+1), RESET_COLOR_TEXT);
                        return -1;
                    }
                    addToFile(file, varName);
                    addToFile(file, " = ");
                    addToFile(file, _content);
                    addToFile(file, ";\n");
                }
                else if (strchr(string, '+')!=NULL)
                {
                    for (int numberChar = 0; numberChar<sizeString+1; numberChar++)
                    {
                        // Проверка контента
                        if (numberChar>sizeString)
                        {
                            printf("%sError at line %d: symbols \"+\" not found.%s", RED_COLOR_TEXT, (line + 1), RESET_COLOR_TEXT);
                            free(_content);
                            return -1;
                        }
                        // Начало и конец записи контента из ++
                        if (string[numberChar] == '+')
                        {
                            location = !location;
                            if (!location)
                            {
                                _content[outputLineIndex] = '\000';
                                break;
                            }
                        }
                        // Запись контента из ++ в _content
                        else if (location)
                        {
                            _content[outputLineIndex++] = string[numberChar];
                        }
                    }
                    if (!strcmp(_content, ""))
                    {
                        printf("%sError in line %d: You haven't specified the values.%s", RED_COLOR_TEXT, (line+1), RESET_COLOR_TEXT);
                        return -1;
                    }
                    addToFile(file, varName);
                    addToFile(file, " += ");
                    addToFile(file, _content);
                    addToFile(file, ";\n");
                }
                else if (strchr(string, '-')!=NULL)
                {
                    for (int numberChar = 0; numberChar<sizeString+1; numberChar++)
                    {
                        // Проверка контента
                        if (numberChar>sizeString)
                        {
                            printf("%sError at line %d: symbols \"-\" not found.%s", RED_COLOR_TEXT, (line + 1), RESET_COLOR_TEXT);
                            free(_content);
                            return -1;
                        }
                        // Начало и конец записи контента из --
                        if (string[numberChar] == '-')
                        {
                            location = !location;
                            if (!location)
                            {
                                _content[outputLineIndex] = '\000';
                                break;
                            }
                        }
                        // Запись контента из -- в _content
                        else if (location)
                        {
                            _content[outputLineIndex++] = string[numberChar];
                        }
                    }
                    if (!strcmp(_content, ""))
                    {
                        printf("%sError in line %d: You haven't specified the values.%s", RED_COLOR_TEXT, (line+1), RESET_COLOR_TEXT);
                        return -1;
                    }
                    addToFile(file, varName);
                    addToFile(file, " -= ");
                    addToFile(file, _content);
                    addToFile(file, ";\n");
                }

                free(_content);
                return 0;
            }
        }
        printf("%sError in line %d: Variable \"%s\" is not declared.%s", RED_COLOR_TEXT, (line+1), varName, RESET_COLOR_TEXT);       
        return -1;
    }

    // Получения команды
    for (int numberChar = 0; numberChar < sizeString+1; numberChar++)
    {
        // Пропуск комментария
        if (string[numberChar] == '/' && string[numberChar + 1] == '/')
            return 0;

        // Получение команды
        if (string[numberChar] == ':')
        {
            location = !location;
            if (!location)
            {
                location = 0;
                outputLineIndex = 0;
                break;
            }
        }
        else if (location)
        {
            command[outputLineIndex++] = string[numberChar];
        }
    }
    
    // Уничтожения знаков пробела
    trim(command);
    
    // output
    if (!strcmp(command, "output"))
    {
        char textContent[SIZE_TEXT_CONTENT] = "\0";

        if (strchr(string, '`')!=NULL)
        {
            // Получения текста из ``
            for (int numberChar = 0; numberChar<sizeString+1; numberChar++)
            {
                // Проверка контента
                if (numberChar>sizeString)
                {
                    printf("%sError on line %d: backtick(`) not found.%s", RED_COLOR_TEXT, (line + 1), RESET_COLOR_TEXT);
                    return -1;
                }
                // Начало и конец записи контента из ``
                if (string[numberChar] == '`')
                {
                    location = !location;
                    if (!location)
                        break;
                }
                // Запись контента из `` в textContent
                else if (location)
                {
                    textContent[outputLineIndex++] = string[numberChar];
                    textContent[outputLineIndex] = '\000';
                }
            }
            
            // Исправления ", %, /, ^ в обратную сторону
            for (short numberChar = 0; numberChar < sizeString; numberChar++)
            {
                // "
                if (textContent[numberChar] == '"' && textContent[numberChar - 1] != '\\')
                {
                    for (short numberCharEnd = (SIZE_TEXT_CONTENT - 1); numberCharEnd > numberChar; numberCharEnd--)
                    {
                        textContent[numberCharEnd] = textContent[numberCharEnd - 1];
                    }
                    textContent[numberChar] = '\\';
                }
                // /
                if (textContent[numberChar] == '\\' && textContent[numberChar - 1] != '\\' && textContent[numberChar - 2] != '\\' && textContent[numberChar - 3] != '\\')
                {
                    for (short numberCharEnd = (SIZE_TEXT_CONTENT - 1); numberCharEnd > numberChar; numberCharEnd--)
                    {
                        textContent[numberCharEnd] = textContent[numberCharEnd - 1];
                    }
                    textContent[numberChar] = '\\';
                }
                // %
                if (textContent[numberChar] == '%' && textContent[numberChar - 1] != '%' && textContent[numberChar - 2] != '%' && textContent[numberChar - 3] != '%')
                {
                    for (short numberCharEnd = (SIZE_TEXT_CONTENT - 3); numberCharEnd > numberChar; numberCharEnd--)
                    {
                        textContent[numberCharEnd] = textContent[numberCharEnd - 3];
                    }
                    textContent[numberChar] = '%';
                    textContent[numberChar + 1] = '%';
                    textContent[numberChar + 2] = '%';
                }
                // ^n
                if (textContent[numberChar] == '^' && textContent[numberChar + 1] == 'n')
                {
                    textContent[numberChar]='\\';
                    textContent[numberChar+1]='n';
                }
                
            }

            libraries[0]=1;
        
            addToFile(file, "printf(\"");
            addToFile(file, textContent);
            addToFile(file, "\");\n");
        }
        
        else if (strchr(string, '~')!=NULL)
        {
            // Получения текста из ``
            for (int numberChar = 0; numberChar<sizeString+1; numberChar++)
            {
                // Проверка контента
                if (numberChar>sizeString)
                {
                    printf("%sError on line %d: backtick(`) not found.%s", RED_COLOR_TEXT, (line + 1), RESET_COLOR_TEXT);
                    return -1;
                }
                // Начало и конец записи контента из ``
                if (string[numberChar] == '~')
                {
                    location = !location;
                    if (!location)
                        break;
                }
                // Запись контента из `` в textContent
                else if (location)
                {
                    textContent[outputLineIndex++] = string[numberChar];
                    textContent[outputLineIndex] = '\000';
                }
            }

            libraries[0]=1;

            addToFile(file, "printf(\"%%s\", \"");
            addToFile(file, textContent);
            addToFile(file, "\");\n");
        }
        
        else if (strchr(string, '<')!=NULL&&strchr(string, '>')!=NULL)
        {            
            char varName[MAX_VARS_NAME] = "\0";
            // Получения текста из <>
            for (int numberChar = 0; numberChar<sizeString+1; numberChar++)
            {
                // Проверка контента
                if (numberChar>sizeString)
                {
                    printf("%sError at line %d: Greater than(<) or less than(>) symbol not found.%s", RED_COLOR_TEXT, (line + 1), RESET_COLOR_TEXT);
                    return -1;
                }
                // Начало и конец записи контента из ``
                if (string[numberChar] == '<'||string[numberChar] == '>')
                {
                    location = !location;
                    if (!location&&string[numberChar] == '>')
                        break;
                }
                // Запись контента из `` в textContent
                else if (location)
                {
                    varName[outputLineIndex++] = string[numberChar];
                    varName[outputLineIndex] = '\000';
                }
            }
            for (int i = 0; i < *varsQuantity; i++)
            {
                if (strcmp(vars[i],varName) == 0)
                {
                    if (varsMode[i]==0||varsMode[i]==10)
                    {
                        libraries[0]=1;
                        if (*entryFR==2) addToFile(file, "printf(\"%%%%Lf\", ");
                        else addToFile(file, "printf(\"%%Lf\", ");
                        addToFile(file, vars[i]);
                        addToFile(file, ");\n");
                    }
                    if (varsMode[i]==1||varsMode[i]==11)
                    {
                        libraries[0]=1;
                       if (*entryFR==2)  addToFile(file, "printf(\"%%%%s\", ");
                       else  addToFile(file, "printf(\"%%s\", ");
                        addToFile(file, vars[i]);
                        addToFile(file, ");\n");
                    } 
                }   
            }
        }

        else if (strchr(string, '&')!=NULL && *entryFR==2)
        {
            char argName[MAX_VARS_NAME] = "\0";
            // Получения текста из <>
            for (int numberChar = 0; numberChar<sizeString+1; numberChar++)
            {
                // Проверка контента
                if (numberChar>sizeString)
                {
                    printf("%sError at line %d: Closing & symbol not found.%s", RED_COLOR_TEXT, (line + 1), RESET_COLOR_TEXT);
                    return -1;
                }
                // Начало и конец записи контента из ``
                if (string[numberChar] == '&')
                {
                    location = !location;
                    if (!location)
                        break;
                }
                // Запись контента из `` в textContent
                else if (location)
                {
                    argName[outputLineIndex++] = string[numberChar];
                    argName[outputLineIndex] = '\000';
                }
            }
            for (int i = 0; i < *functionArgQuantity; i++)
            {
                if (strcmp(functionArg[i], argName) == 0)
                {
                    if (functionArgMode[i]==0)
                    {
                        libraries[0]=1;
                        addToFile(file, "printf(\"%%%%Lf\", ");
                        addToFile(file, argName);
                        addToFile(file, ");\n");
                        break;
                    }
                    else if (functionArgMode[i]==1)
                    {
                        libraries[0]=1;
                        addToFile(file, "printf(\"%%%%s\", ");
                        addToFile(file, argName);
                        addToFile(file, ");\n");
                        break;
                    } 
                }   
            }
        }

        else if (strchr(string, '$')!=NULL)
        {
            
            char funcName[MAX_VARS_NAME] = "\0";
            p_char funcArgs = (p_char)malloc(ARG_FUNC_SIZE + 2);

            funcArgs[0] = '(';
            
            // Получения текста из 
            for (int numberChar = 0; numberChar<sizeString+1; numberChar++)
            {
                // Проверка контента
                if (numberChar>sizeString)
                {
                    printf("%sError at line %d.%s", RED_COLOR_TEXT, (line + 1), RESET_COLOR_TEXT);
                    return -1;
                }
                // Начало и конец записи контента из ``
                if (string[numberChar] == '$')
                {
                    location = !location;
                    if (!location)
                    {
                        location = 0;
                        outputLineIndex = 0;
                        break;
                    }
                }
                // Запись имени
                else if (location)
                {
                    funcName[outputLineIndex++] = string[numberChar];
                }
            }            
            
             // Получение аргументов
            if (strchr(string, '(') == NULL || strchr(string, ')') == NULL)
            {
                printf("%sError in line %d: No parentheses \"()\"%s", RED_COLOR_TEXT, (line+1), RESET_COLOR_TEXT);
                free(funcArgs);
                return -1;
            }

            outputLineIndex++;

            for (int numberChar = 0; numberChar < sizeString; numberChar++)
            {
                if (string[numberChar] == '(' || string[numberChar] == ')')
                {
                    location = !location;
                    if (string[numberChar] == ')')
                    {
                        funcArgs[outputLineIndex] = ')';
                        location = 0;
                        outputLineIndex = 0;
                        break;
                    }
                }
                else if (location)
                {
                    funcArgs[outputLineIndex++] = string[numberChar];
                    funcArgs[outputLineIndex] = '\000';
                }
            }

            trim(funcName);

            for (int i = 0; i < *functionsQuantity; i++)
            {
                if (strcmp(functions[i], funcName) == 0)
                {
                    libraries[0]=1;
                    addToFile(file, "printf(\"%%Lf\", ");
                    addToFile(file, funcName);
                    addToFile(file, funcArgs);
                    addToFile(file, ");\n");
                    free(funcArgs);
                    return 0;
                }
            }

            printf("%sError in line %d: The function named \"%s\" is not declared.%s", RED_COLOR_TEXT, (line+1), funcName, RESET_COLOR_TEXT);
            free(funcArgs);
            return -1;
        }
        
        else if (strchr(string, '@')!=NULL)
        {
            
            char funcName[MAX_VARS_NAME] = "\0";
            p_char funcArgs = (p_char)malloc(ARG_FUNC_SIZE + 2);
            funcArgs[0] = '(';
            // Получения текста из 
            for (int numberChar = 0; numberChar<sizeString+1; numberChar++)
            {
                // Проверка контента
                if (numberChar>sizeString)
                {
                    printf("%sError at line %d.%s", RED_COLOR_TEXT, (line + 1), RESET_COLOR_TEXT);
                    return -1;
                }
                // Начало и конец записи контента
                if (string[numberChar] == '@')
                {
                    location = !location;
                    if (!location)
                    {
                        location = 0;
                        outputLineIndex = 0;
                        break;
                    }
                }
                // Запись имени
                else if (location)
                {
                    funcName[outputLineIndex++] = string[numberChar];
                    funcName[outputLineIndex] = '\0';
                }
            }
            
            if (strchr(string, '(') == NULL || strchr(string, ')') == NULL)
            {
                printf("%sError in line %d: No parentheses \"()\"%s", RED_COLOR_TEXT, (line+1), RESET_COLOR_TEXT);
                free(funcArgs);
                return -1;
            }

            outputLineIndex++;

            for (int numberChar = 0; numberChar < sizeString; numberChar++)
            {
                if (string[numberChar] == '(' || string[numberChar] == ')')
                {
                    location = !location;
                    if (string[numberChar] == ')')
                    {
                        funcArgs[outputLineIndex++] = ')';
                        funcArgs[outputLineIndex] = '\000';
                        location = 0;
                        outputLineIndex = 0;
                        break;
                    }
                }
                else if (location)
                {
                    funcArgs[outputLineIndex++] = string[numberChar];
                }
            }
        
            for (int i = 0; i < *functionsQuantity; i++)
            {
                if (strcmp(functions[i], funcName) == 0)
                {
                    libraries[0]=1;
                    addToFile(file, "printf(\"%%s\", ");
                    addToFile(file, funcName);
                    addToFile(file, funcArgs);
                    addToFile(file, ");\n");
                    free(funcArgs);
                    return 0;
                }
            }
            printf("%sError in line %d: The function named \"%s\" is not declared.%s", RED_COLOR_TEXT, (line+1), funcName, RESET_COLOR_TEXT);
            free(funcArgs);
            return -1;
        }
        
        else
        {
            printf("%sError on line %d: backtick(`) not found.%s\n", RED_COLOR_TEXT, (line + 1), RESET_COLOR_TEXT);
            return -1;
        }
    }
    
    // input
    else if (!strcmp(command, "input"))
    {
        char textContent[SIZE_TEXT_CONTENT] = "\0";

        if (strchr(string, '<')!=NULL&&strchr(string, '>')!=NULL)
        {
            
            char varName[MAX_VARS_NAME] = "\0";
            // Получения текста из <>
            for (int numberChar = 0; numberChar<sizeString+1; numberChar++)
            {
                // Проверка контента
                if (numberChar>sizeString)
                {
                    printf("%sError at line %d: Greater than(<) or less than(>) symbol not found.%s", RED_COLOR_TEXT, (line + 1), RESET_COLOR_TEXT);
                    return -1;
                }
                // Начало и конец записи контента из ``
                if (string[numberChar] == '<'||string[numberChar] == '>')
                {
                    location = !location;
                    if (!location&&string[numberChar] == '>')
                        break;
                }
                // Запись контента из `` в textContent
                else if (location)
                {
                    varName[outputLineIndex++] = string[numberChar];
                    varName[outputLineIndex] = '\000';
                }
            }
            
            for (int i = 0; i < *varsQuantity; i++)
            {
                if (strcmp(vars[i], varName) == 0)
                {
                    libraries[3]=1;
                    
                    if (varsMode[i]==0)
                    {
                        addToFile(file, "inputN(&");
                        addToFile(file, vars[i]);
                        addToFile(file, ");\n");
                    }
                    else if (varsMode[i]==1)
                    {
                        addToFile(file, "inputT(");
                        addToFile(file, vars[i]);
                        addToFile(file, ", sizeof(");
                        addToFile(file, vars[i]);
                        addToFile(file, ")-1);\n");
                    }
                    else 
                    {
                        printf("%sError at line %d: Writing data to a constant is not possible.%s", RED_COLOR_TEXT, (line + 1), RESET_COLOR_TEXT);
                        return -1;
                    } 
                }   
            }
        }
        
        if (strchr(string, '&')!=NULL&&*entryFR==2)
        {
            
            char argName[MAX_VARS_NAME] = "\0";
            // Получения текста
            for (int numberChar = 0; numberChar<sizeString+1; numberChar++)
            {
                // Проверка контента
                if (numberChar>sizeString)
                {
                    printf("%sError at line %d: Closing & symbol not found.%s", RED_COLOR_TEXT, (line + 1), RESET_COLOR_TEXT);
                    return -1;
                }
                // Начало и конец записи контента
                if (string[numberChar] == '&')
                {
                    location = !location;
                    if (!location)
                        break;
                }
                // Запись контента и
                else if (location)
                {
                    argName[outputLineIndex++] = string[numberChar];
                    argName[outputLineIndex] = '\000';
                }
            }
            
            for (int i = 0; i < *functionArgQuantity; i++)
            {
                if (strcmp(functionArg[i], argName) == 0)
                {
                    libraries[3]=1;
                    
                    if (functionArgMode[i]==0)
                    {
                        addToFile(file, "inputN(&");
                        addToFile(file, functionArg[i]);
                        addToFile(file, ");\n");
                    }
                    else if (functionArgMode[i]==1)
                    {
                        addToFile(file, "inputT(");
                        addToFile(file, functionArg[i]);
                        addToFile(file, ", sizeof(");
                        addToFile(file, functionArg[i]);
                        addToFile(file, ")-1);\n");
                    }
                    else 
                    {
                        printf("%sError at line %d: Writing data to a constant is not possible.%s", RED_COLOR_TEXT, (line + 1), RESET_COLOR_TEXT);
                        return -1;
                    } 
                }   
            }
        }

        else
        {
            printf("%sError on line %d: backtick(`) not found.%s", RED_COLOR_TEXT, (line + 1), RESET_COLOR_TEXT);
            return -1;
        }
    }

    // var
    else if (!strcmp(command, "var"))
    {
        
        u_short dataVarsize = 0;
        char nameVar[MAX_VARS_NAME] = "";
        char contentVar[CONTENT_VAR_SIZE] = "";
        u_short mode; // 0 числа с запятой; 1 строка
        // Получения имени
        for (int numberChar = 0; numberChar<sizeString+1; numberChar++)
        {
            if (string[numberChar] == '<' || string[numberChar] == '>')
            {
                location = !location;
                if (string[numberChar] == '>')
                {
                    location = 0;
                    outputLineIndex = 0;
                    break;
                }
            }
            else if (location)
            {
                nameVar[outputLineIndex++] = string[numberChar];
            }
        }
        //Получения данных
        if (strchr(string, '`') != NULL)
        {
            mode = 1;
            for (int numberChar = 0; numberChar < sizeString; numberChar++)
            {
                if (string[numberChar] == '`')
                {
                    location = !location;
                    if (!location)
                    {
                        location = 0;
                        outputLineIndex = 0;
                        break;
                    }
                }
                else if (location)
                {
                    contentVar[outputLineIndex++] = string[numberChar];
                }
            }
            for (u_short numberChar = 0; numberChar < strlen(contentVar); numberChar++)
            {
                if (contentVar[numberChar]=='^'&&contentVar[numberChar+1]=='n')
                {
                    contentVar[numberChar]='\\';
                    contentVar[numberChar+1]='n';
                }
                
            }
            
        }

        else if(strchr(string, '~') != NULL)
        {
            mode = 0;
            for (int numberChar = 0; numberChar < sizeString; numberChar++)
            {
                if (string[numberChar] == '~')
                {
                    location = !location;
                    if (!location)
                    {
                        location = 0;
                        outputLineIndex = 0;
                        break;
                    }
                }
                else if (location)
                {
                    contentVar[outputLineIndex++] = string[numberChar];
                }
            }
            if (strchr(contentVar, '.') != NULL) mode=0;
        }

        else
        {
            printf("%sError reading incorrect value specified in variable \"%s\" line %d.%s", RED_COLOR_TEXT,  nameVar, (line+1), RESET_COLOR_TEXT);

            return -1;
        }

        if (mode==0)
        {
            addToFile(file, "long double ");
            addToFile(file, nameVar);
            if (strcmp(contentVar, ""))
            {
                addToFile(file, " = ");
                addToFile(file, contentVar);
            }
            addToFile(file, ";\n");
            // Добавления переменной в реестр
            varsMode[*varsQuantity]=mode;
            strcpy(vars[*varsQuantity], nameVar);
            *varsQuantity+=1;
        } else if (mode==1)
        {
            addToFile(file, "char ");
            addToFile(file, nameVar);
            addToFile(file, "[1201] = \"");
            addToFile(file, contentVar);
            addToFile(file, "\\0\";\n");
            // Добавления переменной в реестр
            varsMode[*varsQuantity]=mode;
            strcpy(vars[*varsQuantity], nameVar);
            *varsQuantity+=1;
        }
    }

    // const
    else if (!strcmp(command, "const"))
    {
        
        u_short dataVarsize = 0;
        char nameVar[MAX_VARS_NAME] = "";
        char contentVar[CONTENT_VAR_SIZE] = "";
        u_short mode; // 0 числа с запятой; 1 строка
        // Получения имени
        for (int numberChar = 0; numberChar<sizeString+1; numberChar++)
        {
            if (string[numberChar] == '<' || string[numberChar] == '>')
            {
                location = !location;
                if (string[numberChar] == '>')
                {
                    location = 0;
                    outputLineIndex = 0;
                    break;
                }
            }
            else if (location)
            {
                nameVar[outputLineIndex++] = string[numberChar];
            }
        }
        //Получения данных
        if (strchr(string, '`') != NULL)
        {
            mode = 1;
            for (int numberChar = 0; numberChar < sizeString; numberChar++)
            {
                if (string[numberChar] == '`')
                {
                    location = !location;
                    if (!location)
                    {
                        location = 0;
                        outputLineIndex = 0;
                        break;
                    }
                }
                else if (location)
                {
                    contentVar[outputLineIndex++] = string[numberChar];
                }
            }
        
            for (u_short numberChar = 0; numberChar < strlen(contentVar); numberChar++)
            {
                if (contentVar[numberChar]=='^'&&contentVar[numberChar+1]=='n')
                {
                    contentVar[numberChar]='\\';
                    contentVar[numberChar+1]='n';
                }
                
            }
            
        }

        else if(strchr(string, '~') != NULL)
        {
            mode = 0;
            for (int numberChar = 0; numberChar < sizeString; numberChar++)
            {
                if (string[numberChar] == '~')
                {
                    location = !location;
                    if (!location)
                    {
                        location = 0;
                        outputLineIndex = 0;
                        break;
                    }
                }
                else if (location)
                {
                    contentVar[outputLineIndex++] = string[numberChar];
                }
            }
            if (strchr(contentVar, '.') != NULL) mode=0;
        }

        else
        {
            printf("%sError reading incorrect value specified in variable \"%s\" line %d.%s", RED_COLOR_TEXT,  nameVar, (line+1), RESET_COLOR_TEXT);

            return -1;
        }

        if (mode==0)
        {
            addToFile(file, "const long double ");
            addToFile(file, nameVar);
            addToFile(file, " = ");
            addToFile(file, contentVar);
            addToFile(file, ";\n");
            varsMode[*varsQuantity]=10;
            strcpy(vars[*varsQuantity], nameVar);
            *varsQuantity+=1;
        } else if (mode==1)
        {
            addToFile(file, "const char ");
            addToFile(file, nameVar);
            addToFile(file, "[1201] = \"");
            addToFile(file, contentVar);
            addToFile(file, "\\0\";\n");
            varsMode[*varsQuantity]=11;
            strcpy(vars[*varsQuantity], nameVar);
            *varsQuantity+=1;
        }
    }
              
    // functions
    else if (!strcmp(command, "function") || !strcmp(command, "func"))
    {
        char funcName[MAX_FUNCTIONS_NAME] = "";
        char funcArg[ARG_FUNC_SIZE] = "";
        u_short variableOrType = 0;// 0 Тип, 1 - Переменная
        u_short modeReturn; // 0 числа с запятой; 1 строка
        // Получения имени
        if (strchr(string, '@') != NULL)
        {
            for (int numberChar = 0; numberChar<sizeString+1; numberChar++)
            {
                if (string[numberChar] == '@')
                {
                    location = !location;
                    if (!location)
                    {
                        location = 0;
                        outputLineIndex = 0;
                        modeReturn = 1;
                        break;
                    }
                }
                else if (location)
                {
                    funcName[outputLineIndex++] = string[numberChar];
                }
            }
        }
        else if (strchr(string, '$') != NULL)
        {
            for (int numberChar = 0; numberChar<sizeString+1; numberChar++)
            {
                if (string[numberChar] == '$')
                {
                    location = !location;
                    if (!location) 
                    {
                        location = 0;
                        outputLineIndex = 0;
                        modeReturn = 0;
                        break;
                    }
                }
                else if (location)
                {
                    funcName[outputLineIndex++] = string[numberChar];
                }
            }
        }
        else
        {
            printf("%sThe function type in line %d is not recognized. Try using $func$ for numbers or @func@ for strings.%s", RED_COLOR_TEXT, (line+1), RESET_COLOR_TEXT);
            return -1;
        }

        // Получения данных
        if (strchr(string, '(') != NULL && strchr(string, ')') != NULL)
        {
            for (int numberChar = 0; numberChar < sizeString; numberChar++)
            {
                if (string[numberChar] == '(' || string[numberChar] == ')')
                {
                    location = !location;
                    if (string[numberChar] == ')')
                    {
                        location = 0;
                        outputLineIndex = 0;
                        break;
                    }
                }
                else if (location)
                {
                    if (string[numberChar]=='n'&&string[numberChar+1]=='u'&&string[numberChar+2] =='m')
                    {
                        if (variableOrType==1&&string[numberChar+3] ==','||variableOrType==1&&string[numberChar+3] =='\0'||variableOrType==1&&string[numberChar+3] ==' ')
                        {
                            printf("%sAn argument cannot be named the same as a type. Error in line %d.%s", RED_COLOR_TEXT, (line+1), RESET_COLOR_TEXT);
                            return -1;
                        }
                        
                        else if (string[numberChar+3] ==' ')
                        {
                            numberChar += 3;
                            strcat(funcArg, "long double ");
                            outputLineIndex+=12;
                            variableOrType = 1;
                            for (int _numberChar = numberChar-1; _numberChar < sizeString; _numberChar++)
                            {
                                if (string[_numberChar] == ' ' || string[_numberChar] == ',' || string[_numberChar] == ')')
                                {
                                    _location = !_location;
                                    if (string[_numberChar] == ',' || string[_numberChar] == ')')
                                    {
                                        _location = 0, _outputLineIndex = 0;
                                        break;
                                    }
                                }
                                else if (_location)
                                {
                                    functionArg[*functionArgQuantity][_outputLineIndex++] = string[_numberChar];
                                    functionArg[*functionArgQuantity][_outputLineIndex] = '\000';
                                }
                            }
                            trim(functionArg[*functionArgQuantity]);
                            functionArgMode[*functionArgQuantity] = 0;
                            *functionArgQuantity+=1;
                        }
                        else{
                            funcArg[outputLineIndex++] = string[numberChar];
                            variableOrType = 0;
                        }
                    }
                    else if (string[numberChar]=='s'&&string[numberChar+1]=='t'&&string[numberChar+2] =='r')
                    {
                        if (variableOrType==1&&string[numberChar+3] ==','||variableOrType==1&&string[numberChar+3] =='\0'||variableOrType==1&&string[numberChar+3] ==' ')
                        {
                            printf("%sAn argument cannot be named the same as a type. Error in line %d.%s", RED_COLOR_TEXT, (line+1), RESET_COLOR_TEXT);
                            return -1;
                        }
                        
                        else if (string[numberChar+3] ==' ')
                        {
                            numberChar += 3;
                            strcat(funcArg, "char* ");
                            outputLineIndex += 6;
                            variableOrType = 1;
                            for (int _numberChar = numberChar-1; _numberChar < sizeString; _numberChar++)
                            {
                                if (string[_numberChar] == ' ' || string[_numberChar] == ',' || string[_numberChar] == ')')
                                {
                                    _location = !_location;
                                    if (string[_numberChar] == ',' || string[_numberChar] == ')')
                                    {
                                        _location = 0, _outputLineIndex = 0;
                                        break;
                                    }
                                }
                                else if (_location)
                                {
                                    functionArg[*functionArgQuantity][_outputLineIndex++] = string[_numberChar];
                                    functionArg[*functionArgQuantity][_outputLineIndex] = '\000';
                                }
                            }
                            trim(functionArg[*functionArgQuantity]);
                            functionArgMode[*functionArgQuantity] = 1;
                            *functionArgQuantity+=1;
                        }
                        else{
                            funcArg[outputLineIndex++] = string[numberChar];
                            variableOrType = 0;
                        }
                    }
                    
                    else{
                        funcArg[outputLineIndex++] = string[numberChar];
                        variableOrType = 0;
                    }
                }
            }
        }
        
        else
        {
            printf("%sError reading invalid value specified in function declaration \"%s\" line %d.%s", RED_COLOR_TEXT,  funcName, (line+1), RESET_COLOR_TEXT);
            return -1;
        }

        if (strchr(string, '{')!= NULL)   *entryFR = 2;
        else    *entryFR = 1;
        
        if (modeReturn==0)
        {   
            FILE *fileP = fopen(pathFileP, "a");
            char* TEMP = (char*)malloc(MAX_LINE_FUNCTION);
            sprintf(TEMP, "long double %s(%s)", funcName, funcArg);
            addToFile(fileP, TEMP);
            addToFile(fileP, " {\n");
            strcat(TEMP, ";\n");
            // Очистка
            addToFileStart(pathFileC, TEMP);
            free(TEMP);
            fclose(fileP);
            // Добавления функции в реестр
            functionsMode[*functionsQuantity] = modeReturn;
            strcpy(functions[*functionsQuantity], funcName);
            *functionsQuantity+=1;
        }
        else if (modeReturn==1)
        {   
            FILE *fileP = fopen(pathFileP, "a");
            char* TEMP = (char*)malloc(MAX_LINE_FUNCTION);
            sprintf(TEMP, "char* %s(%s)", funcName, funcArg);
            addToFile(fileP, TEMP);
            addToFile(fileP, " {\n");
            strcat(TEMP, ";\n");
            // Очистка
            addToFileStart(pathFileC, TEMP);
            free(TEMP);
            fclose(fileP);
            // Добавления функции в реестр
            functionsMode[*functionsQuantity] = modeReturn;
            strcpy(functions[*functionsQuantity], funcName);
            *functionsQuantity+=1;
        }
        else
        {
            printf("%sThe function type in line %d is not recognized. Try using $func$ for numbers or @func@ for strings.%s", RED_COLOR_TEXT, (line+1), RESET_COLOR_TEXT);
            return -1;
        }
    }
    
    // return for functions
    else if (!strcmp(command, "return") && *entryFR==2)
    {
        u_short funcMode = functionsMode[*functionsQuantity-1];
        

        if ((funcMode == 1 && strchr(string, '`') != NULL) || (funcMode == 0 && strchr(string, '~') != NULL))
        {
            p_char data = (p_char)malloc(CONTENT_VAR_SIZE);
            u_short mode;

            if (strchr(string, '`') != NULL)
            {
                mode = 1;
                for (int numberChar = 0; numberChar < sizeString; numberChar++)
                {
                    if (string[numberChar] == '`')
                    {
                        location = !location;
                        if (!location)
                        {
                            location = 0;
                            outputLineIndex = 0;
                            break;
                        }
                    }
                    else if (location)
                    {
                        data[outputLineIndex++] = string[numberChar];
                    }
                }
                for (u_short numberChar = 0; numberChar < strlen(data); numberChar++)
                {
                    if (data[numberChar]=='^'&&data[numberChar+1]=='n')
                    {
                        data[numberChar]='\\';
                        data[numberChar+1]='n';
                    }
                    
                }
                
            }

            else if(strchr(string, '~') != NULL)
            {
                mode = 0;
                for (int numberChar = 0; numberChar < sizeString; numberChar++)
                {
                    if (string[numberChar] == '~')
                    {
                        location = !location;
                        if (!location)
                        {
                            location = 0;
                            outputLineIndex = 0;
                            break;
                        }
                    }
                    else if (location)
                    {
                        data[outputLineIndex++] = string[numberChar];
                    }
                }
            }

            if (mode == 0)
            {
                addToFile(file, "return (long double)");
                addToFile(file, data);
                addToFile(file, ";\n");
                // Очистка
                free(data);
            }
            else if (mode == 1)
            {
                addToFile(file, "return \"");
                addToFile(file, data);
                addToFile(file, "\";\n");
                // Очистка
                free(data);
            }
            else
            {
                printf("%sError in line %d: Mode=%d STDERR=%s.%s\n", RED_COLOR_TEXT, (line + 1), mode, stderr, RESET_COLOR_TEXT);
                return -1;
            }
        }

        else if ((funcMode == 0 || funcMode == 1) && strchr(string, '<') != NULL && strchr(string, '>') != NULL)
        {
            char *nameVar = (char*)malloc(MAX_VARS_NAME);
            if (nameVar == NULL) 
            {
                printf("%sError: Memory allocation failed.%s\n", RED_COLOR_TEXT, RESET_COLOR_TEXT);
                return -1;
            }
            // Получения имени
            for (int numberChar = 0; numberChar<sizeString+1; numberChar++)
            {
                if (string[numberChar] == '<' || string[numberChar] == '>')
                {
                    location = !location;
                    if (string[numberChar] == '>')
                    {
                        location = 0;
                        outputLineIndex = 0;
                        break;
                    }
                }
                else if (location)
                {
                    nameVar[outputLineIndex++] = string[numberChar];
                    nameVar[outputLineIndex] = '\0';
                }
            }
            
            for (int i = 0; i < *varsQuantity; i++)
            {
                if (strcmp(vars[i], nameVar) == 0)
                {
                    if ((varsMode[i] == 1 || varsMode[i] == 11) && funcMode==0)
                    {
                        printf("%sError at line %d: Variable \"%s\" cannot be returned because it is a string and function \"%s\" returns a number.%s", RED_COLOR_TEXT, (line+1), nameVar, functions[*functionsQuantity-1], RESET_COLOR_TEXT);
                        free(nameVar);
                        fclose(file);
                        return -1;
                    }
                    if ((varsMode[i] == 0 || varsMode[i] == 10) && funcMode == 1)
                    {
                        printf("%sError at line %d: The variable \"%s\" cannot be returned because it is a number and the function \"%s\" returns a string.%s", RED_COLOR_TEXT, (line+1), nameVar, functions[*functionsQuantity-1], RESET_COLOR_TEXT);
                        free(nameVar);
                        fclose(file);
                        return -1;
                    } 
                    addToFile(file, "return ");
                    addToFile(file, nameVar);
                    addToFile(file, ";\n");
                    free(nameVar);
                    fclose(file);
                    return 0;
                }   
            }
            free(nameVar);
            printf("%sError in line %d: Variable named %s is not declared.%s", RED_COLOR_TEXT, (line+1), nameVar, RESET_COLOR_TEXT);
            return -1;
        }
   
        else
        {
            printf("%sError at line %d: Function \"%s\" cannot return what you specified.%s", RED_COLOR_TEXT, (line+1), functions[*functionsQuantity-1], RESET_COLOR_TEXT);
            return -1;
        }
    }
     
    else
    {
        if(command==NULL) strncpy(command,string, 15);
        printf("%sError reading \"%s\" on line %d.%s\n", RED_COLOR_TEXT,  command, (line + 1), RESET_COLOR_TEXT);
        return -1;
    }
    
    if (*entryFR==2) fclose(file);

    return 0;
}

#endif