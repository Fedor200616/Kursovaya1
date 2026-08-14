#include "Main.h"
#include "Reader.h"
#include "Chrono.h"
#include "PrintErr.h"
#include "Analyse.h"
#include "User.h"
#include "Menu.h"

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
	bool exit_program = false;

	MenuOut MainMenu{
		.Menu = {
			"Файл для анализа ",
			"Процент комментариев: ",
			"Интервал оценивания: ",
			"Продолжить",
			"Выход"
		},
		.MenuParam = {
			[]() {return setting.filepath.string();},
			[]() {return std::to_string(setting.ref_percent) + "%";},
			[]() {return std::to_string(setting.ref_interval);},
			[]() {return "";},
			[]() {return "";}
		},
			.PostMenuMessage = "Используйте стрелки для навигации по меню, Enter для выбора, Esc для выхода.",
			.MenuEnterParam = 0b01100000,
			.MenuOutParam = 0b10001000,
	};
	MainMenuLogic MainLogic;
	do {
		MainMenuAction action = menu_navigation(MainMenu, MainLogic); // Запускаем меню

		switch (action) {
		case MainMenuAction::OpenFile: // Выбор файла
			setting.filepath = OpenFileDialog();
			if (setting.filepath.empty()) {
				MainMenu.MenuEnterParam &= ~0X80; // Сбрасываем бит для выбора файла
			}
			else {
				MainMenu.MenuEnterParam |= 0X80; // Устанавливаем бит для выбора файла
				fileLines = CopyStringFromFile(setting.filepath); // Считываем файл в вектор
			}
			break;
		case MainMenuAction::SetPercent: // Процент комментариев
			setting.ref_percent = GetUserInfo(setting.PERCENT_DIFF, setting.percent_dialog, setting.PERCENT_RANGE, setting.ref_percent);
			MainMenu.MenuEnterParam |= 0X40; // Устанавливаем бит для процента комментариев
			break;
		case MainMenuAction::SetInterval: // Интервал комментариев
			setting.ref_interval = GetUserInfo(setting.INTERVAL_DIFF, setting.interval_dialog, setting.INTERVAL_RANGE, setting.ref_interval);
			MainMenu.MenuEnterParam |= 0X20; // Устанавливаем бит для интервала комментариев
			break;
		case MainMenuAction::Continue: // Продолжить
			if (complite_mask & MainMenu.MenuEnterParam) {
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
		case MainMenuAction::Exit: // Выход
			exit_program = true;
			std::exit(0);
			break;
		default:
			break;
		}

	} 
	while (!exit_program);

	return 0;
}