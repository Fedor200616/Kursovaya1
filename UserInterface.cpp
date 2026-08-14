#include "User.h"
#include "Main.h"
#include "Reader.h"
#include "Chrono.h"
#include "PrintErr.h"
#include "Analyse.h"
#include "Menu.h"

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

void RetrunResult(const std::vector<string_info>& fileLines, const std::vector<err_info>& errorInfo, const std::filesystem::path& filepath){
	std::string BeforeMenu = "Файл: " + filepath.string() + "\n" +
		"Найдено ошибок: " + std::to_string(errorInfo.size()) + "\n\n" +

		"Пороговый процент комментариев: " + std::to_string(setting.ref_percent) + '\n' +
		"Интервал оценивания: " + std::to_string(setting.ref_interval) + "\n\n";


}

//Программа не проверяет файл заново после выбора нового файла, а сразу выводит результат предыдущей проверки. Нужно добавить проверку нового файла и вывод результата для него.
