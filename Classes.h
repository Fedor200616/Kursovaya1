#pragma once


/// <summary>
/// Класс состояний посимвольного анализатора
/// </summary>
enum class State {
    Normal,
    InQuote,
    InLongComment,
    InLineComment,
    IsNumber,
    InPreprocessor
};

/// <summary>
/// Класс всех возможных стандартов препроцессора
/// </summary>
enum class PreprocStandard
{
    NONE,
    Include,
    Define,
    Undef,
    If,
    Ifdef,
    Ifndef,
    Elif,
    Else,
    Endif,
    Error,
    Line,
    Pragma
};

/// <summary>
/// Класс всех состояний посимвольного анализатора внутри анализа препроцессора
/// </summary>
enum class PreprocState
{
    AfterHesh, // Состояние ожидания названия директивы
    InName,
    AfterName, // Состояние ожидания аргументов
    InArg,
    AfterArg, // После данного аргумента точно не должно быть еще символов
    ErrorConstr // Если была найдена ошибка, то не проверяем строку дальше во избежание мусорных предупреждений
};

