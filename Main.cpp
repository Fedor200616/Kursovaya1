#include "Main.h"
#include "Reader.h"
#include "Chrono.h"
#include "PrintErr.h"
#include "Analyse.h"

std::vector<string_info> fileLines;

int main()
{
    SetConsoleCP(1251);       // входная кодировка
    SetConsoleOutputCP(1251); // выходная кодировка
    setlocale(LC_ALL, "");

	fs::path filePath = OpenFileDialog();
	std::cout << "Selected file path: " << filePath << std::endl;
	auto start = chrono();
	fileLines = CopyStringFromFile(filePath);
	AnaliseIterator(fileLines);
	//for (auto i : fileLines) {
	//	std::cout << i.line << ' ' << i.str << '\n' <<
	//		i.brackets << ' ' << i.have_comment << '\n';
	//}
	auto end = chrono();
	std::cout << chrono_diff(start, end) << '\n';
	std::cout << fileLines.back().brackets << '\n';

	print_error();
    return 0;
}

