#include "User.h"

#include <iostream>
#include <string>

#include "Main.h"
#include "Reader.h"
#include "Chrono.h"
#include "PrintErr.h"
#include "Analyse.h"
#include "Menu.h"


// ============================================================
// ИЗМЕНЕНИЕ ЧИСЛОВЫХ ПАРАМЕТРОВ
// ============================================================

int GetUserInfo(
    int DIFF,
    const std::string& text,
    const int* interval,
    int user_enter
)
{
    // Исходное значение.
    // Нужно для отмены через ESC.
    int original_value = user_enter;


    while (true)
    {
        system("cls");

        std::cout
            << text << "\n\n"

            << " <" << user_enter << "> \n\n"

            << "Используйте стрелки вправо/влево "
            << "для изменения\n"

            << "Нажмите Enter чтобы продолжить\n"

            << "Нажмите Esc, для отмены изменений "
            << "и возврата в главное меню\n";


        key user_key = int_to_key(_getch());


        switch (user_key)
        {
        case key::Enter:

            return user_enter;


        case key::Esc:

            return original_value;


        case key::Utility:
        {
            key arrow = int_to_key(_getch());


            if (
                arrow == key::Left &&
                user_enter - DIFF >= interval[0]
                )
            {
                user_enter -= DIFF;
            }


            if (
                arrow == key::Right &&
                user_enter + DIFF <= interval[1]
                )
            {
                user_enter += DIFF;
            }

            break;
        }


        default:
            break;
        }
    }
}


// ============================================================
// СТАРАЯ ФУНКЦИЯ
// ============================================================

int GetUserOpinion(int arg_num)
{
    int real_arg_num = arg_num + 1;

    std::cout
        << "Чтобы вывести информацию в файл нажмите "
        << arg_num + 1 << '\n'

        << "\nНажмите Enter чтобы выбрать другой файл\n"

        << "Нажмите Esc, чтоб выйти из программы\n";


    while (true)
    {
        int pressed = _getch();

        switch (pressed)
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


// ============================================================
// ДИАЛОГ СОХРАНЕНИЯ
// ============================================================

fs::path SaveFileDialog(const fs::path& filepath)
{
    wchar_t filename[MAX_PATH] = {};

    std::wstring default_name =
        filepath.stem().wstring() +
        L"_errors.txt";


    // Записываем предлагаемое имя
    // прямо в буфер диалога.
    wcscpy_s(
        filename,
        MAX_PATH,
        default_name.c_str()
    );


    fs::path root =
        fs::current_path().root_directory();


    OPENFILENAMEW ofn{};

    ofn.lStructSize = sizeof(ofn);

    ofn.hwndOwner = nullptr;

    ofn.lpstrFilter =
        L"Text Files\0*.txt\0"
        L"All Files\0*.*\0";

    ofn.lpstrFile = filename;

    ofn.nMaxFile = MAX_PATH;

    ofn.lpstrTitle =
        L"Сохранить файл как";

    ofn.lpstrInitialDir =
        root.c_str();

    ofn.Flags =
        OFN_DONTADDTORECENT |
        OFN_OVERWRITEPROMPT;


    if (GetSaveFileNameW(&ofn))
    {
        return fs::path(filename);
    }


    return {};
}


// ============================================================
// МЕСТО СОХРАНЕНИЯ
// ============================================================

fs::path place_to_save(const fs::path& filepath)
{
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
        "Enter для выбора, Esc для возврата.";


    SaveMenuLogic SaveLogic;


    int result =
        menu_navigation(
            SaveMenu,
            SaveLogic
        );


    if (result == -1)
        return {};


    SaveMenuAction action =
        static_cast<SaveMenuAction>(result);


    switch (action)
    {
    case SaveMenuAction::SaveNearFile:

        return filepath.parent_path() /
            (
                filepath.stem().wstring() +
                L"_errors.txt"
                );


    case SaveMenuAction::SaveNearExe:

        return exe_filepath.parent_path() /
            (
                filepath.stem().wstring() +
                L"_errors.txt"
                );


    case SaveMenuAction::SaveOpinion:

        return SaveFileDialog(filepath);


    case SaveMenuAction::Exit:

        return {};


    default:

        return {};
    }
}


// ============================================================
// МЕНЮ РЕЗУЛЬТАТОВ
// ============================================================

void ReturnResult(
    const std::vector<string_info>& fileLines,
    const std::vector<err_info>& errorInfo,
    const fs::path& filepath
)
{
    std::vector<comm_percent> intervals =
        CommPercent(
            fileLines,
            setting.ref_percent,
            setting.ref_interval
        );


    // --------------------------------------------------------
    // Какие пункты показываем
    // --------------------------------------------------------

    unsigned char menu_mask = 0x38;


    // --------------------------------------------------------
    // Информация о результате
    // --------------------------------------------------------

    std::string before_menu =
        "Файл: " +
        filepath.string() +
        "\n";


    if (errors.empty())
    {
        before_menu +=
            "Ошибок не найдено\n";
    }
    else
    {
        before_menu +=
            "Найдено ошибок: " +
            std::to_string(errors.size()) +
            "\n";

        menu_mask |= 0x80;
    }


    before_menu +=
        "Пороговый процент комментариев: " +
        std::to_string(setting.ref_percent) +
        "\n";


    before_menu +=
        "Интервал оценивания: " +
        std::to_string(setting.ref_interval) +
        "\n\n";


    if (intervals.empty())
    {
        before_menu +=
            "Количество комментариев соответствует требованию\n";
    }
    else
    {
        before_menu +=
            "Есть интервалы, с малым количеством комментариев\n";

        menu_mask |= 0x40;
    }


    // --------------------------------------------------------
    // Создаём меню
    // --------------------------------------------------------

    MenuOut ReturnMenu;

    ReturnMenu.PreMenuMessage =
        before_menu;


    ReturnMenu.Menu =
    {
        "Показать ошибки",
        "Показать интервалы с нехваткой комментариев",
        "Экспортировать результат в файл",
        "Вернуться в главное меню",
        "Выйти из программы"
    };


    ReturnMenu.MenuOutParam =
        menu_mask;


    ReturnMenu.PostMenuMessage =
        "Используйте стрелки для навигации, "
        "Enter для выбора, Esc для возврата.";


    ReturnMenuLogic ReturnLogic;


    // --------------------------------------------------------
    // Цикл меню
    // --------------------------------------------------------

    while (true)
    {
        int result =
            menu_navigation(
                ReturnMenu,
                ReturnLogic
            );


        if (result == -1)
            return;


        ReturnMenuAction action =
            static_cast<ReturnMenuAction>(result);


        switch (action)
        {
        case ReturnMenuAction::OpenErrors:

            system("cls");

            print_error();

            _getch();

            break;


        case ReturnMenuAction::OpenComms:

            system("cls");

            CommPercentPrint(
                intervals,
                setting.ref_interval,
                fileLines.size()
            );

            _getch();

            break;


        case ReturnMenuAction::SaveResult:

            system("cls");

            ExportError(
                errorInfo,
                intervals,
                filepath
            );

            _getch();

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