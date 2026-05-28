#include "PrintErr.h"
#include "User.h"
#include "Main.h"

int print_error(const std::vector<err_info>& errors) {
	system("cls");
	int err_size = errors.size();
	std::cout << "Найдено ошибок:" << ' ' << err_size << '\n'
		<< '\n';
	for (auto& err : errors) {
		std::cout << "Позиция {" << err.position.line << ", " << err.position.num 
			<< "}, Тип ошибки: " << err.message(err.type) << ", символ: " << err.symbol << ", строка:" << '\n'
			<< fileLines[err.position.line].str << '\n'
			<< '\n';
	}
	std::cout << "Для продолжения нажмите любую кнопку.";
	_getch();
	return 0;
}

int CommPercentPrint(const std::vector<comm_percent>& comm_vec, int interval_size, int fileinfosize) {
	system("cls");
	for (size_t i = 0; i < comm_vec.size(); i++) {
		int start = comm_vec[i].interval * interval_size + 1;
		int end = std::min(start + interval_size - 1, fileinfosize);

		std::cout << "Интервал: " << comm_vec[i].interval << ". Строки с " << start << ", по " << end
			<< " Процент комментариев: " << comm_vec[i].percent << "%" << '\n';

	}
	std::cout << "Для продолжения нажмите любую кнопку.";
	_getch();
	return 0;
}
