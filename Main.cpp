#include "Main.h"
#include "Reader.h"
#include "PrintErr.h"
#include "Analyse.h"
#include "User.h"
#include "Menu.h"

#include <iostream>
#include <cstdlib>
#include <clocale>


std::filesystem::path exe_filepath; // храним глобально путь к ексешнику

std::vector<string_info> fileLines;
std::vector<err_info> errors;


int main(int argc, char* argv[])
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "");

    // Путь к exe с помощью аргументов cmd
    if (argc > 0) {
        exe_filepath = std::filesystem::absolute(argv[0]);
    }
    else {
        std::cerr << "Проблема с путем к файлу.";
        return -404;
    }

    
    // Маска обязательных параметров в начальном меню
    //
    // 11100000
    //
    // 80 = файл
    // 40 = процент
    // 20 = интервал
    const unsigned char complete_mask = 0xE0;
    
    MenuOut MainMenu; // создаем объект основного меню 


    MainMenu.Menu = {
        "Файл для анализа ",
        "Процент комментариев: ",
        "Интервал оценивания: ",
        "Продолжить",
        "Выход"
    };
    MainMenu.MenuParam = {
        [](){return setting.filepath.string();},
        [](){return std::to_string(setting.ref_percent) + "%"; },
        [](){return std::to_string(setting.ref_interval); },
        [](){return ""; },
        [](){return ""; }
    };
    // Изначально доступны:
    //
    // 10001000
    //
    // Файл
    // Выход
    //
    MainMenu.MenuOutParam = 0x88; //параметры вывода на экран
    MainMenu.MenuEnterParam = 0x60; // введенные параметры 0110 0000 т.к. для процента и интервала есть значения по умолчанию
    MainMenu.PostMenuMessage = "Используйте стрелки для навигации по меню, Enter для выбора.";

    MainMenuLogic MainLogic; // обьект логики меню
    // Главный цикл
    while (true){
        int result = menu_navigation(MainMenu, MainLogic); // показываем меню и обрабатываем ввод пользователя
        
        MainMenuAction action = static_cast<MainMenuAction>(result); //переводим ввод пользователя

        switch (action){
        case MainMenuAction::OpenFile:
            setting.filepath = OpenFileDialog();
            if (setting.filepath.empty()) {
                MainMenu.MenuEnterParam &= ~0x80; // И 01111111 следовательно бит обнулился
            }
            else {
                MainMenu.MenuEnterParam |= 0x80;
                fileLines = CopyStringFromFile(setting.filepath); // копируем строки в массив
            }
            break;
        case MainMenuAction::SetPercent:
            setting.ref_percent = ChangeMenuDialog(CommInfoType::Percent, setting);
            if (setting.ref_percent != -1)
                MainMenu.MenuEnterParam |= 0x40;
            else
                MainMenu.MenuEnterParam &= ~0x40;
            break;
        case MainMenuAction::SetInterval:
            setting.ref_interval = ChangeMenuDialog(CommInfoType::Interval, setting);
            if (setting.ref_interval != -1)
                MainMenu.MenuEnterParam |= 0x20;
            else
                MainMenu.MenuEnterParam &= ~0x20;
            break;
        case MainMenuAction::Continue:{
            if ((MainMenu.MenuEnterParam & complete_mask) == complete_mask){ //проверка заполненности всех параметров
                system("cls");
                std::cout << "Выполняется обработка...";
                AnaliseIterator(fileLines); //проанализировали файл
                ReturnResult(fileLines, errors, setting.filepath); // И показали результат
            }
            else{
                system("cls");
                std::cout << "Пожалуйста, заполните все параметры перед продолжением.\n"; // вообще кнопка продолжить не отобразиться, но во избежание проблем есть это
                int _k = _getch();
            }
            break;
        }
        case MainMenuAction::Exit:
            return 0;
        }
    }
}