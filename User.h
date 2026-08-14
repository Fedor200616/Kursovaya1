#pragma once

#include <iostream>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <filesystem>
#include "Main.h"
#include "PrintErr.h"

namespace fs = std::filesystem;

struct Settings { // Структура настроек
	fs::path filepath = ""; // Путь к проверяемому файлу

	int ref_percent = 10; // Минимальный процент комментариев
	int ref_interval = 20; // Интервал проверки количества комментариев

	const int PERCENT_DIFF = 5; // Шаг выбора процента комментариев
	const int PERCENT_RANGE[2] = { 0, 100 }; // Диапазон выбора процента комментариев
    const std::string percent_dialog = "Введите минимальный процент комментариев (от " + std::to_string(PERCENT_RANGE[0]) + " до " + std::to_string(PERCENT_RANGE[1]) + "): ";
	const int INTERVAL_DIFF = 2;
	const int INTERVAL_RANGE[2] = { 9, 41 };
	const std::string interval_dialog = "Введите интервал комментариев (от " + std::to_string(INTERVAL_RANGE[0]) + " до " + std::to_string(INTERVAL_RANGE[1]) + "): ";
};
inline Settings setting;

/// <summary>
/// Функция отображения меню с возможностью выбора пользователем параметров
/// </summary>
/// <param name="act_index">Индекс текущего выбранного элемента</param>
/// <param name="set">Структура настроек</param>
/// <param name="menu">Массив строк меню</param>
/// <param name="size">Размер массива меню</param>
void show_menu(unsigned char act_index, Settings set, const std::string menu[], const int size);

/// <summary>
/// Функция навигации в меню
/// </summary>
/// <returns>Выбранный элемент меню</returns>
unsigned char menu_navigation();

/// <summary>
/// Функция выбора пользователем пределов в оценке
/// </summary>
/// <param name="DIFF">Шаг выбора</param>
/// <param name="text">Текст для сообщения пользователю</param>
/// <param name="interval">интервал, внутри которого пользователь может выбирать значения</param>
/// <returns>выбранное пользователем значение</returns>
int GetUserInfo(const int DIFF, const std::string text, const int* interval, int user_enter);

/// <summary>
/// Функция выбора пользователем действии для вывода результата
/// </summary>
/// <param name="arg_num">число параметров, доступных пользователю</param>
/// <returns>выбор пользователя</returns>
int GetUserOpinion(int arg_num);

/// <summary>
/// Функция вызова окна сохранения файла через проводник Windows
/// </summary>
/// <returns></returns>
fs::path SaveFileDialog(const fs::path& filepath);

/// <summary>
/// Функция выбора пользователем места для сохранения файла с результатами, предлагает 3 варианта: сохранить в папке с проверяемым файлом, сохранить в папке по умолчанию (рабочая папка программы), выбрать папку для сохранения через проводник Windows
/// </summary>
/// <param name="filepath">Путь к проверяемому файлу</param>
/// <returns>путь расположения тектового файла отчета</returns>
fs::path place_to_save(const fs::path& filepath);

/// <summary>
/// Функция печати результатов
/// </summary>
/// <param name="fileLines">Информация о файле</param>
/// <param name="errorInfo">Информация о полученных ошибках</param>
/// <param name="ref_percent">Минимальный процент комментариев</param>
/// <param name="ref_interval">Интервал проверок</param>
/// <param name="filepath">Путь к проверяемому файлу</param>
void ReturnResult(const std::vector<string_info>& fileLines, const std::vector<err_info>& errorInfo, const int ref_percent, const int ref_interval, const fs::path& filepath);
