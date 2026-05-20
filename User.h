#pragma once
#include <iostream>
#include <vector>
#include <conio.h>
#include <filesystem>
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

struct Settings {
	int ref_percent = 10;
	int ref_inteval = 20;

	const int PERCENT_DIFF = 5;
	const int PERCENT_RANGE[2] = { 0, 100 };
	const int INTERVAL_DIFF = 2;
	const int INTERVAL_RANGE[2] = { 9, 41 };
};

extern Settings setting;

/// <summary>
/// Функция выбора пользователем пределов в оценке
/// </summary>
/// <param name="DIFF">Шаг выбора</param>
/// <param name="text">Текст для сообщения пользователю</param>
/// <param name="interval">интервал, внутри которого пользователь может выбирать значения</param>
/// <returns>выбранное пользователем значение</returns>
int GetUserInfo(const int DIFF, const std::string text, const int *interval, int user_enter);

/// <summary>
/// Функция выбора пользователем действии для вывода результата
/// </summary>
/// <param name="arg_num">число параметров, доступных пользователю</param>
/// <returns>выбор пользователя</returns>
int GetUserOpinion(int arg_num);

/// <summary>
/// Функция печати результатов
/// </summary>
/// <param name="fileLines">Информация о файле</param>
/// <param name="errorInfo">Информация о полученных ошибках</param>
/// <param name="ref_percent">Минимальный процент комментариев</param>
/// <param name="ref_interval">Интервал проверок</param>
/// <param name="filepath">Путь к проверяемому файлу</param>
void ReturnResult(const std::vector<string_info>& fileLines, const std::vector<err_info>& errorInfo, const int ref_percent, const int ref_interval, const std::filesystem::path& filepath);

