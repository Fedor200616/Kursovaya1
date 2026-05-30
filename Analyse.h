#pragma once
#include <iostream>
#include <vector>
#include "main.h"
#include "PrintErr.h"

#ifndef ANALYSE_H
#define ANALYSE_H


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



#endif // ANALYSE_H
