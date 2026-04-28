#pragma once

#include <iostream>
#include <filesystem>
#include <windows.h>
#include <commdlg.h>
#include <fstream>
#include <vector>
#include <string>

#ifndef MAIN_H
#define MAIN_H

namespace fs = std::filesystem;

/// <summary>
/// Структура информации о строке:
/// номер строки, 
/// сама строка, 
/// скобки в строках до нее, 
/// наличие незакрытых кавычек в строке, 
/// наличие комментария в строке, 
/// наличие незакрытого многострочного комментария в строке, 
/// </summary>
struct string_info {
    int str_num;
    std::string str;
    std::string brackets;
    int have_unclosedquote;
    int have_comment;
    int have_long_comment;
};

/// <summary>
/// Открывает окно выбора файла через проводник Windows
/// </summary>
/// <returns>Путь к выбранному файлу</returns>
fs::path OpenFileDialog();

/// <summary>
/// Считывает файл построчно и сохраняет строки в вектор
/// </summary>
/// <param name="filePath">Путь к файлу для чтения</param>
/// <returns>Вектор структур с информацией о каждой строке</returns>
std::vector<string_info> CopyStringFromFile(const fs::path filePath);

/// <summary>
/// Анализируем строку и возвращаем структуру с информацией о ней
/// </summary>
/// <param name="str_info">Информация о ПРЕДЫДЫДУЩЕЙ строке</param>
/// <returns></returns>
string_info GetStringInfo(const string_info& str_info);

#endif

