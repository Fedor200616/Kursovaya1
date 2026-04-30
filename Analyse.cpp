#include "Main.h"
#include "Analyse.h"
#include "PrintErr.h"

string_info analyse(const string_info& prev) {
    string_info str_info = prev;
    enum class State {
        Normal,
        InQuote,
        InLongComment,
        InLineComment
    };
    
    State state = State::Normal;
    if (str_info.have_unclosed_long_comment)
        state = State::InLongComment;
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
                break;
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
        str_info.have_unclosedquote = 1;
    else
        str_info.have_unclosedquote = 0;

    return str_info;
}

void BracketChecker(string_info& str_info, const char bracket){
    std::string& result = str_info.brackets;
    if (IsOpenBracket(bracket)) { // скобки
        result += bracket;
    }
    else if (IsCloseBracket(bracket)) {
        if (result.empty()) {
            errors.push_back({ str_info.line, bracket, err_info::err_type::CLOSE_BRAKET_FIRST});
            return;
        }

        char last_bracket = result.back();

        if (BracketCompare(last_bracket, bracket)) {
            result.pop_back(); //Массив не может быть пустым из-за проверки в началае
        }
        else{
            errors.push_back({ str_info.line, bracket, err_info::err_type::MISSING_CLOSE_BRACKET });
        }
    }
}

int CommentChecker(char first, char second) {
    if (first != comment) return 0;

    if (second == comment) return 1;      // //
    if (second == long_comment) return 2; // /*

    return 0;
}

bool IsQuote(char ch) {
    if (ch == '\'' || ch == '\"') return true;
    else return false;
}

bool IsOpenBracket(char ch) {
    for (auto& b : open_brackets) {
        if (ch == b) return true;
    }
    return false;
}

bool IsCloseBracket(char ch) {
    for (auto& b : close_brackets) {
        if (ch == b) return true;
    }
    return false;
}

bool IsBracket(char ch) {
    for (auto &b : brackets) {
        if (ch == b) return true;
    }
    return false;
}

bool BracketCompare(char open, char close) {
    return (open == '(' && close == ')') ||
        (open == '[' && close == ']') ||
        (open == '{' && close == '}');
}

