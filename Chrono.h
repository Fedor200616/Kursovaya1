#include <chrono>
#include <iostream>

#ifndef CHRONO_H
#define CHRONO_H

/// <summary>
/// Функция запуска таймера
/// </summary>
/// <returns>точку времени (timestamp) типа std::chrono::high_resolution_clock::time_point</returns>
std::chrono::steady_clock::time_point chrono();

inline double chrono_diff(auto start, auto end) {
    return std::chrono::duration<double>(end - start).count();
}

#endif // CHRONO_H