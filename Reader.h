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
std::vector<string_info> CopyStringFromFile(const fs::path& filePath);

/// <summary>
/// Функция цикла для анализа кода по
/// </summary>
/// <param name="info"></param>
/// <returns></returns>
int AnaliseIterator(std::vector<string_info>& info);

#endif // READER
