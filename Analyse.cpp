#include "Main.h"
#include "Analyse.h"
#include "PrintErr.h"

void analyse(const string_info& prev_str, string_info& str_info) {
    
    recent(prev_str, str_info);
    enum class State {
        Normal,
        InQuote,
        InLongComment,
        InLineComment
    };

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

    for (size_t i = 0; i < str_info.str.size(); i++)
    {
        unsigned char ch = str_info.str[i];
        unsigned char prev = (i - 1 > 0) ? str_info.str[i - 1] : '\0';
        unsigned char next = (i + 1 < str_info.str.size()) ? str_info.str[i + 1] : '\0';
        int comment_type = 0;
        switch (state)
        {
        case State::Normal:
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
                BracketChecker(str_info, inf);
            }

            // ПОСИМВОЛЬНО
            //@, $, ` (обратный апостроф), а также кириллица (если это не комментарий/строка). INVALID_CHARACTER
            //В C++ нельзя писать a + / b или int a = = 5; (через пробел). INVALID_CONSTRUCT
            // , ) не норм
            // ; перед }
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

            break;

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
        default:
            break;
        }
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
    return ch == ')' ||ch == ']' || ch == '}';
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

void FindEndBrackets(const std::vector<string_info>& info) { 
    if (info.back().brackets.empty())
        return;
    int line = info.back().line;
    do {
        const string_info& last_str = info[line];
        errors.emplace_back(FindErrUnCloseBrack(last_str, info));
        line = errors.back().position.line - 1;

    } while (!info[line].brackets.empty());
}

inline bool IsInvalidChar(const unsigned char& ch) { //@, $, ` (обратный апостроф), а также кириллица (если это не комментарий/строка). INVALID_CHARACTER
    if (ch == '$' || ch == '@' || ch == '`')
        return 1;
    else if (ch >= 0xC0 && ch <= 0xFF || ch == 0xA8 || ch == 0xB8)
        return 1;
    else return 0;
}