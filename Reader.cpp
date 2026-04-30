#define _CRT_SECURE_NO_WARNINGS
#include "Main.h"
#include "Reader.h"
#include "Analyse.h"

fs::path OpenFileDialog() {  // Вызов диалоговго окна выбора файла через проводник
    wchar_t filename[MAX_PATH];
    fs::path root = fs::current_path().root_directory();

    OPENFILENAME ofn;
    ZeroMemory(&filename, sizeof(filename));
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = L"Text Files\0*.cpp\0*.*\0";  //фильтруем на текстовые
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = L"Выберите файл";  //заголовок
    ofn.lpstrInitialDir = root.c_str();
    ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST; //Флаги не добавлять несуществующий, не делать недавним 

    if (GetOpenFileNameW(&ofn)) {
        std::wcout << L"You chose the file \"" << filename << L"\"\n";
        return fs::path(filename);
    }
    else {
        std::wcout << L"You cancelled.\n";
        return "0";
    }
}

std::vector<string_info> CopyStringFromFile(const fs::path filePath) { //Построчечное считываение файла в вектор
    std::vector<string_info> results;

    std::ifstream ofile(filePath);
    if (!ofile.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl; //TODO выводить ошибку через отдельную функцию
        return results; // Возвращаем пустой вектор в случае ошибки
    }

    std::string line;
    results.push_back({ 0, "", "", 0, 0, 0 }); // Инициализируем нулевую строку для хранения информации о предыдущей строке
    for (int i = 1; std::getline(ofile, line); i++) {
        results.push_back(empty_str_info);
    }



    return results;
}

string_info GetStringInfo(const string_info str_info) { // Получение информации о передыдущей строке строке
    // на входе имеем информацию о предыдущей строке, на выходе - информацию о текущей строке
    string_info result = str_info;
    result = analyse(str_info);
	
	// И так далее, по мере добавления новых проверок


    return result; // информация о текущей строке
}
