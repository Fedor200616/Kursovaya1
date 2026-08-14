#include "Menu.h"


void show_menu(const MenuOut& menu) // Отображает меню
{
    system("cls");

    std::ostringstream out_buffer; 

    unsigned char menu_out = menu.MenuOutParam; 

    if (!menu.PreMenuMessage.empty()) // Если есть сообщение перед меню
    {
        out_buffer << menu.PreMenuMessage << '\n'; // Добавляем его в буфер
    }

    for (size_t i = 0; i < menu.Menu.size(); ++i) // Перебираем пункты меню
    {
        unsigned char mask = 0x80 >> i; // Получаем бит текущего пункта

        if (!(menu_out & mask)) // Если пункт скрыт
        {
            continue;
        }

        if (menu.ActIndex & mask) // Если пункт выбран
        {
            out_buffer << menu.ActMark; 
        }
        else 
        {
            out_buffer << menu.InactMark;
        }

        out_buffer << menu.Menu[i]; 

        if (i < menu.MenuParam.size() && (menu.MenuEnterParam & mask)) // Если у пункта есть установленный параметр
        {
            out_buffer << menu.MenuParam[i](); // Выводим параметр
        }

        out_buffer << '\n';
    }

    if (!menu.PostMenuMessage.empty()) // Если есть сообщение после меню
    {
        out_buffer << '\n' << menu.PostMenuMessage;
    }

    std::cout << out_buffer.str(); 
}

