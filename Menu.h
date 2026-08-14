#pragma once

#include <conio.h>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

// ============================================================
// КЛАВИАТУРА
// ============================================================

enum class key
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

inline key int_to_key(int code)
{
    switch (code)
    {
    case 72:  return key::Up;
    case 80:  return key::Down;
    case 75:  return key::Left;
    case 77:  return key::Right;
    case 13:  return key::Enter;
    case 27:  return key::Esc;
    case 224: return key::Utility;

    default:
        return key::Null;
    }
}


// ============================================================
// ДЕЙСТВИЯ МЕНЮ
// ============================================================

enum class MenuAction
{
    None,

    MoveUp,
    MoveDown,

    ChangeLeft,
    ChangeRight,

    Select,
    Exit
};


// ============================================================
// СОСТОЯНИЕ И ВНЕШНИЙ ВИД МЕНЮ
// ============================================================

struct MenuOut
{
    // Текущий выбранный пункт.
    // 0x80 = первый
    // 0x40 = второй
    // 0x20 = третий
    // ...
    unsigned char ActIndex = 0x80;

    // Текст перед меню
    std::string PreMenuMessage = "";

    // Названия пунктов
    std::vector<std::string> Menu;

    // Функции, возвращающие параметры пунктов
    std::vector<std::function<std::string()>> MenuParam;

    // Текст после меню
    std::string PostMenuMessage = "";

    // Маркеры
    std::string ActMark = "->";
    std::string InactMark = "  ";

    // Какие параметры уже установлены
    unsigned char MenuEnterParam = 0xFF;

    // Какие пункты вообще отображаются
    unsigned char MenuOutParam = 0xFF;
};


// ============================================================
// БАЗОВАЯ ЛОГИКА МЕНЮ
// ============================================================

class MenuLogic
{
public:

    virtual ~MenuLogic() = default;

    // Преобразование физической клавиши
    // в действие меню
    MenuAction GetAction(key key_code) const
    {
        switch (key_code)
        {
        case key::Up:
            return MenuAction::MoveUp;

        case key::Down:
            return MenuAction::MoveDown;

        case key::Left:
            return MenuAction::ChangeLeft;

        case key::Right:
            return MenuAction::ChangeRight;

        case key::Enter:
            return MenuAction::Select;

        case key::Esc:
            return MenuAction::Exit;

        default:
            return MenuAction::None;
        }
    }

    // Обработка обычной навигации
    void ProcessNavigation(MenuOut& menu, MenuAction action)
    {
        switch (action)
        {
        case MenuAction::MoveUp:
            MoveUp(menu);
            break;

        case MenuAction::MoveDown:
            MoveDown(menu);
            break;

        case MenuAction::ChangeLeft:
            ChangeLeft(menu);
            break;

        case MenuAction::ChangeRight:
            ChangeRight(menu);
            break;

        default:
            break;
        }
    }

    // Enter.
    // Конкретное меню само решает, что возвращать.
    virtual int Select(MenuOut& menu) = 0;

    // Вызывается перед каждым отображением.
    virtual void BeforeShow(MenuOut& menu)
    {}

protected:

    void MoveUp(MenuOut& menu)
    {
        if (menu.Menu.empty())
            return;

        do
        {
            if (menu.ActIndex == 0x80)
                menu.ActIndex = GetLastBit(menu);
            else
                menu.ActIndex <<= 1;

        } while (!(menu.ActIndex & menu.MenuOutParam));
    }


    void MoveDown(MenuOut& menu)
    {
        if (menu.Menu.empty())
            return;

        do
        {
            if (menu.ActIndex == GetLastBit(menu))
                menu.ActIndex = 0x80;
            else
                menu.ActIndex >>= 1;

        } while (!(menu.ActIndex & menu.MenuOutParam));
    }


    virtual void ChangeLeft(MenuOut&)
    {}

    virtual void ChangeRight(MenuOut&)
    {}


private:

    unsigned char GetLastBit(const MenuOut& menu) const
    {
        if (menu.Menu.empty())
            return 0;

        return static_cast<unsigned char>(
            0x80 >> (menu.Menu.size() - 1)
            );
    }
};


// ============================================================
// ОТОБРАЖЕНИЕ
// ============================================================

void show_menu(const MenuOut& menu);


// ============================================================
// ОСНОВНОЙ ЦИКЛ МЕНЮ
// ============================================================

int menu_navigation(MenuOut& menu, MenuLogic& logic);