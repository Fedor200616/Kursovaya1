#pragma once

#include <conio.h>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

/// <summary>
/// Клавиши, которыми пользуеся клиент
/// </summary>
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

/// <summary>
/// Перевод из числового значения от getch в enum class key
/// </summary>
/// <param name="code">код клавиши</param>
/// <returns></returns>
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

/// <summary>
/// Основные функции в меню
/// </summary>
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

/// <summary>
/// Структура меню
/// </summary>
struct MenuOut
{
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
    std::vector<std::function<std::string()>> MenuParam; // Пишем лямбда функции внутрь для вывода чисел и других изменяемых параметров

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

/// <summary>
/// Базовая логика меню
/// </summary>
class MenuLogic //Все экшены меню 0 пунктом возвращают None!!!
{
public:

    virtual ~MenuLogic() = default;

    // Преобразование физической клавиши
    // в действие меню
    MenuAction GetAction(key key_code) const{
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
    int ProcessNavigation(MenuOut& menu, MenuAction action) {
        switch (action)
        {
        case MenuAction::MoveUp:
            MoveUp(menu);
            return 0;
            break;

        case MenuAction::MoveDown:
            MoveDown(menu);
            return 0;
            break;

        case MenuAction::ChangeLeft:
            return ChangeLeft(menu);
            break;

        case MenuAction::ChangeRight:
            return ChangeRight(menu);
            break;

        default:
            return 0;
        }
    }

    // Действия при выборе и нажатии Enter
    virtual int Select(MenuOut& menu) = 0;

    // Вызывается перед каждым отображением.
    virtual void BeforeShow(MenuOut& menu){}
    //функции изменяемы в наследниках
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

    virtual int ChangeLeft(MenuOut&)
    {
        return 0;
    }

    virtual int ChangeRight(MenuOut&)
    {
        return 0;
    }

private:

    /// <summary>
    /// Определяет, какой бит соответствует последнему пункту меню
    /// </summary>
    /// <param name="menu">Меню</param>
    /// <returns>Битовая маска последнего пункта или 0 для пустого меню</returns>
    unsigned char GetLastBit(const MenuOut& menu) const
    {
        if (menu.Menu.empty())
            return 0;

        return static_cast<unsigned char>(
            0x80 >> (menu.Menu.size() - 1)
            );
    }
};

/// <summary>
/// Отображает меню в консоли
/// </summary>
/// <param name="menu">объект меню для отображения</param>
void show_menu(const MenuOut& menu);

/// <summary>
/// Навигация по меню (от показа меню до выбора пользователем действия)
/// </summary>
/// <param name="menu">Обьект меню</param>
/// <param name="logic">Логика конкретного меню</param>
/// <returns>-1 для выхода, logic.Select(menu) для enter и, как правило, 0 для остального</returns>
int menu_navigation(MenuOut& menu, MenuLogic& logic);

