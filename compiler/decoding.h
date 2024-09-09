/*
    Copyright (C) ZerTeam.
*/
#ifndef DECODING_FILE_ZRC
#define DECODING_FILE_ZRC
#define CONTENT_VAR_SIZE   401
#define NAME_VAR_SIZE      50
#define SIZE_TEXT_CONTENT  552
#if defined(_WIN32) || defined(_WIN64)
    #define RED_COLOR_TEXT     ""
    #define RESET_COLOR_TEXT   ""
#else
    #define RED_COLOR_TEXT     "\033[1;31m"
    #define RESET_COLOR_TEXT   "\033[0m"
#endif

#define _INPUT_ZRC "Ext_C_Lib/_input_.zrc"

#include "working_with_files.h"
#include "working_with_text.h"
#include <string.h>
#include <stdlib.h>

typedef unsigned short u_short;
typedef char *  p_char;

int decodingString(p_char string, u_short line, FILE *file, char *pathFileC, u_short sizeString, char Vars[5000][1201], short *VarsMode, unsigned int *VarsQuantity, unsigned short *libraries)
{
    char command[16] = "";
    command[16] = '\0';
    int location = 0;
    int outputLineIndex = 0;

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
    // Проверка существования команды
    if(strchr(string, ':')==NULL) {
        char *tempContent = strrchr(string, '\n');
        *tempContent = '\0';
        printf("%sError reading \"%s\" on line %d.%s\n", RED_COLOR_TEXT, string, (line + 1), RESET_COLOR_TEXT);        
        return -1;
        }
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
                textContent[outputLineIndex] = *"\0";
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
                textContent[outputLineIndex] = *"\0";
            }
        }

        libraries[0]=1;

        addToFile(file, "printf(\"%%s\", \"");
        addToFile(file, textContent);
        addToFile(file, "\");\n");
    }
    
    else if (strchr(string, '<')!=NULL&&strchr(string, '>')!=NULL)
    {
        
        char varName[NAME_VAR_SIZE] = "\0";
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
                varName[outputLineIndex] = *"\0";
            }
        }
        for (int i = 0; i < *VarsQuantity; i++)
        {
            if (strcmp(Vars[i],varName) == 0)
            {
                if (VarsMode[i]==0)
                {
                    libraries[0]=1;
                    addToFile(file, "printf(\"%%Lf\", ");
                    addToFile(file, Vars[i]);
                    addToFile(file, ");\n");
                }
                if (VarsMode[i]==1)
                {
                    libraries[0]=1;
                    addToFile(file, "printf(\"%%s\", ");
                    addToFile(file, Vars[i]);
                    addToFile(file, ");\n");
                } 
            }   
        }
    }
    else
    {
        printf("%sError on line %d: backtick(`) not found.%s\n", RED_COLOR_TEXT, (line + 1), RESET_COLOR_TEXT);
        return -1;
    }
        return 0;
    }
    // input
    else if (!strcmp(command, "input"))
    {
        char textContent[SIZE_TEXT_CONTENT] = "\0";

        if (strchr(string, '<')!=NULL&&strchr(string, '>')!=NULL)
    {
        
        char varName[NAME_VAR_SIZE] = "\0";
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
                varName[outputLineIndex] = *"\0";
            }
        }
        
        for (int i = 0; i < *VarsQuantity; i++)
        {
            if (strcmp(Vars[i], varName) == 0)
            {
                libraries[3]=1;
                
                if (VarsMode[i]==0)
                {
                    addToFile(file, "inputN(&");
                    addToFile(file, Vars[i]);
                    addToFile(file, ");\n");
                }
                if (VarsMode[i]==1)
                {
                    addToFile(file, "inputT(");
                    addToFile(file, Vars[i]);
                    addToFile(file, ", sizeof(");
                    addToFile(file, Vars[i]);
                    addToFile(file, ")-1);\n");
                } 
            }   
        }
    }
        
        else
        {
            printf("%sError on line %d: backtick(`) not found.%s", RED_COLOR_TEXT, (line + 1), RESET_COLOR_TEXT);
            return -1;
        }
        return 0;
    }
    // var
    else if (!strcmp(command, "var"))
    {
        
        u_short dataVarSize = 0;
        char nameVar[NAME_VAR_SIZE] = "";
        char contentVar[CONTENT_VAR_SIZE] = "";
        u_short mode; // 1 цифры ; 2 цифры с запятой; вторая цифра 1 значит что нет - (пример 11, 21); 3 строка
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
            addToFile(file, " = ");
            addToFile(file, contentVar);
            addToFile(file, ";\n");
            VarsMode[*VarsQuantity]=mode;
            strcpy(Vars[*VarsQuantity], nameVar);
            *VarsQuantity+=1;
        } else if (mode==1)
        {
            addToFile(file, "char ");
            addToFile(file, nameVar);
            addToFile(file, "[1201] = \"");
            addToFile(file, contentVar);
            addToFile(file, "\\0\";\n");
            VarsMode[*VarsQuantity]=mode;
            strcpy(Vars[*VarsQuantity], nameVar);
            *VarsQuantity+=1;
        }
    }
    // const
    else if (!strcmp(command, "const"))
    {
        
        u_short dataVarSize = 0;
        char nameVar[NAME_VAR_SIZE] = "";
        char contentVar[CONTENT_VAR_SIZE] = "";
        u_short mode; // 1 строка;0 число с зап
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
            VarsMode[*VarsQuantity]=mode;
            strcpy(Vars[*VarsQuantity], nameVar);
            *VarsQuantity+=1;
        } else if (mode==1)
        {
            addToFile(file, "const char ");
            addToFile(file, nameVar);
            addToFile(file, "[1201] = \"");
            addToFile(file, contentVar);
            addToFile(file, "\\0\";\n");
            VarsMode[*VarsQuantity]=mode;
            strcpy(Vars[*VarsQuantity], nameVar);
            *VarsQuantity+=1;
        }
    }
    
    else
    {
        if(command==NULL) strncpy(command,string, 15);
        printf("%sError reading \"%s\" on line %d.%s\n", RED_COLOR_TEXT,  command, (line + 1), RESET_COLOR_TEXT);
        return -1;
    }
    
    return 0;
}

#endif