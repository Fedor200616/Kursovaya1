#include "Main.h"
#include "Reader.h"
#include "Chrono.h"
#include "PrintErr.h"
#include "Analyse.h"
#include "User.h"
#include "Menu.h"

#include <iostream>
#include <cstdlib>
#include <clocale>


std::filesystem::path exe_filepath;

std::vector<string_info> fileLines;
std::vector<err_info> errors;


int main(int argc, char* argv[])
{
    // --------------------------------------------------------
    // Путь к exe
    // --------------------------------------------------------

    if (argc > 0)
    {
        exe_filepath =
            std::filesystem::absolute(argv[0]);
    }


    // --------------------------------------------------------
    // Кодировка консоли
    // --------------------------------------------------------

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    setlocale(LC_ALL, "");


    // --------------------------------------------------------
    // Маска обязательных параметров
    //
    // 11100000
    //
    // 80 = файл
    // 40 = процент
    // 20 = интервал
    // --------------------------------------------------------

    const unsigned char complete_mask =
        0xE0;


    // --------------------------------------------------------
    // Главное меню
    // --------------------------------------------------------

    MenuOut MainMenu;


    MainMenu.Menu =
    {
        "Файл для анализа ",
        "Процент комментариев: ",
        "Интервал оценивания: ",
        "Продолжить",
        "Выход"
    };


    MainMenu.MenuParam =
    {
        []()
        {
            return setting.filepath.string();
        },

        []()
        {
            return
                std::to_string(
                    setting.ref_percent
                ) + "%";
        },

        []()
        {
            return
                std::to_string(
                    setting.ref_interval
                );
        },

        []()
        {
            return "";
        },

        []()
        {
            return "";
        }
    };


    // Изначально доступны:
    //
    // 10001000
    //
    // Файл
    // Выход
    //
    MainMenu.MenuOutParam =
        0x88;


    // Установлены по умолчанию:
    //
    // процент
    // интервал
    //
    MainMenu.MenuEnterParam =
        0x60;


    MainMenu.PostMenuMessage =
        "Используйте стрелки для навигации "
        "по меню, Enter для выбора, Esc для выхода.";


    MainMenuLogic MainLogic;


    // --------------------------------------------------------
    // Главный цикл
    // --------------------------------------------------------

    while (true)
    {
        int result =
            menu_navigation(
                MainMenu,
                MainLogic
            );


        // ESC
        if (result == -1)
        {
            return 0;
        }


        MainMenuAction action =
            static_cast<MainMenuAction>(result);


        switch (action)
        {
            // ====================================================
            // ФАЙЛ
            // ====================================================

        case MainMenuAction::OpenFile:
        {
            setting.filepath =
                OpenFileDialog();


            if (setting.filepath.empty())
            {
                MainMenu.MenuEnterParam &=
                    ~0x80;
            }
            else
            {
                MainMenu.MenuEnterParam |=
                    0x80;


                fileLines =
                    CopyStringFromFile(
                        setting.filepath
                    );
            }

            break;
        }


        // ====================================================
        // ПРОЦЕНТ
        // ====================================================

        case MainMenuAction::SetPercent:

            setting.ref_percent =
                GetUserInfo(
                    setting.PERCENT_DIFF,
                    setting.percent_dialog,
                    setting.PERCENT_RANGE,
                    setting.ref_percent
                );

            MainMenu.MenuEnterParam |=
                0x40;

            break;


            // ====================================================
            // ИНТЕРВАЛ
            // ====================================================

        case MainMenuAction::SetInterval:

            setting.ref_interval =
                GetUserInfo(
                    setting.INTERVAL_DIFF,
                    setting.interval_dialog,
                    setting.INTERVAL_RANGE,
                    setting.ref_interval
                );

            MainMenu.MenuEnterParam |=
                0x20;

            break;


            // ====================================================
            // ПРОДОЛЖИТЬ
            // ====================================================

        case MainMenuAction::Continue:
        {
            // Проверяем, установлены ли
            // все необходимые параметры.
            if (
                (MainMenu.MenuEnterParam &
                    complete_mask)
                == complete_mask
                )
            {
                system("cls");


                auto start =
                    chrono();


                AnaliseIterator(
                    fileLines
                );


                auto end =
                    chrono();


                system("cls");


                std::cout
                    << "Процесс выполнен за "
                    << chrono_diff(
                        start,
                        end
                    )
                    << " секунды\n";


                _getch();


                ReturnResult(
                    fileLines,
                    errors,
                    setting.filepath
                );
            }
            else
            {
                system("cls");

                std::cout
                    << "Пожалуйста, заполните "
                    << "все параметры перед продолжением.\n";

                _getch();
            }

            break;
        }


        // ====================================================
        // ВЫХОД
        // ====================================================

        case MainMenuAction::Exit:

            return 0;
        }
    }
}