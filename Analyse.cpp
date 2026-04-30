#include "Main.h"
#include "Analyse.h"
#include "PrintErr.h"

string_info analyse(string_info& str_info) {
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

        switch (state)
        {
        case State::Normal:
            int comment_type = CommentChecker(ch, next); // 2 - длинный, 1 - строчный, 0 - нет коммента
            if (comment_type == 2) { //Длинный коммент
                state = State::InLongComment;
                i++;
                str_info.have_comment = 2;
                str_info.have_unclosed_long_comment = 1;
                break;
            }
            else if (comment_type == 1) {//Проверка на обычный коммент
                state = State::InLineComment;
                str_info.have_comment = 1;
                break;
            }
            if (IsQuote(ch)) { // Кавычки
                state = State::InQuote;
                quote_char = ch;
                str_info.have_unclosedquote = 1;
                break;
            }
            str_info.brackets = BracketChecker(str_info, ch);
            
            break;

        case State::InQuote:
            if (ch == '\\') { // escape
                i++;
                break;
            }
            if (ch == quote_char) {
                state = State::Normal;
                str_info.have_unclosedquote = 0;
            }
            break;

        case State::InLongComment:
            if (ch == long_comment_end[0] && next == long_comment_end[1]) {
                state = State::Normal;
                i++;
                str_info.have_unclosed_long_comment = 0;
            }
            break;
        case State::InLineComment:
            return str_info;
        default:
            break;
        }
    }

    return str_info;
}

std::string BracketChecker(const string_info& str_info, const char bracket){
    std::string result = str_info.brackets;
    if (IsOpenBracket(bracket)) { // скобки
        result += bracket;
    }
    else if (IsCloseBracket(bracket)) {
        if (result.empty()) {
            errors.push_back({ str_info.line, bracket, err_info::err_type::CLOSE_BRAKET_FIRST});
            return result;
        }
        char last_bracket = result.back();
        if (BracketCompare(last_bracket, bracket)) {
            result.pop_back(); //Массив не может быть пустым из-за проверки в началае
        }
        else{
            errors.push_back({ str_info.line, bracket, err_info::err_type::MISSING_CLOSE_BRACKET });
        }
    }
    return result;
}
