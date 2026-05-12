#include "Main.h"
#include "Analyse.h"
#include "PrintErr.h"

void analyse(const string_info& prev, string_info& str_info) {
    recent(prev, str_info);
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

    for (size_t i = 0; i < str_info.str.size(); i++)
    {
        char ch = str_info.str[i];

        char next = (i + 1 < str_info.str.size()) ? str_info.str[i + 1] : '\0';
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
                return;
            }
            if (IsQuote(ch)) { // Кавычки
                state = State::InQuote;
                quote_char = ch;
                break;
            }

            if(IsBracket(ch))
                BracketChecker(str_info, ch);
            
            break;

        case State::InQuote:
            if (ch == '\\') { // escape
                i++;
                break;
            }
            if (ch == quote_char) {
                state = State::Normal;
            }
            break;

        case State::InLongComment:
            if (ch == long_comment_end[0] && next == long_comment_end[1]) {
                state = State::Normal;
                i++;
                str_info.have_unclosed_long_comment = 0;
            }
            break;
        default:
            break;
        }
    }
    if (state == State::InQuote)
        switch (quote_char)
        {
        case '\'':
            str_info.have_unclosedquote = 1;
            errors.push_back({ str_info.line, '\'', err_info::err_type::UNCLOSED_QUOTE});
            break;
        case '\"':
            str_info.have_unclosedquote = 2;
            errors.push_back({ str_info.line, '\"', err_info::err_type::UNCLOSED_QUOTE});
            break;
        default:
            break;
        }
    else
        str_info.have_unclosedquote = 0;

    if (str_info.line == fileLines.back().line)
        if(state == State::InLongComment)
            errors.push_back({ str_info.line, ' ', err_info::err_type::UNCLOSED_LONG_COMMENT});
}

void BracketChecker(string_info& str_info, const char bracket) {
    std::string& result = str_info.brackets;
    if (IsOpenBracket(bracket)) {
        result += bracket;
    }
    else if (IsCloseBracket(bracket)) {
        if (result.empty()) {
            errors.push_back({ str_info.line, bracket, err_info::err_type::CLOSE_BRAKET_FIRST });
            return;
        }
        char last_open = result.back();
        if (BracketCompare(last_open, bracket)) {
            result.pop_back();
        }
        else{
            if (!HaveSimOpenBrack(result, bracket)) {
                // В буфере нет скобок такого типа
                errors.push_back({ str_info.line, bracket, err_info::err_type::CLOSE_BRAKET_FIRST });
            }
            else {
                // есть скобка такого типа, значит порядок нарушен
                errors.push_back(FindErrUnCloseBrack(str_info));
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

inline int CommentChecker(char first, char second) {
    if (first != comment) return 0;

    if (second == comment) return 1;      // //
    if (second == long_comment) return 2; // /*

    return 0;
}

inline bool IsQuote(char ch) {
    if (ch == '\'' || ch == '\"') return true;
    else return false;
}

inline bool IsOpenBracket(char ch) {
        return ch == '(' || ch == '[' || ch == '{';
}

inline bool IsCloseBracket(char ch) {
    return ch == ')' ||ch == ']' || ch == '}';
}

inline bool IsBracket(char ch) {
    return ch == '(' || ch == ')' ||
        ch == '[' || ch == ']' ||
        ch == '{' || ch == '}';
}

inline bool BracketCompare(char open, char close) {
    return (open == '(' && close == ')') ||
        (open == '[' && close == ']') ||
        (open == '{' && close == '}');
}

inline bool HaveSimOpenBrack(const std::string& bracket_buff, const char close) { 
    for (auto open : bracket_buff) { 
        if (BracketCompare(open, close)) { 
            return true; 
        } 
    } 
    return false; 
}

std::vector<int> CommPercent(const std::vector<string_info>& Info, const int ref_percent, const int interval) {
    std::vector<int> not_comp_inter;
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
            not_comp_inter.push_back(i);
        }
    }
    return not_comp_inter;
}

err_info FindErrUnCloseBrack(const string_info& str_info, const std::vector<string_info>& Lines) {
    if (Lines.empty()) {
        return { -1, ' ', err_info::err_type::UNDEFINE_ERROR};
    }
    for (int i = str_info.line; i > 0; i--) {
        if (Lines[i].brackets == str_info.brackets && Lines[i - 1].brackets != str_info.brackets)
            if (Lines[i].brackets > Lines[i - 1].brackets)
                return { Lines[i].line, Lines[i].brackets.back(), err_info::err_type::UNCLOSED_BRACKET };
    }
    return { str_info.line, str_info.brackets.back(), err_info::err_type::UNCLOSED_BRACKET };
}