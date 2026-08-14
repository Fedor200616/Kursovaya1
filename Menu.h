#pragma once
#include <functional>
#include <vector>
#include <conio.h>
#include <iostream>
#include <string>
#include <sstream>

enum class key // Физические клавиши
{
    Up = 72,
    Down = 80,
    Left = 75,
    Right = 77,
    Enter = 13,
    Esc = 27,
    Utility = 224,
    Null = 0
};
/// <summary>
/// Функция преобразования кода клавиши в enum ke
/// </summary>
/// <param name="int_key">Код клавиши</param>
/// <returns>Клавиша типа Key</returns>
inline key int_to_key(int int_key) {
    switch (int_key) {
    case 72:
        return key::Up;
    case 80:
        return key::Down;
    case 75:
        return key::Left;
    case 77:
        return key::Right;
    case 13:
        return key::Enter;
    case 27:
        return key::Esc;
    case 224:
        return key::Utility;
    default:
        return key::Null;
    }
}

enum class MenuAction // Универсальные действия самого меню
{
    None,
    MoveUp,
    MoveDown,
    ChangeLeft,
    ChangeRight,
    Select,
    Exit
};

enum class MainMenuAction // Действия конкретно главного меню
{
    None,
    OpenFile,
    SetPercent,
    SetInterval,
    Continue,
    Exit
};

struct MenuOut // Состояние и внешний вид меню
{
    unsigned char ActIndex = 0x80; // Бит текущего выбранного пункта
    std::string PreMenuMessage = ""; // Сообщение перед меню
    std::vector<std::string> Menu; // Названия пунктов меню
    std::vector<std::function<std::string()>> MenuParam; // Функции, возвращающие актуальный параметр
    std::string PostMenuMessage = ""; // Сообщение после меню
    std::string ActMark = "->"; // Маркер активного пункта
    std::string InactMark = "  "; // Маркер неактивного пункта
    unsigned char MenuEnterParam = 0xFF; // Биты установленных параметров
    unsigned char MenuOutParam = 0xFF; // Биты отображаемых пунктов

    static MenuAction GetMenuAction(key key_code)// Преобразует клавишу в универсальное действие меню
    {
        switch (key_code)
        {
        case key::Up:
            return MenuAction::MoveUp; // Вверх

        case key::Down:
            return MenuAction::MoveDown; // Вниз

        case key::Left:
            return MenuAction::ChangeLeft; // Влево

        case key::Right:
            return MenuAction::ChangeRight; // Вправо

        case key::Enter:
            return MenuAction::Select; // Выбрать

        case key::Esc:
            return MenuAction::Exit; // Выйти

        default:
            return MenuAction::None; // Ничего
        }
    };
};

void show_menu(const MenuOut& menu);

template <typename Logic> // Logic определяет конкретную логику меню
typename Logic::Action menu_navigation(MenuOut& menu, Logic& logic) // Универсальная навигация по любому меню
{
    while (true) // Работаем до получения конечного действия
    {
		logic.BeforeShow(menu); // Вызываем метод логики перед отображением меню, чтобы обновить параметры

        show_menu(menu); // Отображаем текущее состояние меню

        key key_code = int_to_key(_getch());

        MenuAction menu_action = MenuOut::GetMenuAction(key_code); 

        auto action = logic.Process(menu, menu_action); // Передаём действие конкретной логике

        if (action != Logic::Action::None) // Если логика вернула настоящее действие
        {
            return action; // Возвращаем его вызывающему коду
        }
    }
}

class MainMenuLogic // Логика главного меню
{
public:
    using Action = MainMenuAction; // Тип действия, который возвращает эта логика

	void BeforeShow(MenuOut& menu) // Вызывается перед отображением меню, можно использовать для обновления параметров
	{
		if(menu.MenuEnterParam & 0x80){
			menu.MenuOutParam |= 0xF8; // Если файл выбран, показываем все пункты
        }
	}
    Action Process(MenuOut& menu, MenuAction action) // Обрабатывает действие меню и возвращает действие программы
    {
        switch (action)
        {
        case MenuAction::MoveUp:
            MoveUp(menu); // Обрабатываем движение вверх
            break;

        case MenuAction::MoveDown:
            MoveDown(menu); // Обрабатываем движение вниз
            break;

        case MenuAction::ChangeLeft:
            ChangeLeft(menu); // Обрабатываем движение влево
            break;

        case MenuAction::ChangeRight:
            ChangeRight(menu); // Обрабатываем движение вправо
            break;

        case MenuAction::Select:
            return Select(menu); // Возвращаем действие выбранного пункта

        case MenuAction::Exit:
            return MainMenuAction::Exit; // Escape означает выход из меню

        default:
            break;
        }

        return MainMenuAction::None; // Продолжаем работу меню
    }

private:
    void MoveUp(MenuOut& menu) { // Перемещает выбор вверх
        do
        {
            menu.ActIndex = (menu.ActIndex == 0x80) ? 0x08 : menu.ActIndex << 1; // Переходим к предыдущему пункту
        } while (!(menu.ActIndex & menu.MenuOutParam)); // Пропускаем скрытые пункты
    }
    void MoveDown(MenuOut& menu) {
        do
        {
            menu.ActIndex = (menu.ActIndex == 0x08) ? 0x80 : menu.ActIndex >> 1; // Переходим к следующему пункту
        } while (!(menu.ActIndex & menu.MenuOutParam)); // Пропускаем скрытые пункты
    }
    void ChangeLeft(MenuOut& menu) {}
    void ChangeRight(MenuOut& menu) {}
    Action Select(MenuOut& menu) { // Обрабатывает выбор пункта меню
        switch (menu.ActIndex)
        {
        case 0x80:
            return MainMenuAction::OpenFile; // Выбран пункт открытия файла

        case 0x40:
            return MainMenuAction::SetPercent; // Выбран процент комментариев

        case 0x20:
            return MainMenuAction::SetInterval; // Выбран интервал

        case 0x10:
            return MainMenuAction::Continue; // Выбран запуск анализа

        case 0x08:
            return MainMenuAction::Exit; // Выбран выход

        default:
            return MainMenuAction::None; // Ничего не выбрано
        }
    }
};

