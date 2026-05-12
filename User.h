#pragma once
#include <iostream>
#include <vector>
#include <conio.h>
#include "Main.h"
#include "PrintErr.h"

enum class key {
	Enter = 13,
	Esc = 27,
};

/// <summary>
/// Основная функция взаимодействия с пользователем
/// </summary>
/// <returns></returns>
int main_func();

/// <summary>
/// Функция спрашивает у пользователя, вернуться ли ему
/// </summary>
/// <param name="echo"></param>
/// <returns></returns>
key next_or_back(std::string echo);

const int PERCENT_DIFF = 5;
const int PERCENT_RANGE[2] = { 0, 100 };
const int INTERVAL_DIFF = 2;
const int INTERVAL_RANGE[2] = { 9, 41 };
int GetUserInfo(const int DIFF, const std::string text, const int *interval);

/// <summary>
/// Выводит на экран интервалы с малым количеством комментариев
/// </summary>
/// <param name="intervals">массив интервалов для вывода</param>
/// <param name="interval_size">размер интервала</param>
/// <param name="fileinfosize">длина файла</param>
void CommPercentPrint(std::vector<int> intervals, int interval_size, int fileinfosize);

int GetUserOpinion(int arg_num);

/// <summary>
/// Функция печати результата
/// </summary>
/// <param name="fileLines">вектор информации о файле</param>
void ReturnResult(const auto& fileLines, const std::vector<err_info>& errorInfo, const int comm_percent, const int comm_interval);

