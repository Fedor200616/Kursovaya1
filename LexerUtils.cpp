#include "LexerUtils.h"
#include "Analyse.h"

inline int CommentChecker(unsigned char first, unsigned char second) {
    if (first != comment) return 0;

    if (second == comment) return 1;      // //
    if (second == long_comment) return 2; // /*

    return 0;
}

inline bool IsQuote(unsigned char ch) {
    if (ch == '\'' || ch == '\"') return true;
    else return false;
}

inline bool IsOpenBracket(unsigned char ch) {
    return ch == '(' || ch == '[' || ch == '{';
}

inline bool IsCloseBracket(unsigned char ch) {
    return ch == ')' || ch == ']' || ch == '}';
}

inline bool IsBracket(unsigned char ch) {
    return ch == '(' || ch == ')' ||
        ch == '[' || ch == ']' ||
        ch == '{' || ch == '}';
}

inline bool BracketCompare(unsigned char open, unsigned char close) {
    return (open == '(' && close == ')') ||
        (open == '[' && close == ']') ||
        (open == '{' && close == '}');
}

inline bool HaveSimOpenBrack(const std::vector<brack>& bracket_buff, const unsigned char close) {
    for (auto open : bracket_buff) {
        if (BracketCompare(open.bracket, close)) {
            return true;
        }
    }
    return false;
}


bool IsOperator(unsigned char ch) {
    // Строка содержит все символы, которые могут быть операторами или их частью
    static const std::string ops = "+-*/%=!<>|&^~?:.";

    // Если символ найден в строке, значит это оператор
    return ops.find(ch) != std::string::npos;
}

bool binar_oprator_checker(unsigned char ch, unsigned char prev, unsigned char real_prev) {
    bool hadSpace = (prev != real_prev);

    if (hadSpace) {

        if (ch == '=') {
            if (std::string("+-*/%!=<>").find(real_prev) != std::string::npos) return 1; // ОШИБКА: Разрыв составного оператора пробелом (> =, + =, : :)
        }
        if (real_prev == '+' && ch == '+') return 1;
        if (real_prev == '-' && ch == '-') return 1;
        if (real_prev == '-' && ch == '>') return 1;
        if (real_prev == '&' && ch == '&') return 1;
        if (real_prev == '|' && ch == '|') return 1;
        if (real_prev == ':' && ch == ':') return 1;
        if (real_prev == '<' && ch == '<') return 1;
        if (real_prev == '>' && ch == '>') return 1;

        return 0; // В остальных случаях пробел между знаками (напр. "a + -1") допустим
    }



    switch (real_prev) { // Если знаки стоят вплотную, проверяем на допустимость комбинации
        // Разрешенные пары (Белый список)
    case '+': if (ch == '+' || ch == '=') return 0; break;
    case '-': if (ch == '-' || ch == '=' || ch == '>') return 0; break;
    case '*': if (ch == '=' || ch == '*') return 0; break;
    case '/': if (ch == '=') return 0; break;
    case '=': if (ch == '=') return 0; break;
    case '!': if (ch == '=') return 0; break;
    case '<': if (ch == '=' || ch == '<') return 0; break;
    case '>': if (ch == '=' || ch == '>') return 0; break;
    case '&': if (ch == '=' || ch == '&') return 0; break;
    case '|': if (ch == '=' || ch == '|') return 0; break;
    case '%': if (ch == '=') return 0; break;
    case '^': if (ch == '=') return 0; break;
    case ':': if (ch == ':') return 0; break;
    default: break;
    }

    // Разрешаем унарные операторы, которые могут стоять ПОСЛЕ других операторов (напр. "a * -b", "if (!a)")
    // Также разрешаем амперсанд и звездочку для указателей/адресов
    if (std::string("+-!&*").find(ch) != std::string::npos) return 0;

    return 1; // Все остальное (напр. "+ /") - ошибка
}


inline bool IsInvalidChar(const unsigned char& ch) { //@, $, ` (обратный апостроф), а также кириллица (если это не комментарий/строка). INVALID_CHARACTER
    if (ch == '$' || ch == '@' || ch == '`')
        return 1;
    else if (ch >= 0xC0 && ch <= 0xFF || ch == 0xA8 || ch == 0xB8)
        return 1;
    else return 0;
}