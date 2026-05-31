#include "Main.h"
#include "Reader.h"
#include "Chrono.h"
#include "PrintErr.h"
#include "Analyse.h"
#include "User.h"

std::vector<string_info> fileLines;
std::vector<err_info> errors;

int main()
{
    SetConsoleCP(1251);       // входная кодировка
    SetConsoleOutputCP(1251); // выходная кодировка
    setlocale(LC_ALL, "");

	main_func(); //User.cpp

	
    return 0;
}

