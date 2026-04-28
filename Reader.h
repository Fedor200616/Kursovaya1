#pragma once

#include <iostream>
#include <filesystem>
#include <windows.h>
#include <commdlg.h>
#include <fstream>
#include <vector>
#include <string>

namespace fs = std::filesystem;

#ifndef READER
#define READER

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
/// Анализируем строку и возвращаем структуру с информацией о ней.
/// На входе имеем информацию о предыдущей строке, на выходе - информацию о текущей строке
/// </summary>
/// <param name="str_info">Информация о ПРЕДЫДЫДУЩЕЙ строке</param>
/// <returns></returns>
string_info GetStringInfo(const string_info str_info);

#endif // READER
