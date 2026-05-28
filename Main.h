#include <iostream>
#include <string>
#include <vector>

#ifndef MAIN_H
#define MAIN_H

//namespace fs = std::filesystem;

struct brack {
		char bracket;
		unsigned int position;

		bool operator==(const brack& other) const {
			return (bracket == other.bracket && position == other.position);
		}

};

/// <summary>
/// Структура информации о строке:
/// номер строки | 
/// сама строка | 
/// скобки в строках | 
/// наличие незакрытых кавычек в строке (1 - ', 2 - ") | 
/// наличие комментария в строке (1 - однострочный, 2 - многострочный) | 
/// </summary>
struct string_info {
    int line;
    std::string str;
	
	std::vector<brack> brackets; // BracketChecker(str_info) - результат проверки на скобки

	int have_unclosedquote; // UnclosedquoteChecker(str_info) - результат проверки на незакрытые кавычки
	int have_unclosed_long_comment; // UnclosedLongCommentChecker(str_info) - результат проверки на незакрытый многострочный комментарий
	//Далее переменные относящиеся к заданию с комментариями
    int have_comment; // CommentChecker(const string_info str_info);

};

const string_info empty_str_info{};

extern std::vector<string_info> fileLines;

#endif

