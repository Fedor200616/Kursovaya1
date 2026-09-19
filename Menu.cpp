#include "Menu.h"

#include <cstdlib>
#include <iostream>
#include <sstream>


void show_menu(MenuOut& menu)
{
    system("cls");

    std::ostringstream buffer; // попытки избежать мерцания

    if (!menu.PreMenuMessage.empty())
    {
        buffer << menu.PreMenuMessage; 

        if (menu.PreMenuMessage.back() != '\n') //чтоб мэсседж был отделен минимум 1 строкой для красоты
            buffer << '\n';

        buffer << '\n';
    }

    for (size_t i = 0; i < menu.Menu.size(); ++i)
    {
        unsigned char mask =
            static_cast<unsigned char>(0x80 >> i); // проходим циклом по каждому пункту меню, можно ли его отображать

        // Пункт скрыт
        if (!(menu.MenuOutParam & mask)) {
            if (menu.ActIndex & mask) {
                menu.ActIndex >>= 1;
            }
            continue;
        }


        // Маркер
        if (menu.ActIndex & mask)
            buffer << menu.ActMark;
        else
            buffer << menu.InactMark;


        // Название
        buffer << menu.Menu[i];


        // Параметр
        if (i < menu.MenuParam.size())
        {
            buffer << menu.MenuParam[i]();
        }

        buffer << '\n';
    }

    if (!menu.PostMenuMessage.empty())
    {
        buffer << '\n';
        buffer << menu.PostMenuMessage;
    }

    std::cout << buffer.str(); // вывод собранного меню
}


int menu_navigation(MenuOut& menu, MenuLogic& logic)
{
    while (true)
    {
        // Позволяем конкретной логике обновить состояние меню.
        logic.BeforeShow(menu);


        // Полностью формируем кадр
        show_menu(menu);


        // Ждём клавишу
        key key_code = int_to_key(_getch());


        // Превращаем клавишу в действие
        MenuAction action = logic.GetAction(key_code);

        // Анализ действия
        // ESC
        if (action == MenuAction::Exit)
            return -1;
        // ENTER
        if (action == MenuAction::Select)
            return logic.Select(menu);
        // Стрелки
        int result = logic.ProcessNavigation(menu, action); //функции смены указателя или значения заложены внутри функции
        if (result != 0) {
            return result; // как правило 0
        }
    }
    return 0;
}