#include "User.h"
#include "Main.h"
#include "Reader.h"
#include "Chrono.h"
#include "PrintErr.h"
#include "Analyse.h"

const int MENU_SIZE = 5;
const std::string act_mark = "->";
const std::string inact_mark = "  ";
const std::string MENU[MENU_SIZE] = {
	"Выбрать файл для анализа ",
	"Процент комментариев: ",
	"Интервал оценивания: ",
	"Продолжить",
	"Выход"
};
unsigned char menu_enter_param = 0b01100000; //изначально неизвестен файл для анализа, но известны процент и интервал по умолчанию
unsigned char menu_out = 0b10001000; //изначально выводим только "Выбрать файл для анализа" и "Выход"



void show_menu(unsigned char act_index, Settings set, const std::string menu[], const int size) {
	system("cls");
	std::ostringstream out_buffer;
	if (menu_enter_param & 0x80) { // Если файл для анализа выбран, то выводим все пункты меню
		menu_out |= 0x78; // Устанавливаем биты для всех пунктов меню
	}
	for (int i = 0; i < size; i++) {
		unsigned char mask = 0x80 >> i; // Маска для проверки заданности параметра
		if (menu_out & mask) {
			if (mask & act_index) {
				out_buffer << act_mark << menu[i]; // Выводим типа ->Выбрать файл для анализа
			}
			else {
				out_buffer << inact_mark << menu[i];
			}
			bool is_param_set = menu_enter_param & mask; // Проверяем, установлен ли параметр
			if (i == 0 && is_param_set) {
				out_buffer << set.filepath; // Добавляем к строке путь к файлу
			}
			else if (i == 1 && is_param_set) {
				out_buffer << set.ref_percent << "%"; // Добавляем к строке процент или интервал
			}
			else if (i == 2 && is_param_set) {
				out_buffer << set.ref_interval;
			}
			out_buffer << '\n';
		}
	}
	std::cout << out_buffer.str() << std::endl;
}

unsigned char menu_navigation() {
	static unsigned char index = 0x80;
	while (true)
	{
		show_menu(index, setting, MENU, MENU_SIZE);
		switch (int_to_key(_getch()))
		{
		case key::Enter:
			return index;
		case key::Up:
			do {
				index = (index == 0x80) ? 0x08 : index << 1;
			} while (!(index & menu_out));
			break;

		case key::Down:
			do {
				index = (index == 0x08) ? 0x80 : index >> 1;
			} while (!(index & menu_out));
			break;

		case key::Esc:
			return 0x08;
		}
	}
}

int GetUserInfo(const int DIFF, const std::string text, const int* interval, int user_enter) {
	int buff = user_enter;
	bool close = 0;
	while (!close) {
		bool correct = 0;
		system("cls");
		std::cout << text << "\n"
			<< " <" << user_enter << "> "
			<< "Используйте стрелки вправо/влево для изменения \n"
			<< "Нажмите Enter чтобы продолжить \n"
			<< "Нажмите Esc, для отмены изменений и возврата в главное меню \n";
		do {
			correct = 0;
			key user_choose = int_to_key(_getch());
			switch (user_choose) {
			case key::Enter:
				close = 1;
				return user_enter;
				break;
			case key::Esc:
				close = 1;
				return buff;
				break;

			case key::Utility:
				user_choose = int_to_key(_getch());
				if (user_choose == key::Left && user_enter > interval[0] + DIFF) {
					user_enter -= DIFF;
					correct = 1;
				}
				else if (user_choose == key::Right && user_enter < interval[1] - DIFF) {
					user_enter += DIFF;
					correct = 1;
				}
				break;
			default:
				break;
			}
		} while (!correct);
	}
}

int GetUserOpinion(int arg_num) {
	int correct = 0;
	std::cout << "Чтобы вывести информацию в файл нажмите " << arg_num + 1 << '\n' <<
		"\nНажмите Enter чтобы выбрать другой файл \n"
		<< "Нажмите Esc, чтоб выйти из программы \n";
	int real_arg_num = arg_num + 1;
	while (!correct) {
		int key = _getch();
		switch (key)
		{
		case '1':
			return 1;
		case '2':
			if (real_arg_num >= 2 && real_arg_num <= 3)
				return 2;
			break;
		case '3':
			if (real_arg_num == 3)
				return 3;
			break;
		case 13:
			return 13;
		case 27:
			return 27;
		default:
			break;
		}

	}
}

fs::path SaveFileDialog(const fs::path& filepath) {  // Вызов диалоговго окна выбора файла через проводник
	std::string defaultName = filepath.stem().string() + "_errors.txt";
	wchar_t filename[MAX_PATH];
	fs::path root = fs::current_path().root_directory();

	OPENFILENAME ofn;
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = L"Text Files\0*.txt\0*.*\0";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = L"Сохранить файл как";  //заголовок
	ofn.lpstrInitialDir = root.c_str();
	ofn.Flags = OFN_DONTADDTORECENT | OFN_OVERWRITEPROMPT;
	
	if (GetSaveFileNameW(&ofn)) {
		//std::cout << "Сохраняем в: " << filename << "\n";
		return fs::path(filename);
	}
	else {
		std::cout << "You cancelled.\n";
		return "0";
	}
}

fs::path place_to_save(const fs::path& filepath) {
	std::cout << "Выберите место для сохранения отчета \n"
		<< "1. Сохранить в той же папке, что и проверяемый файл\n"
		<< "2. Сохранить в папке с программой (exe)\n"
		<< "3. Выбрать папку для сохранения\n\n"
		<< "Нажмите Esc чтоб вернуться в меню обработки файла \n";
	int user_enter = 0;
	fs::current_path();
	
	while (true) {
		user_enter = _getch();
		switch (user_enter) {
		case '1':
            return filepath.parent_path() / (filepath.stem().wstring() + L"_errors.txt");
			break;
		case '2': {
			return exe_filepath.parent_path() / (filepath.stem().wstring() + L"_errors.txt");
			break;
		}
		case '3':
			return SaveFileDialog(filepath);
			break;
		case 27:
			return "";
			break;
		default:
			break;
		}
	}
}

void ReturnResult(const std::vector<string_info>& fileLines, const std::vector<err_info>& errorInfo, const int ref_percent, const int ref_interval, const std::filesystem::path& filepath) {
	//std::cout << fileLines.back().brackets << '\n';
	int err_size = errorInfo.size();
	std::vector<comm_percent> intervals = CommPercent(fileLines, ref_percent, ref_interval);
	do {
		int user_enter = 0;
		std::cout << "Файл: " << filepath << '\n';
		std::cout << "Найдено ошибок: " << err_size << "\n\n";
		std::cout << "Пороговый процент комментариев: " << setting.ref_percent << '\n' <<
			"Интервал оценивания: " << setting.ref_interval << "\n\n";

		bool interv_err = 0;
		int need_to_export = 0;
		if (intervals.size() == 0) {
			std::cout << "Количество комментариев соответствует требованию \n \n";
			if (err_size != 0) {
				std::cout << "Чтобы посмотреть ошибки нажмите 1 \n";
				user_enter = GetUserOpinion(1);
				if (user_enter == 1)
					print_error();
				else if (user_enter == 2)
					ExportError(errorInfo, intervals, filepath);
				else if (user_enter == 13)
					return;
				else if (user_enter == 27)
					std::exit(0);
			}
			else {
				
				user_enter = GetUserOpinion(0);
				if (user_enter == 13)
					return;
				else if (user_enter == 1)
					ExportError(errorInfo, intervals, filepath);
				else if (user_enter == 27)
					std::exit(0);
			}
		}
		else {
			std::cout << "Есть интервалы, с малым количеством комментариев \n \n"
				<< "Чтобы посмотреть интервалы с нехваткой комментариев нажмите 1 \n";
			interv_err = 1;
			if (err_size != 0) {
				std::cout << "Чтобы посмотреть ошибки нажмите 2 \n";
				user_enter = GetUserOpinion(2);
				if (user_enter == 1)
					CommPercentPrint(intervals, ref_interval, fileLines.size());
				else if (user_enter == 2)
					need_to_export = print_error();
				else if (user_enter == 3)
					ExportError(errorInfo, intervals, filepath);
				else if (user_enter == 13)
					return;
				else if (user_enter == 27)
					std::exit(0);
			}
			else {
				user_enter = GetUserOpinion(1);
				if (user_enter == 1)
					CommPercentPrint(intervals, ref_interval, fileLines.size());
				else if (user_enter == 2)
					ExportError(errorInfo, intervals, filepath);
				else if (user_enter == 13)
					return;
				else if (user_enter == 27)
					std::exit(0);
			}
		}
		system("cls");
	} while (1);

	CommPercentPrint(intervals, ref_interval, fileLines.size());
	print_error();
}

