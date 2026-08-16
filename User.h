#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <windows.h>
#include <conio.h>

#include "Menu.h"
#include "PrintErr.h"

namespace fs = std::filesystem;

struct Settings {
    fs::path filepath = "";

    int ref_percent = 10;
    int ref_interval = 20;


    const int PERCENT_DIFF = 5;

    const int PERCENT_RANGE[2] = { 0, 100 };

    const std::string percent_dialog =
        "Введите минимальный процент комментариев (от " +
        std::to_string(PERCENT_RANGE[0]) +
        " до " +
        std::to_string(PERCENT_RANGE[1]) +
        "): ";


    const int INTERVAL_DIFF = 2;

    const int INTERVAL_RANGE[2] = { 9, 41 };

    const std::string interval_dialog =
        "Введите интервал комментариев (от " +
        std::to_string(INTERVAL_RANGE[0]) +
        " до " +
        std::to_string(INTERVAL_RANGE[1]) +
        "): ";
};


// Глобальные настройки
inline Settings setting;

enum class ChangeMenuAction {
    None,

    ChangeNumLeft,
    ChangeNumRight,
    Enter,
    Cancel
};

class ChangeMenuLogic : public MenuLogic {
public:

    int Select(MenuOut& menu) override {
        switch (menu.ActIndex) {
        case 0x40:
            return static_cast<int>(ChangeMenuAction::Enter);
        case 0x20:
            return static_cast<int>(ChangeMenuAction::Cancel);
        }
    }

    int ChangeLeft(MenuOut& menu) override {
        if (menu.ActIndex & 0x80) {
            return static_cast<int>(ChangeMenuAction::ChangeNumLeft);
        }
        else
            return static_cast<int>(ChangeMenuAction::None);
    }

    int ChangeRight(MenuOut& menu) override {
        if (menu.ActIndex & 0x80) {
            return static_cast<int>(ChangeMenuAction::ChangeNumRight);
        }
        else
            return static_cast<int>(ChangeMenuAction::None);
    }
};

/// <summary>
/// Функция меню изменения параметров оценивания комментариев
/// </summary>
/// <param name="ChangeType">Тип параметра (только Percent или Interval)</param>
/// <param name="set">Обьект настроек программы</param>
/// <returns>Измененное значение</returns>
int ChangeMenuDialog(const std::string ChangeType, const Settings& set);

fs::path SaveFileDialog(const fs::path& filepath);

fs::path place_to_save(const fs::path& filepath);

/// <summary>
/// Действия в главном меню
/// </summary>
enum class MainMenuAction {
    OpenFile,
    SetPercent,
    SetInterval,
    Continue,
    Exit
};

/// <summary>
/// Логика главного меню
/// </summary>
class MainMenuLogic : public MenuLogic {
public:

    int Select(MenuOut& menu) override {
        switch (menu.ActIndex) {
        case 0x80:
            return static_cast<int>(MainMenuAction::OpenFile);

        case 0x40:
            return static_cast<int>(MainMenuAction::SetPercent);

        case 0x20:
            return static_cast<int>(MainMenuAction::SetInterval);

        case 0x10:
            return static_cast<int>(MainMenuAction::Continue);

        case 0x08:
            return static_cast<int>(MainMenuAction::Exit);

        default:
            return -1;
        }
    }

    void BeforeShow(MenuOut& menu) override {
        // Если файл выбран,
        // разрешаем остальные пункты.
        if (!setting.filepath.empty()) //Введен файл
        {
            menu.MenuEnterParam |= 0x80;
        }
        else
            menu.MenuEnterParam &= ~0x80;

        if (menu.MenuEnterParam & 0x80) { //Показываем настройки комментариев
            menu.MenuOutParam |= 0x60;
        }
        else
            menu.MenuOutParam &= ~0x60;

        if ((menu.MenuEnterParam & 0xE0) == 0xE0) { //Введены все три поля
            menu.MenuOutParam |= 0x10;
        }
        else {
            menu.MenuOutParam &= ~0x10;
        }

    }
};

/// <summary>
/// Действия, доступные при выборе способа экспорта в файл
/// </summary>
enum class SaveMenuAction {
    None,

    SaveNearFile,
    SaveNearExe,
    SaveOpinion,

    Exit
};

/// <summary>
/// Логика меню способа экспорта
/// </summary>
class SaveMenuLogic : public MenuLogic {
public:

    int Select(MenuOut& menu) override {
        switch (menu.ActIndex) {
        case 0x80:
            return static_cast<int>(SaveMenuAction::SaveNearFile);

        case 0x40:
            return static_cast<int>(SaveMenuAction::SaveNearExe);

        case 0x20:
            return static_cast<int>(SaveMenuAction::SaveOpinion);

        case 0x10:
            return static_cast<int>(SaveMenuAction::Exit);

        default:
            return static_cast<int>(SaveMenuAction::None);
        }
    }
};

/// <summary>
/// Действия в меню вывода результатов
/// </summary>
enum class ReturnMenuAction {
    None,

    OpenErrors,
    OpenComms,
    SaveResult,

    ExitToMain,
    ExitToDesktop
};

/// <summary>
/// Логика меню вывода результатов
/// </summary>
class ReturnMenuLogic : public MenuLogic {
public:

    int Select(MenuOut& menu) override {
        switch (menu.ActIndex) {
        case 0x80:
            return static_cast<int>(ReturnMenuAction::OpenErrors);

        case 0x40:
            return static_cast<int>(ReturnMenuAction::OpenComms);

        case 0x20:
            return static_cast<int>(ReturnMenuAction::SaveResult);

        case 0x10:
            return static_cast<int>(ReturnMenuAction::ExitToMain);

        case 0x08:
            return static_cast<int>(ReturnMenuAction::ExitToDesktop);

        default:
            return static_cast<int>(ReturnMenuAction::None);
        }
    }
};


void ReturnResult(const std::vector<string_info>& fileLines, const std::vector<err_info>& errorInfo, const fs::path& filepath);