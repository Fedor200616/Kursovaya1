#include "LexerUtils.h"
#include "Analyse.h"


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
