#include "Main.h"
#include "Reader.h"
#include "Analyse.h"
#include "Chrono.h"
#include "LexerUtils.h"

fs::path OpenFileDialog() {  // Вызов диалогового окна выбора файла через проводник
    wchar_t filename[MAX_PATH];
    fs::path root = fs::current_path().root_directory();

    OPENFILENAME ofn;
    ZeroMemory(&filename, sizeof(filename));
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = L"C++\0*.cpp;*.c\0Headers\0*.h";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = L"Выберите файл";
    ofn.lpstrInitialDir = root.c_str();
    ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn)) {
        fs::path selected(filename);
        auto ext = selected.extension().string();
        // Приводим расширение к нижнему регистру для универсальности
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext == ".c" || ext == ".cpp" || ext == ".h") {
            std::wcout << L"You chose the file \"" << filename << L"\"\n";
            return selected;
        }
        else {
            std::wcout << L"Ошибка: выберите файл с расширением .c, .cpp или .h\n";
            return fs::path{};
        }
    }
    else {
        std::wcout << L"You cancelled.\n";
        return fs::path{}; // Пустой путь
    }
}

std::vector<string_info> CopyStringFromFile(const fs::path& filePath) { //Построчечное считываение файла в вектор
    std::vector<string_info> results;
    std::ifstream ofile(filePath);
    if (!ofile.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl; //TODO выводить ошибку через отдельную функцию
        return results; // Возвращаем пустой вектор в случае ошибки
    }

    std::string line;
    results.push_back(empty_str_info); // Инициализируем нулевую строку для хранения информации о предыдущей строке
    for (int i = 1; std::getline(ofile, line); i++) {
        results.push_back(empty_str_info);
        results.back().line = i;
        results.back().str = line;
    }
    return results;
}


int AnaliseIterator(std::vector<string_info>& info) {
    for (int i = 1; i < info.size(); i++) {
        analyse(info[i - 1], info[i]);
    }
    if (!info.back().brackets.empty()) {
        FindEndBrackets(info);
    }
   
    return 0;
}

