#include "Main.h"
#include "Reader.h"
#include "Chrono.h"
#include "PrintErr.h"
#include "Analyse.h"
#include "User.h"

std::vector<string_info> fileLines;

int main()
{
    SetConsoleCP(1251);       // входная кодировка
    SetConsoleOutputCP(1251); // выходная кодировка
    setlocale(LC_ALL, "");

	main_func();
	
	//for (auto i : fileLines) {
	//	std::cout << i.line << ' ' << i.str << '\n' <<
	//		i.brackets << ' ' << i.have_comment << '\n';
	//}
	
    return 0;
}

