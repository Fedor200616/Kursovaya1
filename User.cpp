#include "User.h"
#include "Main.h"
#include "Reader.h"
#include "Chrono.h"
#include "PrintErr.h"
#include "Analyse.h"

Settings setting;
int main_func() {
	int file_choose = 1;
	while (file_choose == 1) {
		system("cls");
		errors.clear();
		fileLines.clear();
		std::cout << "Выберите файл \n";
		auto filePath = OpenFileDialog();
		if (!filePath) {
			key user_key = next_or_back("Выбор файла прерван,");
			if (user_key == key::Esc) {
				file_choose = 0;
				return 1;
			}
			else
				continue;
		}

		std::cout << "Selected file path: " << filePath.value() << std::endl;

		setting.ref_percent = GetUserInfo(setting.PERCENT_DIFF, "Выберите процент комментариев", setting.PERCENT_RANGE, setting.ref_percent); //Спрашиваем у пользователя процент комментариев 
		if (setting.ref_percent == -1)
			continue;

		setting.ref_inteval = GetUserInfo(setting.INTERVAL_DIFF, "Выберите интервал для проверки", setting.INTERVAL_RANGE, setting.ref_inteval); //Спрашиваем у пользователя интервал для проверки 
		if (setting.ref_inteval == -1)
			continue;

		auto start = chrono();
		fileLines = CopyStringFromFile(filePath.value());

		AnaliseIterator(fileLines);

		auto end = chrono();
		system("cls");
		std::cout << "Процесс выполнен за " << chrono_diff(start, end) << " секунды" << '\n';

		ReturnResult(fileLines, errors, setting.ref_percent, setting.ref_inteval, filePath.value());
	}
	return 0;
}

key next_or_back(std::string echo) {
	int user_enter = 0;
	std::cout << echo << "\n"
		<< "Нажмите Enter для повторного выбора \n"
		<< "Нажмите ESC для выхода \n";
	while (true) {
		int user_choose = _getch();
		if (user_choose == 13) {
			return key::Enter;
		}
		if (user_choose == 27) {
			return key::Esc;
		}
	}
}

int GetUserInfo(const int DIFF, const std::string text, const int *interval, int user_enter) {
	bool close = 0;
	while (!close) {
		bool correct = 0;
		system("cls");
		std::cout << text << "\n"
			<< " <" << user_enter << "> "
			<< "Используйте стрелки вправо/влево для изменения \n"
			<< "Нажмите Enter чтобы продолжить \n"
			<< "Нажмите Esc, чтоб вернуться к выбору файла \n";
		do {
			correct = 0;
			int user_choose = _getch();
			switch (user_choose) {
			case 13:
				close = 1;
				return user_enter;
				break;
			case 27:
				close = 1;
				return -1;
				break;
			case 224:
				user_choose = _getch();
				if (user_choose == 75 && user_enter > interval[0] + DIFF) {
					user_enter -= DIFF;
					correct = 1;
				}
				else if (user_choose == 77 && user_enter < interval[1] - DIFF) {
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
	std::cout << "\nНажмите Enter чтобы выбрать другой файл \n"
		<< "Нажмите Esc, чтоб выйти из программы \n";
	while (!correct) {
		int key = _getch();
		switch (key)
		{
		case '1':
			return 1;
		case '2':
			if (arg_num == 2)
				return 2;
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

void ReturnResult(const std::vector<string_info>& fileLines, const std::vector<err_info>& errorInfo, const int ref_percent, const int ref_interval, const std::filesystem::path& filepath) {
	//std::cout << fileLines.back().brackets << '\n';
	int err_size = errorInfo.size();
	std::vector<comm_percent> intervals = CommPercent(fileLines, ref_percent, ref_interval);
	do {
		int user_enter = 0;
		std::cout << "Файл: " << filepath << '\n';
		std::cout << "Найдено ошибок: " << err_size << '\n';

		bool interv_err = 0;
		if (intervals.size() == 0) {
			std::cout << "Количество комментариев соответствует требованию \n \n";
			if (err_size != 0) {
				std::cout << "Чтобы посмотреть ошибки нажмите 1 \n";
				user_enter = GetUserOpinion(1);
				if (user_enter == 1)
					print_error();
				else if (user_enter == 13)
					return;
				else if (user_enter == 27)
					std::exit(0);
			}
			else
				user_enter = GetUserOpinion(0);
			if (user_enter == 13)
				return;
			else if (user_enter == 27)
				std::exit(0);
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
					print_error();
				else if (user_enter == 13)
					return;
				else if (user_enter == 27)
					std::exit(0);
			}
			else {
				user_enter = GetUserOpinion(1);
				if (user_enter == 1)
					CommPercentPrint(intervals, ref_interval, fileLines.size());
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

