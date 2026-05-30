#pragma once
#include <iostream>
#include <vector>
#include "main.h"
#include "PrintErr.h"


inline const char comment = '/';
inline const char long_comment = '*';
inline const char long_comment_end[] = "*/";

/// <summary>
/// проверка на комментарии
/// </summary>
/// <param name="first">Первый символ на проверку</param>
/// <param name="second">Второй символ на проверке</param>
/// <returns>0 - нет коммента; 1 - однострочечный коммент; 2 - многострочечный коммент</returns>
inline int CommentChecker(unsigned char first, unsigned char second) {
    if (first != comment) return 0;

    if (second == comment) return 1;      // //
    if (second == long_comment) return 2; // /*

    return 0;
}

/// <summary>
/// Проверка на любой тип скобок.
/// </summary>
/// <param name="ch">Символ на проверку</param>
/// <returns>Если символ - скобка - true, иначе false</returns>
inline bool IsBracket(unsigned char ch) {
    return ch == '(' || ch == ')' ||
        ch == '[' || ch == ']' ||
        ch == '{' || ch == '}';
} 

/// <summary>
/// Проверка на открытые скобки. 
/// </summary>
/// <param name="ch">Символ на проверке</param>
/// <returns>Если символ является открытой скобкой, то возвращает true, иначе false.</returns>
inline bool IsOpenBracket(unsigned char ch) {
    return ch == '(' || ch == '[' || ch == '{';
}

/// <summary>
/// Проверка на закрытые скобки.
/// </summary>
/// <param name="ch">Символ на проверку</param>
/// <returns>Если символ - закрытая скобка - true, иначе false</returns>
inline bool IsCloseBracket(unsigned char ch) {
    return ch == ')' || ch == ']' || ch == '}';
}

/// <summary>
/// Проверка на любой тип скобок.
/// </summary>
/// <param name="ch">Символ на проверку</param>
/// <returns>Если символ - скобка - true, иначе false</returns>
inline bool IsBracket(unsigned char ch); // Проверка на скобки в целом, для удобства

/// <summary>
/// Функция проверки что две скобки одного вида
/// </summary>
/// <param name="open">открывающаяся скобка</param>
/// <param name="close">закрывающаяся скобка</param>
/// <returns>1 - ели одного вида, 0 - иначе</returns>
inline bool BracketCompare(unsigned char open, unsigned char close) {
    return (open == '(' && close == ')') ||
        (open == '[' && close == ']') ||
        (open == '{' && close == '}');
}

/// <summary>
/// Проверка на кавычки.
/// </summary>
/// <param name="ch">Символ на проверке</param>
/// <returns>Если символ является кавычкой, то возвращает true, иначе false.</returns>
inline bool IsQuote(unsigned char ch) {
    if (ch == '\'' || ch == '\"') return true;
    else return false;
}

/// <summary>
/// Первая проверка ошибки закрытой скобки: на наличие подобной открытой в буфере
/// </summary>
/// <param name="bracket_buff">Буфер скобок из strin_info</param>
/// <param name="close">Скобка, которую проверяем</param>
/// <returns>1 - если в сткроке есть подобная открывающаяся скобка, ноль, если не было открытых скобок такого типа до этого</returns>
inline bool HaveSimOpenBrack(const std::vector<brack>& bracket_buff, const unsigned char close) {
    for (auto open : bracket_buff) {
        if (BracketCompare(open.bracket, close)) {
            return true;
        }
    }
    return false;
}

/// <summary>
/// Проверка, что символ не может быть внутри C++ кода
/// </summary>
/// <param name="ch">Символ</param>
/// <returns>1 если символ $ @ ` или кирилица</returns>
inline bool IsInvalidChar(const unsigned char& ch) { //@, $, ` (обратный апостроф), а также кириллица (если это не комментарий/строка). INVALID_CHARACTER
    if (ch == '$' || ch == '@' || ch == '`')
        return 1;
    else if (ch >= 0xC0 && ch <= 0xFF || ch == 0xA8 || ch == 0xB8)
        return 1;
    else return 0;
}

/// <summary>
/// Проверка символа, является лм он оператором
/// </summary>
/// <param name="ch">символ</param>
/// <returns>1 если оператор, 0 иначе</returns>
inline bool IsOperator(unsigned char ch) {
    // Строка содержит все символы, которые могут быть операторами или их частью
    static const std::string ops = "+-*/%=!<>|&^~?:.";

    // Если символ найден в строке, значит это оператор
    return ops.find(ch) != std::string::npos;
}

/// <summary>
/// Проверка на ошибки связанные с двойними операторами
/// </summary>
/// <param name="ch">последний знак</param>
/// <param name="prev">знак перед последним</param>
/// <param name="real_prev">реальный знак перед последним</param>
/// <returns>1 если есть ошибка, 0 иначе</returns>
bool binar_oprator_checker(unsigned char ch, unsigned char prev, unsigned char real_prev);