#pragma once
#include <iostream>
#include "main.h"

#ifndef ANALYSE_H
#define ANALYSE_H

const char open_brackets[] = "({[<";
const char close_brackets[] = ")}]>";
const char quotes[] = "\"\'";
const char comment[] = "/";
const char long_comment[] = "/*";
const char long_comment_end[] = "*/";
const char eos[] = ";";

/// <summary>
/// Анализатор скобок в скроке
/// </summary>
/// <param name="str_info">Инофрмацию о строке для изменения</param>
/// <returns>ХЗ</returns>
std::string BracketChecker(const string_info str_info); // Мы это испоьзуем поолсе проверок посимвольно

/// <summary>
/// Проверка на незакрытые ковычки.
/// </summary>
/// <param name="str_info"></param>
/// <returns>1 - если одинарные кавычки не закрыты, 2 - двойные, 0 - иначе</returns>
int UnclosedquoteChecker(const string_info str_info);

/// <summary>
/// Проверка на незакрытый многострочный комментарий.
/// </summary>
/// <param name="str_info"></param>
/// <returns>1 - имеется, 0 - иначе</returns>
int UnclosedlongcommentChecker(const string_info str_info);

/// <summary>
/// Проверка на конец строки. В нашем случае - на наличие ';' в строке.
/// </summary>
/// <param name="ch"></param>
/// <returns>1 - иммется, 0 - иначе</returns>
int NotEosChecker(const string_info str_info);

/// <summary>
/// Проверка строки на наличие в строке каких-либо комментариев
/// </summary>
/// <param name="str_info">информация о строке</param>
/// <returns></returns>
bool CommentChecker(const string_info str_info);

//==================================================================================================
//==================================================================================================
//==================================================================================================

/// <summary>
/// Проверка на открытые скобки. 
/// </summary>
/// <param name="ch">Символ на проверке</param>
/// <returns>Если символ является открытой скобкой, то возвращает true, иначе false.</returns>
bool IsOpenBracket(char ch);

/// <summary>
/// Проверка на закрытые скобки.
/// </summary>
/// <param name="ch">Символ на проверку</param>
/// <returns>Если символ - закрытая скобка - true, иначе false</returns>
bool IsCloseBracket(char ch);

/// <summary>
/// Проверка на любой тип скобок.
/// </summary>
/// <param name="ch">Символ на проверку</param>
/// <returns>Если символ - скобка - true, иначе false</returns>
bool IsBracket(char ch); // Проверка на скобки в целом, для удобства

/// <summary>
/// Проверка на кавычки.
/// </summary>
/// <param name="ch">Символ на проверке</param>
/// <returns>Если символ является кавычкой, то возвращает true, иначе false.</returns>
bool IsQuote(char ch);

/// <summary>
/// Проверка на комментарий.
/// </summary>
/// <param name="ch">Символ на проверке</param>
/// <returns>Если символ является кавычкой, то возвращает true, иначе false.</returns>
bool IsComment(char ch);


#endif // ANALYSE_H
