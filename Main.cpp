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
	for (int i = 0; i < fileLines.size(); i++) {
		std::cout << "Line " << fileLines[i].str_num << ": " << fileLines[i].str << std::endl;
	}
    return 0;
}

