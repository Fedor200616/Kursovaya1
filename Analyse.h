#pragma once
#include <iostream>
#include <vector>
#include "main.h"
#include "PrintErr.h"

#ifndef ANALYSE_H
#define ANALYSE_H

inline const char quotes[] = "\"\'";
inline const char comment = '/';
inline const char long_comment = '*';
inline const char long_comment_end[] = "*/";

/// <summary>
/// Функция анализа строки файла
/// </summary>
/// <param name="prev">Строка перед анализируемой, дает контекст для анализа</param>
/// <param name="str_info">строка, которую анализируем</param>
void analyse(const string_info& prev, string_info& str_info);

/// <summary>
/// Проверка скобок
/// </summary>
/// <param name="str_info">Информация о строке</param>
/// <param name="bracket">Информация о скобке</param>
void BracketChecker(string_info& str_info, const brack bracket);

/// <summary>
/// Копирует нужные в str_info параметры из prev
/// </summary>
/// <param name="prev">предыдущая строка(для копирования)</param>
/// <param name="str_info">строка для вставки</param>
void recent(const string_info& prev, string_info& str_info); //не копирует номер строки,

/// <summary>
/// проверка на комментарии
/// </summary>
/// <param name="first">Первый символ на проверку</param>
/// <param name="second">Второй символ на проверке</param>
/// <returns>0 - нет коммента; 1 - однострочечный коммент; 2 - многострочечный коммент</returns>
inline int CommentChecker(unsigned char first, unsigned char second);

/// <summary>
/// Проверка на любой тип скобок.
/// </summary>
/// <param name="ch">Символ на проверку</param>
/// <returns>Если символ - скобка - true, иначе false</returns>
inline bool IsBracket(unsigned char ch); // Проверка на скобки в целом, для удобства

//==================================================================================================
//==================================================================================================
//==================================================================================================

/// <summary>
/// Проверка на открытые скобки. 
/// </summary>
/// <param name="ch">Символ на проверке</param>
/// <returns>Если символ является открытой скобкой, то возвращает true, иначе false.</returns>
inline bool IsOpenBracket(unsigned char ch);

/// <summary>
/// Проверка на закрытые скобки.
/// </summary>
/// <param name="ch">Символ на проверку</param>
/// <returns>Если символ - закрытая скобка - true, иначе false</returns>
inline bool IsCloseBracket(unsigned char ch);

/// <summary>
/// Проверка на любой тип скобок.
/// </summary>
/// <param name="ch">Символ на проверку</param>
/// <returns>Если символ - скобка - true, иначе false</returns>
inline bool IsBracket(unsigned char ch); // Проверка на скобки в целом, для удобства

/// <summary>
/// Функция проверки что две скобки одного вида
/// </summary>
/// <param name="open">открывающаяся скобка</param>
/// <param name="close">закрывающаяся скобка</param>
/// <returns>1 - ели одного вида, 0 - иначе</returns>
inline bool BracketCompare(unsigned char open, unsigned char close);

/// <summary>
/// Проверка на кавычки.
/// </summary>
/// <param name="ch">Символ на проверке</param>
/// <returns>Если символ является кавычкой, то возвращает true, иначе false.</returns>
inline bool IsQuote(unsigned char ch);

/// <summary>
/// Первая проверка ошибки закрытой скобки: на наличие подобной открытой в буфере
/// </summary>
/// <param name="bracket_buff">Буфер скобок из strin_info</param>
/// <param name="close">Скобка, которую проверяем</param>
/// <returns>1 - если в сткроке есть подобная открывающаяся скобка, ноль, если не было открытых скобок такого типа до этого</returns>
inline bool HaveSimOpenBrack(const std::vector<brack>& bracket_buff, const unsigned char close);

/// <summary>
/// Проверка, что символ не может быть внутри C++ кода
/// </summary>
/// <param name="ch">Символ</param>
/// <returns>1 если символ $ @ ` или кирилица</returns>
inline bool IsInvalidChar(const unsigned char& ch);

/// <summary>
/// Находим строку с открытой скобкой, которая не была закрыта
/// </summary>
/// <param name="str_info">информация о строке, в которой найдена некорректная закрытая скобка</param>
/// <param name="Lines">информация о всем файле, в данной программе выставляется по умолчание на глобальную переменную</param>
/// <returns></returns>
err_info FindErrUnCloseBrack(const string_info& str_info, const std::vector<string_info>& Lines = fileLines);


/// <summary>
/// Находит строки, с процентов комментов ниже заданного
/// </summary>
/// <param name="Info">Информация о строках</param>
/// <param name="ref_percent">Заданный процент коментариев</param>
/// <param name="interval">Интервал, внутри которого осуществялются проверки</param>
/// <returns>массив чисел - нумера интервалов</returns>
std::vector<comm_percent> CommPercent(const std::vector<string_info>& Info, const int ref_percent, const int interval);

/// <summary>
/// Нахождение незакрытых скобок в конце
/// </summary>
/// <param name="info">информация о строках</param>
void FindEndBrackets(const std::vector<string_info>& info);


#endif // ANALYSE_H
