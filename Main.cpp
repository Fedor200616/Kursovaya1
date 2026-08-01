#include "Main.h"
#include "Reader.h"
#include "Chrono.h"
#include "PrintErr.h"
#include "Analyse.h"
#include "User.h"

std::filesystem::path exe_filepath;

std::vector<string_info> fileLines;
std::vector<err_info> errors; 

int main(int argc, char* argv[])
{
	if (argc > 0)
	{
		exe_filepath = argv[0];
	}

	SetConsoleCP(1251);       // входная кодировка
    SetConsoleOutputCP(1251); // выходная кодировка
    setlocale(LC_ALL, "");
	unsigned char user_index = 0;
	unsigned char complite_mask = 0xE0; // Маска для проверки завершенности всех параметров (11100000)
	do {
		user_index = menu_navigation();
		switch (user_index) {
		case SelectFile: // Выбор файла
			setting.filepath = OpenFileDialog();
			if (setting.filepath.empty()) {
				menu_enter_param &= ~0X80; // Сбрасываем бит для выбора файла
			}
			else {
				menu_enter_param |= 0X80; // Устанавливаем бит для выбора файла
				fileLines = CopyStringFromFile(setting.filepath); // Считываем файл в вектор
			}
			break;
		case SetPercent: // Процент комментариев
			setting.ref_percent = GetUserInfo(setting.PERCENT_DIFF, setting.percent_dialog, setting.PERCENT_RANGE, setting.ref_percent);
			menu_enter_param |= 0X40; // Устанавливаем бит для процента комментариев
			break;
		case SetInterval: // Интервал коммов
			setting.ref_interval = GetUserInfo(setting.INTERVAL_DIFF, setting.interval_dialog, setting.INTERVAL_RANGE, setting.ref_interval);
			menu_enter_param |= 0X20; // Устанавливаем бит для интервала коммов
			break;
		case Continue: // Продолжить
			if (complite_mask & menu_enter_param) {
				system("cls");
				auto start = chrono();
				AnaliseIterator(fileLines); //Reader.cpp
				auto end = chrono();
				system("cls");
				std::cout << "Процесс выполнен за " << chrono_diff(start, end) << " секунды" << '\n';

				ReturnResult(fileLines, errors, setting.ref_percent, setting.ref_interval, setting.filepath);
			}
			else {
				system("cls");
				std::cout << "Пожалуйста, заполните все параметры перед продолжением.\n";
				(void)_getch();
			}
			break;
		case Exit: // Выход
			std::exit(0);
			break;
		default:
			break;
		}
	} 
	while (user_index != Exit);

	return 0;
}