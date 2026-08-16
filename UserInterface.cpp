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
    int key = _getch();

    if (key == 224)
        _getch();
}

int ChangeMenuDialog(CommInfoType ChangeType, Settings& set) {
    std::string type;
    
    int *value;

    MenuOut ChangeMenu;

    ChangeMenu.Menu = {
            "",
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
    else {
        std::cerr << "Неверный параметр функции ChangeMenuDialog " + type;
        return -1;
    }
    int orig_value = *value;
    ChangeMenu.MenuParam = {
        [&value]() {return "<" + std::to_string(*value) + ">"; },
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
                (*value > set.PERCENT_RANGE[0] and *value < set.PERCENT_RANGE[1]) :
                (*value > set.INTERVAL_RANGE[0] and *value < set.INTERVAL_RANGE[1]); //Мы уже проверили что тип точно определен

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
    // прямо в буфер диалога.
    wcscpy_s(filename, MAX_PATH, default_name.c_str());

    fs::path root = fs::current_path().root_directory();

    OPENFILENAMEW ofn{};

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFilter =L"Text Files\0*.txt\0 All Files\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle =
        L"Сохранить файл как";
    ofn.lpstrInitialDir =
        root.c_str();
    ofn.Flags = OFN_DONTADDTORECENT |
                OFN_OVERWRITEPROMPT;

    if (GetSaveFileNameW(&ofn)) {
        return fs::path(filename);
    }
    else 
        return {};
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
        "Используйте стрелки для навигации, "
        "Enter для выбора, Esc для возврата.\n";

    SaveMenuLogic SaveLogic;

    int result = menu_navigation(SaveMenu, SaveLogic);

    if (result == -1)
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
    std::vector<comm_percent> intervals = CommPercent(fileLines, setting.ref_percent, setting.ref_interval);

    unsigned char menu_mask = 0x38;

    std::string before_menu = "Файл: " + filepath.string() + "\n";

    if (errors.empty()) {
        before_menu += "Ошибок не найдено\n";
    }
    else {
        before_menu += "Найдено ошибок: " + std::to_string(errors.size()) + "\n";
        menu_mask |= 0x80;
    }

    before_menu += "Пороговый процент комментариев: " + std::to_string(setting.ref_percent) + "\n";

    before_menu += "Интервал оценивания: " + std::to_string(setting.ref_interval) + "\n\n";

    if (intervals.empty()) {
        before_menu += "Количество комментариев соответствует требованию\n";
    }
    else {
        before_menu += "Есть интервалы, с малым количеством комментариев\n";
        menu_mask |= 0x40;
    }

    MenuOut ReturnMenu;

    ReturnMenu.PreMenuMessage = before_menu;

    ReturnMenu.Menu = {
        "Показать ошибки",
        "Показать интервалы с нехваткой комментариев",
        "Экспортировать результат в файл",
        "Вернуться в главное меню",
        "Выйти из программы"
    };


    ReturnMenu.MenuOutParam = menu_mask;


    ReturnMenu.PostMenuMessage = "Используйте стрелки для навигации, Enter для выбора, Esc для возврата.";

    ReturnMenuLogic ReturnLogic;

    while (true) {
        int result = menu_navigation(ReturnMenu, ReturnLogic);

        if (result == -1)
            return;

        ReturnMenuAction action = static_cast<ReturnMenuAction>(result);

        switch (action){
        case ReturnMenuAction::OpenErrors:
            system("cls");
            print_error();
            break;

        case ReturnMenuAction::OpenComms:
            system("cls");
            CommPercentPrint(intervals, setting.ref_interval, fileLines.size());
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

    switch (change_type) {
    case ChangeMenuAction::ChangeNumLeft:
        if ((*num - diff) > range[0] and (*num - diff) < range[1]) {
            *num -= diff;
        }
        break;
    case ChangeMenuAction::ChangeNumRight:
        if ((*num + diff) > range[0] and (*num + diff) < range[1]) {
            *num += diff;
        }
        break;
    default:
        return;
    }
}