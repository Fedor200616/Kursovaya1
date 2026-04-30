#include "Main.h"
#include "Reader.h"

int main()
{
    SetConsoleCP(1251);       // входная кодировка
    SetConsoleOutputCP(1251); // выходная кодировка
    setlocale(LC_ALL, "");

	fs::path filePath = OpenFileDialog();
	std::cout << "Selected file path: " << filePath << std::endl;
	std::vector<string_info> fileLines = CopyStringFromFile(filePath);
	AnaliseIterator(fileLines);
	for (auto i : fileLines) {
		std::cout << i.line << ' ' << i.str << '\n' <<
			i.brackets << ' ' << i.have_comment << '\n';
	}
	std::cout << fileLines.back().brackets << '\n';
    return 0;
}

