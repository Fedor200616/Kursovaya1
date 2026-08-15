#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <windows.h>
#include <conio.h>

#include "Menu.h"
#include "PrintErr.h"

namespace fs = std::filesystem;

struct Settings{
    fs::path filepath = "";

    int ref_percent = 10;
    int ref_interval = 20;


    const int PERCENT_DIFF = 5;

    const int PERCENT_RANGE[2] = {0, 100};

    const std::string percent_dialog = 
        "Введите минимальный процент комментариев (от " +
        std::to_string(PERCENT_RANGE[0]) +
        " до " +
        std::to_string(PERCENT_RANGE[1]) +
        "): ";


    const int INTERVAL_DIFF = 2;

    const int INTERVAL_RANGE[2] = {9, 41};

    const std::string interval_dialog =
        "Введите интервал комментариев (от " +
        std::to_string(INTERVAL_RANGE[0]) +
        " до " +
        std::to_string(INTERVAL_RANGE[1]) +
        "): ";
};


// Глобальные настройки
inline Settings setting;


// Путь к exe
extern fs::path exe_filepath;

int GetUserInfo(int DIFF, const std::string& text, const int* interval, int user_enter);

fs::path SaveFileDialog(const fs::path& filepath);

fs::path place_to_save(const fs::path& filepath);


// ============================================================
// ГЛАВНОЕ МЕНЮ
// ============================================================

enum class MainMenuAction
{
    OpenFile,
    SetPercent,
    SetInterval,
    Continue,
    Exit
};


class MainMenuLogic : public MenuLogic
{
public:

    int Select(MenuOut& menu) override
    {
        switch (menu.ActIndex)
        {
        case 0x80:
            return static_cast<int>(
                MainMenuAction::OpenFile
                );

        case 0x40:
            return static_cast<int>(
                MainMenuAction::SetPercent
                );

        case 0x20:
            return static_cast<int>(
                MainMenuAction::SetInterval
                );

        case 0x10:
            return static_cast<int>(
                MainMenuAction::Continue
                );

        case 0x08:
            return static_cast<int>(
                MainMenuAction::Exit
                );

        default:
            return -1;
        }
    }


    void BeforeShow(MenuOut& menu) override
    {
        // Если файл выбран,
        // разрешаем остальные пункты.
        if (!setting.filepath.empty())
        {
            menu.MenuOutParam |= 0x78;
        }
    }
};


// ============================================================
// МЕНЮ СОХРАНЕНИЯ
// ============================================================

enum class SaveMenuAction
{
    None,

    SaveNearFile,
    SaveNearExe,
    SaveOpinion,

    Exit
};


class SaveMenuLogic : public MenuLogic
{
public:

    int Select(MenuOut& menu) override
    {
        switch (menu.ActIndex)
        {
        case 0x80:
            return static_cast<int>(
                SaveMenuAction::SaveNearFile
                );

        case 0x40:
            return static_cast<int>(
                SaveMenuAction::SaveNearExe
                );

        case 0x20:
            return static_cast<int>(
                SaveMenuAction::SaveOpinion
                );

        case 0x10:
            return static_cast<int>(
                SaveMenuAction::Exit
                );

        default:
            return static_cast<int>(
                SaveMenuAction::None
                );
        }
    }
};


// ============================================================
// МЕНЮ РЕЗУЛЬТАТОВ
// ============================================================

enum class ReturnMenuAction
{
    None,

    OpenErrors,
    OpenComms,
    SaveResult,

    ExitToMain,
    ExitToDesktop
};


class ReturnMenuLogic : public MenuLogic
{
public:

    int Select(MenuOut& menu) override
    {
        switch (menu.ActIndex)
        {
        case 0x80:
            return static_cast<int>(
                ReturnMenuAction::OpenErrors
                );

        case 0x40:
            return static_cast<int>(
                ReturnMenuAction::OpenComms
                );

        case 0x20:
            return static_cast<int>(
                ReturnMenuAction::SaveResult
                );

        case 0x10:
            return static_cast<int>(
                ReturnMenuAction::ExitToMain
                );

        case 0x08:
            return static_cast<int>(
                ReturnMenuAction::ExitToDesktop
                );

        default:
            return static_cast<int>(
                ReturnMenuAction::None
                );
        }
    }
};


// ============================================================
// РЕЗУЛЬТАТЫ
// ============================================================

void ReturnResult(
    const std::vector<string_info>& fileLines,
    const std::vector<err_info>& errorInfo,
    const fs::path& filepath
);