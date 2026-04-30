#pragma once
#include <iostream>
#include "main.h"

#ifndef ANALYSE_H
#define ANALYSE_H

const char open_brackets[] = "({[";
const char close_brackets[] = ")}]";
const char brackets[] = "({[<)}]>";
const char quotes[] = "\"\'";
const char comment = '/';
const char long_comment = '*';
const char long_comment_end[] = "*/";
const char eos = ';';


/// <summary>
/// Основная функция проверки строки
/// </summary>
/// <param name="str_info"></param>
/// <returns></returns>
string_info analyse(string_info& str_info);

/// <summary>
/// Проверка скобок
/// </summary>
/// <param name="str_info">Информация о строке</param>
/// <param name="bracket">Скобка</param>
void BracketChecker(string_info& str_info, const char bracket);

/// <summary>
/// проверка на комментарии
/// </summary>
/// <param name="first">Первый символ на проверку</param>
/// <param name="second">Второй символ на проверке</param>
/// <returns>0 - нет коммента; 1 - однострочечный коммент; 2 - многострочечный коммент</returns>
int CommentChecker(char first, char second);

/// <summary>
/// Проверка на кавычки.
/// </summary>
/// <param name="ch">Символ на проверке</param>
/// <returns>Если символ является кавычкой, то возвращает true, иначе false.</returns>
bool IsQuote(char ch);

/// <summary>
/// Проверка на любой тип скобок.
/// </summary>
/// <param name="ch">Символ на проверку</param>
/// <returns>Если символ - скобка - true, иначе false</returns>
bool IsBracket(char ch); // Проверка на скобки в целом, для удобства

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
/// Функция проверки что две скобки одного вида
/// </summary>
/// <param name="open">открывающаяся скобка</param>
/// <param name="close">закрывающаяся скобка</param>
/// <returns>1 - ели одного вида, 0 - иначе</returns>
bool BracketCompare(char open, char close);

/// <summary>
/// Проверка на кавычки.
/// </summary>
/// <param name="ch">Символ на проверке</param>
/// <returns>Если символ является кавычкой, то возвращает true, иначе false.</returns>
bool IsQuote(char ch);


#endif // ANALYSE_H
