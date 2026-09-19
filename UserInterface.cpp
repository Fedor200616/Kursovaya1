#include "User.h"

#include <iostream>
#include <string>

#include "Main.h"
#include "Reader.h"
#include "PrintErr.h"
#include "Analyse.h"
#include "Menu.h"

void wait_key()
{
    while (_kbhit()) _getch(); //Обнуляем ввод клавиш при использовании меню
    int key = _getch();

    if (key == 224)
        _getch(); //если пользователь нажал на стелочки наприммер
}

int ChangeMenuDialog(CommInfoType ChangeType, Settings& set) {
    std::string type;
    
    int *value; // значение которое изменяем (процент или интервал в зависимости от типа окна)

    MenuOut ChangeMenu;

    ChangeMenu.Menu = {
            "", //выводится процент
            "Применить изменения",
            "Отменить изменения"
        };
    ChangeMenu.PostMenuMessage = "Используйте стрелки для навигации по меню и изменению параметров, Enter для выбора";

    if (ChangeType == CommInfoType::Percent) {
        type = "Percent";
        value = &set.ref_percent;

        ChangeMenu.PreMenuMessage = set.percent_dialog;
    }
    else if (ChangeType == CommInfoType::Interval) {
        type = "Interval";
        value = &set.ref_interval;

        ChangeMenu.PreMenuMessage = set.interval_dialog;
    }
    else { //вообще вызываться не должно, но все равно, как минимум для безопасности указателя
        std::cerr << "Неверный параметр функции ChangeMenuDialog";
        return -1;
    }
    int orig_value = *value; //начальное значение для отката

    ChangeMenu.MenuParam = {
        [&value]() {return "<" + std::to_string(*value) + ">"; }, //вычисляет под обновленные value
        []() {return ""; },
        []() {return ""; },
    };

    ChangeMenuLogic ChangeLogic;

    while (true) {
        int result = menu_navigation(ChangeMenu, ChangeLogic);
        if (result == -1) { //ESC
            continue;
        }

        ChangeMenuAction action = static_cast<ChangeMenuAction>(result);

        bool is_correct = ChangeType == CommInfoType::Percent ?
                (*value >= set.PERCENT_RANGE[0] and *value <= set.PERCENT_RANGE[1]) :  //Мы уже проверили что тип точно определен
                (*value >= set.INTERVAL_RANGE[0] and *value <= set.INTERVAL_RANGE[1]); //Поэтому или проценты или интервал

        switch (action) {
        case ChangeMenuAction::Enter:
            
            if (is_correct) {
                return *value;
            }
            else
                return orig_value;
            break;
        
        case ChangeMenuAction::Cancel:
            *value = orig_value;
            return orig_value;
            break;

        case ChangeMenuAction::ChangeNumLeft:
            ChangeNum(set, action, ChangeType);
            break;

        case ChangeMenuAction::ChangeNumRight:
            ChangeNum(set, action, ChangeType);
            break;

        case ChangeMenuAction::None:
        default:
            break;
        }
    }   
    return -1;
}

fs::path SaveFileDialog(const fs::path& filepath) {
    wchar_t filename[MAX_PATH] = {}; //Windows нативно UTF-16

    std::wstring default_name =
        filepath.stem().wstring() +
        L"_errors.txt";

    // Записываем предлагаемое имя
    // прямо в строку диалогового окна
    wcscpy_s(filename, MAX_PATH, default_name.c_str());

    fs::path root = fs::current_path().root_directory();

    OPENFILENAMEW ofn{}; // Структура с параметрами стандартного диалога сохранения Windows

    ofn.lStructSize = sizeof(ofn); 
    ofn.hwndOwner = nullptr;
    ofn.lpstrFilter =L"Text Files\0*.txt\0 All Files\0*.*\0"; // Фильтры типов файлов в диалоге
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle =
        L"Сохранить файл как";
    ofn.lpstrInitialDir =
        root.c_str();
    ofn.Flags = OFN_DONTADDTORECENT | // Не добавлять файл в список последних документов
                OFN_OVERWRITEPROMPT;  // и запрашивать подтверждение при перезаписи существующего файла

    if (GetSaveFileNameW(&ofn)) {
        while (_kbhit()) _getch();  // Очищаем оставшиеся нажатия клавиш от меню
        return fs::path(filename);
    }
    else {
        while (_kbhit()) _getch(); 
        return {};  // Пустой путь означает отмену сохранения
    }
}

fs::path place_to_save(const fs::path& filepath){
    MenuOut SaveMenu;

    SaveMenu.PreMenuMessage =
        "Выберите место для сохранения отчета";

    SaveMenu.Menu =
    {
        "Сохранить в той же папке, что и проверяемый файл", 
        "Сохранить в папке с программой (exe)",
        "Выбрать папку для сохранения",
        "Вернуться в меню обработки файла"
    };

    SaveMenu.MenuOutParam = 0xF0;

    SaveMenu.PostMenuMessage =
		"Примечание: при выборе 1-го и 2-го пунктов меню, имя файла будет сгенерировано как <имя_файла>_errors.txt\n"
		"При этом, если файл уже существует, он будет перезаписан!\n\n"
        "Используйте стрелки для навигации, "
        "Enter для выбора, Esc для возврата.\n";

    SaveMenuLogic SaveLogic;

    int result = menu_navigation(SaveMenu, SaveLogic);

    if (result == -1) //Esc
        return {};

    SaveMenuAction action = static_cast<SaveMenuAction>(result);

    switch (action) {
    case SaveMenuAction::SaveNearFile:

        return filepath.parent_path() / (filepath.stem().wstring() + L"_errors.txt");

    case SaveMenuAction::SaveNearExe:
        return exe_filepath.parent_path() / (filepath.stem().wstring() + L"_errors.txt");

    case SaveMenuAction::SaveOpinion:
        return SaveFileDialog(filepath);

    case SaveMenuAction::Exit:
        return {};

    default:
        return {};
    }
}

void ReturnResult(const std::vector<string_info>& fileLines, const std::vector<err_info>& errorInfo, const fs::path& filepath){
   
    // Рассчитываем интервалы с недостаточным количеством комментариев
    std::vector<comm_percent> intervals = CommPercent(fileLines, setting.ref_percent, setting.ref_interval);

    unsigned char menu_mask = 0x38; // Маска доступности пунктов меню по умолчанию

    std::string before_menu = "Файл: " + filepath.string() + "\n";

    if (errors.empty()) {
        before_menu += "Ошибок не найдено\n";
    }
    else {
        before_menu += "Найдено ошибок: " + std::to_string(errors.size()) + "\n";
        menu_mask |= 0x80; // Включаем пункт просмотра ошибок
    }

    before_menu += "Пороговый процент комментариев: " + std::to_string(setting.ref_percent) + "\n";

    before_menu += "Интервал оценивания: " + std::to_string(setting.ref_interval) + "\n\n";

    if (intervals.empty()) {
        before_menu += "Количество комментариев соответствует требованию\n";
    }
    else {
        before_menu += "Есть интервалы, с малым количеством комментариев\n";
        menu_mask |= 0x40; // Включаем пункт просмотра проблемных интервалов
    }

    MenuOut ReturnMenu;

    ReturnMenu.PreMenuMessage = before_menu;

    ReturnMenu.Menu = {
        "Показать ошибки", // 80
        "Показать интервалы с нехваткой комментариев", // 40
        "Экспортировать результат в файл", // 20
        "Вернуться в главное меню", // 10
        "Выйти из программы" // 08
    };


    ReturnMenu.MenuOutParam = menu_mask;


    ReturnMenu.PostMenuMessage = "Используйте стрелки для навигации, Enter для выбора, Esc для возврата.";

    ReturnMenuLogic ReturnLogic;

    while (true) {
        int result = menu_navigation(ReturnMenu, ReturnLogic);

        if (result == -1) //Esv
            return;

        ReturnMenuAction action = static_cast<ReturnMenuAction>(result);

        switch (action){
        case ReturnMenuAction::OpenErrors:
            system("cls");
            print_error();
            break;

        case ReturnMenuAction::OpenComms:
            system("cls");
            CommPercentPrint(intervals, setting.ref_interval, fileLines.size() - 1); //-1 потому что нулевая строка не используется
            break;

        case ReturnMenuAction::SaveResult:
            system("cls");
            ExportError(errorInfo, intervals, filepath);
            break;

        case ReturnMenuAction::ExitToMain:
            return;

        case ReturnMenuAction::ExitToDesktop:
            std::exit(0);

        default:
            break;
        }
    }
}


void ChangeNum(Settings& set, ChangeMenuAction change_type, CommInfoType num_type) {
    int *num;
    int diff;
    int range[2];

    // Выбираем параметр, его шаг и допустимый диапазон
    // в зависимости от типа изменяемого значения
    if (num_type == CommInfoType::Percent) {
        num = &set.ref_percent;
        diff = set.PERCENT_DIFF;
        for (int i = 0; i < 2; i++) {
            range[i] = set.PERCENT_RANGE[i];
        }
    }
    else{
        num = &set.ref_interval;
        diff = set.INTERVAL_DIFF;
        for (int i = 0; i < 2; i++) {
            range[i] = set.INTERVAL_RANGE[i];
        }
    }

    // Изменяем значение только в пределах допустимого диапазона
    switch (change_type) {
    case ChangeMenuAction::ChangeNumLeft:
        if ((*num - diff) >= range[0] and (*num - diff) <= range[1]) {
            *num -= diff;
        }
        break;
    case ChangeMenuAction::ChangeNumRight:
        if ((*num + diff) >= range[0] and (*num + diff) <= range[1]) {
            *num += diff;
        }
        break;
    default:
        return;
    }
}