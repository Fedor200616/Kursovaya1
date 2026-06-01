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
			<< delete_tab(fileLines[err.position.line].str) << '\n';

		int caret_pos = err.position.num - offset(fileLines[err.position.line].str);
		std::cout << std::string(caret_pos, ' ') << "^\n";
	}
	std::cout << "Нажмите 1, для вывода информации в файл\n"
		<< '\n'
		<< "Нажмите Enter чтобы продолжить";
	int user_choose = _getch();
	if (user_choose == '1')
		return 1;
	else
		return 0;
}

int CommPercentPrint(const std::vector<comm_percent>& comm_vec, int interval_size, int fileinfosize) {
	system("cls");
	for (size_t i = 0; i < comm_vec.size(); i++) {
		int start = comm_vec[i].interval * interval_size + 1;
		int end = min(start + interval_size - 1, fileinfosize);

		std::cout << "Интервал: " << comm_vec[i].interval << ". Строки с " << start << ", по " << end
			<< " Процент комментариев: " << comm_vec[i].percent << "%" << '\n';

	}
	std::cout << "Для продолжения нажмите любую кнопку.";
	_getch();
	return 0;
}

std::string delete_tab(const std::string& s) {
	size_t first = s.find_first_not_of(" \t"); //Находим сивол который не пробел и не таб
	if (first == std::string::npos)
		return "";
	else
		return s.substr(first);
}

int offset(const std::string& s) {
	// Находим, сколько символов мы обрезали в начале (табы и пробелы)
	size_t offset = s.find_first_not_of(" \t");
	if (offset == std::string::npos) return 0;
	return offset;
}


void ExportError(const std::vector<err_info>& errorInfo, const std::vector<comm_percent>& comm_vec, const std::filesystem::path& filepath) {
	system("cls");
	std::filesystem::path path = place_to_save(filepath);
	if (path.empty()) {
		return;
	}
	std::ofstream outFile(path);

	std::filesystem::path reportPath = std::filesystem::absolute(path);
	std::cout << "Путь к файлу отчета: " << reportPath.string() << '\n';

	if (!outFile.is_open()) {
		std::cerr << "Error opening file for writing: " << path.stem().string() + "_errors.txt" << std::endl;
		std::cout << "Нажмите любую кнопку чтобы продолжить\n";
		_getch();
		return;
	}
	outFile << "Информация о файле: " << std::filesystem::absolute(filepath).string() << "\n\n";
	outFile << "Интервалов с малым количеством комментариев: " << comm_vec.size() << "\n\n";
	int interval_size = setting.ref_inteval;
	int fileinfosize = fileLines.size() - 1; // -1 потому что нулевая строка не используется
	for (size_t i = 0; i < comm_vec.size(); i++) {
		int start = comm_vec[i].interval * interval_size + 1;
		int end = min(start + interval_size - 1, fileinfosize);

		outFile << "Интервал: " << comm_vec[i].interval << ". Строки с " << start << ", по " << end
			<< " Процент комментариев: " << comm_vec[i].percent << "%" << '\n';

	}

	outFile << "\nОшибок в файле: " << errorInfo.size() << "\n\n";

	for (const auto& err : errorInfo) {
		outFile << "Позиция {" << err.position.line << ", " << err.position.num
			<< "}, Тип ошибки: " << err.message(err.type) << ", символ: " << err.symbol << ", строка:" << '\n'
			<< delete_tab(fileLines[err.position.line].str) << '\n';
		int caret_pos = err.position.num - offset(fileLines[err.position.line].str);
		outFile << std::string(caret_pos, ' ') << "^\n";
	}
	outFile << "Конец отчета\n";
	outFile.close();
	std::cout << "Отчет успешно сохранен.\n"
		<< "Нажмите любую кнопку чтобы продолжить\n";
	_getch();
}