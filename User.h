#pragma once

#include <iostream>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <filesystem>
#include "Main.h"
#include "PrintErr.h"

namespace fs = std::filesystem;

extern unsigned char menu_enter_param;
extern unsigned char menu_out;
enum MenuAction : unsigned char
{
	SelectFile = 0x80,
	SetPercent = 0x40,
	SetInterval = 0x20,
	Continue = 0x10,
	Exit = 0x08
};

enum class key {
	Up = 72,
	Down = 80,
	Left = 75,
	Right = 77,
	Enter = 13,
	Esc = 27,
	Utility = 224,
	Null = 0,
};
inline key int_to_key(int int_key) {
	switch (int_key) {
	case 72:
		return key::Up;
	case 80:
		return key::Down;
	case 75:
		return key::Left;     
	case 77:
		return key::Right;    
	case 13:
		return key::Enter;
	case 27:
		return key::Esc;
	case 224:
		return key::Utility;  
	default:
		return key::Null;
	}
}

struct Settings {
	fs::path filepath = "";

	int ref_percent = 10;
	int ref_interval = 20;

	const int PERCENT_DIFF = 5;
	const int PERCENT_RANGE[2] = { 0, 100 };
    const std::string percent_dialog = "Введите минимальный процент комментариев (от " + std::to_string(PERCENT_RANGE[0]) + " до " + std::to_string(PERCENT_RANGE[1]) + "): ";
	const int INTERVAL_DIFF = 2;
	const int INTERVAL_RANGE[2] = { 9, 41 };
	const std::string interval_dialog = "Введите интервал комментариев (от " + std::to_string(INTERVAL_RANGE[0]) + " до " + std::to_string(INTERVAL_RANGE[1]) + "): ";
};
inline Settings setting;


void show_menu(unsigned char act_index, Settings set, const std::string menu[], const int size);

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
