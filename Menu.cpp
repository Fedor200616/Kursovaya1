#include "Menu.h"

#include <cstdlib>
#include <iostream>
#include <sstream>


void show_menu(const MenuOut& menu)
{
    system("cls");

    std::ostringstream buffer;

    if (!menu.PreMenuMessage.empty())
    {
        buffer << menu.PreMenuMessage;

        if (menu.PreMenuMessage.back() != '\n')
            buffer << '\n';

        buffer << '\n';
    }

    for (size_t i = 0; i < menu.Menu.size(); ++i)
    {
        unsigned char mask =
            static_cast<unsigned char>(0x80 >> i);

        // Пункт скрыт
        if (!(menu.MenuOutParam & mask))
            continue;


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

    std::cout << buffer.str();
}


int menu_navigation(MenuOut& menu, MenuLogic& logic)
{
    while (true)
    {
        // Позволяем конкретной логике
        // обновить состояние меню.
        logic.BeforeShow(menu);


        // Полностью формируем кадр
        show_menu(menu);


        // Ждём клавишу
        key key_code = int_to_key(_getch());


        // Превращаем клавишу в действие
        MenuAction action = logic.GetAction(key_code);


        // ESC
        if (action == MenuAction::Exit)
            return -1;


        // ENTER
        if (action == MenuAction::Select)
            return logic.Select(menu);


        // Стрелки
        logic.ProcessNavigation(menu, action);
    }
}