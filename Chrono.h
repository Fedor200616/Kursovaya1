#pragma once
#include <chrono>
#include <iostream>

#ifndef CHRONO_H
#define CHRONO_H

/// <summary>
/// Функция запуска таймера
/// </summary>
/// <returns>точку времени (timestamp) типа std::chrono::high_resolution_clock::time_point</returns>
auto crono();

double chrono_diff(auto start, auto end);

#endif // CHRONO_H