#include "Main.h"
#include "Analyse.h"
#include "PrintErr.h"
#include "LexerUtils.h"

void analyse(const string_info& prev_str, string_info& str_info) {
    
    recent(prev_str, str_info);
    enum class State {
        Normal,
        InQuote,
        InLongComment,
        InLineComment,
        IsNumber
    };

    struct NumberParam
    {
        enum class type {
            Bin,
            Oct,
            Dec,
            Hex
        };

        std::string value = "";
        bool has_dot = false;
        bool In_Exp = false;
        type numtype = type::Dec;
    };
    NumberParam number;

    State state = State::Normal;
    if (str_info.have_unclosed_long_comment)
        state = State::InLongComment;
    else
        str_info.have_comment = 0;
    char quote_char = 0;
    int quote_pos = 0;
    int quote_counter = 0;

    //if (state == State::Normal)
    //    FindBrackBeforeVoid(str_info);

    unsigned char real_prev = '\0';

    for (int i = 0; i < str_info.str.size(); i++)
    {
        unsigned char ch = str_info.str[i];
        unsigned char prev = (i > 0) ? str_info.str[i - 1] : '\0';
        

        unsigned char next = (i + 1 < str_info.str.size()) ? str_info.str[i + 1] : '\0';
        int comment_type = 0;
        switch (state)
        {
        case State::Normal:
        {
            comment_type = CommentChecker(ch, next); // 2 - длинный, 1 - строчный, 0 - нет коммента
            if (comment_type == 2) { //Длинный коммент
                state = State::InLongComment;
                i++;
                str_info.have_comment = comment_type;
                str_info.have_unclosed_long_comment = 1;
                break;
            }
            else if (comment_type == 1) {//Проверка на обычный коммент
                state = State::InLineComment;
                str_info.have_comment = comment_type;
                break;
            }
            if (IsQuote(ch)) { // Кавычки
                state = State::InQuote;
                quote_char = ch;
                quote_pos = i;
                quote_counter = 0;
                break;
            }

            if (IsBracket(ch)) {
                brack inf = { ch, i }; // unsigned char -> char но проверка IsBracket должна убрать UB
                if (ch == ')' && real_prev == ',')
                    errors.emplace_back(pos(str_info.line, i), real_prev, err_info::err_type::INVALID_CONSTRUCTION);
                if (ch == '}' && real_prev == ';')
                    errors.emplace_back(pos(str_info.line, i), real_prev, err_info::err_type::INVALID_CONSTRUCTION);
                BracketChecker(str_info, inf);
            }

            if (IsOperator(ch) && IsOperator(real_prev)) {
                if (binar_oprator_checker(ch, prev, real_prev))
                    errors.emplace_back(pos(str_info.line, i), ch, err_info::err_type::INVALID_CONSTRUCTION);
            }

            // ПОСИМВОЛЬНО
            //===== @, $, ` (обратный апостроф), а также кириллица (если это не комментарий/строка). INVALID_CHARACTER
            //===== В C++ нельзя писать a + / b или int a = = 5; (через пробел). INVALID_CONSTRUCT
            //===== , ) не норм пустое условие в скобках
            //===== ; перед }
            // 
            // ТОКЕНЫ
            //В переменной инт не может быть запятой, 
            //В C++ не бывает if () или while () или for (). Внутри должно что-то быть. INVALID_CONSTRUCT 
            // В C++ операторы выше должны иметь скобки 
            // В C++ имя переменной или функции не может начинаться с цифры INVALID_IDENTIFIER
            // 
            // ПРЕДПРОЦЕССОР
            //Проверим все инклюд файлы на их наличие в директории???
            //

            if (IsInvalidChar(ch)) {
                errors.emplace_back(pos(str_info.line, i), ch, err_info::err_type::INVALID_CHARACTER);
            }

            bool is_start_of_number = isdigit(ch) && !isalpha(real_prev) && real_prev != '_';

            if (is_start_of_number) {
                if (isdigit(ch)) {
                    state = State::IsNumber;
                    number.value = ch;
                    number.has_dot = false;
                    number.In_Exp = false;
                    number.numtype = NumberParam::type::Dec;
                    if (ch == '0') {
                        switch (next) {
                        case 'x':
                        case 'X':
                            number.numtype = NumberParam::type::Hex;
                            i++;
                            break;
                        case 'b':
                        case 'B':
                            number.numtype = NumberParam::type::Bin;
                            i++;
                            break;
                        default:
                            if (isdigit(next)) {
                                number.numtype = NumberParam::type::Oct;
                            }/*
                            else {
                                errors.emplace_back(pos(str_info.line, i + 1), next, err_info::err_type::INVALID_CHARACTER);
                                state = State::Normal;
                            }*/
                            break;
                        }
                    }

                }
            }

            break;
        }
        case State::InQuote:
            if (ch == '\\') { // escape
                i++;
                quote_counter++;
                break;
            }
            if (ch == quote_char) {
                if (quote_char == '\'') {
                    if (quote_counter == 0) // нельзя '' пустые
                        errors.emplace_back(pos(str_info.line, i), ch, err_info::err_type::EMPTY_CHAR_QUOTE);
                    if (quote_counter > 1) 
                        errors.emplace_back(pos(str_info.line, i - 1), prev, err_info::err_type::TOO_LONG_CHAR_QUOTE);
                }
                state = State::Normal;
            }
            else 
                quote_counter++;
            break;

        case State::InLongComment:
            str_info.have_comment = 2;
            if (ch == long_comment_end[0] && next == long_comment_end[1]) {
                state = State::Normal;
                i++;
                str_info.have_unclosed_long_comment = 0;
            }
            break;
        case State::IsNumber:
        {
            bool is_dot = (ch == '.');
            bool is_exp = (tolower(ch) == 'e' && number.numtype == NumberParam::type::Dec);
            bool is_sign_after_exp = (number.numtype == NumberParam::type::Dec &&
                (tolower(prev) == 'e') &&
                (ch == '+' || ch == '-'));
            bool EndOfNum = !is_dot && !is_exp && !is_sign_after_exp &&
                (isspace(ch) || IsOperator(ch) || IsBracket(ch) || ch == ';' || ch == ',');
            if (EndOfNum) {
                state = State::Normal;
                i--;
                break;
            }
            bool Suffix = tolower(ch) == 'u' || 
                tolower(ch) == 'l' ||
                tolower(ch) == 'f';
            if (Suffix) {
                break;
            }

            switch (number.numtype) {
            case NumberParam::type::Bin:
                if (ch != '0' && ch != '1') {
                    errors.emplace_back(pos(str_info.line, i), ch, err_info::err_type::INVALID_CHARACTER);
                }
                break;
            case NumberParam::type::Oct:
                if (ch < '0' || ch > '7') {
                    errors.emplace_back(pos(str_info.line, i), ch, err_info::err_type::INVALID_CHARACTER);
                }
                break;
            case NumberParam::type::Dec:
                if (is_dot) {
                    if (number.has_dot || number.In_Exp) // Точка после точки или после экспоненты - ошибка
                        errors.emplace_back(pos(str_info.line, i), ch, err_info::err_type::INVALID_CHARACTER);
                    number.has_dot = true;
                }
                else if (is_exp) {
                    if (number.In_Exp) // Вторая 'e' в числе - ошибка
                        errors.emplace_back(pos(str_info.line, i), ch, err_info::err_type::INVALID_CHARACTER);
                    number.In_Exp = true;
                }
                else if (isdigit(ch) || is_sign_after_exp) {
                    // Это нормальные части числа
                }
                else {
                    // Если это не цифра, не точка, не экспонента и не суффикс - значит ошибка
                    // Например: 123a
                    errors.emplace_back(pos(str_info.line, i), ch, err_info::err_type::INVALID_CHARACTER);
                }
                break;
            case NumberParam::type::Hex:
                if (!isxdigit(ch)) {
                    errors.emplace_back(pos(str_info.line, i), ch, err_info::err_type::INVALID_CHARACTER);
                }
                break;
            default:
                errors.emplace_back(pos(str_info.line, i), ch, err_info::err_type::UNDEFINE_ERROR);
                break;

            }

            break;
        }
        default:
            break;
        }

        
        real_prev = (!iswspace(ch)) ? ch : real_prev;
    } // Выход из цикла


    if (state == State::InQuote)
        switch (quote_char)
        {
        case '\'':
            str_info.have_unclosedquote = 1;
            errors.emplace_back(pos(str_info.line, quote_pos), '\'', err_info::err_type::UNCLOSED_QUOTE);
            break;
        case '\"':
            str_info.have_unclosedquote = 2;
            errors.emplace_back(pos(str_info.line, quote_pos), '\"', err_info::err_type::UNCLOSED_QUOTE);
            break;
        default:
            break;
        }
    else
        str_info.have_unclosedquote = 0;

    if (str_info.line == fileLines.back().line) // Проверка что мы в конце файла
        if(state == State::InLongComment) // Длинный коммент не закрыт
            errors.emplace_back(pos(str_info.line), ' ', err_info::err_type::UNCLOSED_LONG_COMMENT);
    
}

void BracketChecker(string_info& str_info, const brack bracket) {
    int line = str_info.line;
    std::vector<brack>& result = str_info.brackets;
    if (IsOpenBracket(bracket.bracket)) {
        result.push_back(bracket);
    }
    else if (IsCloseBracket(bracket.bracket)) {
        if (result.empty()) {
            errors.emplace_back( pos(str_info.line, bracket.position), bracket.bracket, err_info::err_type::CLOSE_BRAKET_FIRST );
            return;
        }
        char last_open = result.back().bracket;
        if (BracketCompare(last_open, bracket.bracket)) {
            result.pop_back();
        }
        else{
            if (!HaveSimOpenBrack(result, bracket.bracket)) { //Проверяем на такую же открывающуюся во всем массиве скобок
                // В буфере нет скобок такого типа
                errors.emplace_back( pos(str_info.line, bracket.position), bracket.bracket, err_info::err_type::CLOSE_BRAKET_FIRST );
            }
            else {
                // есть скобка такого типа, значит порядок нарушен
                errors.emplace_back(FindErrUnCloseBrack(str_info));
                result.pop_back(); //удаляем лишнюю открытую скобку
                result.pop_back(); //удаляем скобку, которая закрывалась
            }
        }
    }
}

void recent(const string_info& prev, string_info& str_info) {
    str_info.brackets = prev.brackets;
    str_info.have_unclosed_long_comment = prev.have_unclosed_long_comment;
    if (prev.have_comment == 2)
        str_info.have_comment = 2;
}

std::vector<comm_percent> CommPercent(const std::vector<string_info>& Info, const int ref_percent, const int interval) {
    std::vector<comm_percent> not_comp_inter;
    for (int i = 0; i < Info.size() / interval; i++) {
        int count = 0;
        for (int j = (interval * i) + 1; j <= interval * (i + 1) && j < Info.size(); j++) {
            //if (j == 0);
            if (Info[j].have_comment != 0)
                count++;
        }
        int real_size = std::min(interval, (int)Info.size() - i * interval);
        int percent = count * 100 / real_size;
        if (percent < ref_percent) {
            not_comp_inter.push_back({i, percent});
        }
    }
    return not_comp_inter;
}

err_info FindErrUnCloseBrack(const string_info& str_info, const std::vector<string_info>& Lines) {
    if (Lines.empty()) {
        return { pos(- 1), ' ', err_info::err_type::UNDEFINE_ERROR};
    }

    for (int i = str_info.line; i > 0; i--) {
        if (Lines[i].brackets == str_info.brackets && Lines[i - 1].brackets != str_info.brackets)
            if (Lines[i].brackets.size() > Lines[i - 1].brackets.size())
                return err_info(pos(Lines[i].line, Lines[i].brackets.back().position),
                Lines[i].brackets.back().bracket, err_info::err_type::UNCLOSED_BRACKET);
    }
    return err_info( pos(str_info.line, str_info.brackets.back().position), 
        str_info.brackets.back().bracket, err_info::err_type::UNCLOSED_BRACKET);
}


