#pragma once

#include <iostream>

#ifndef MAIN_H
#define MAIN_H

//namespace fs = std::filesystem;

/// <summary>
/// Структура информации о строке:
/// номер строки | 
/// сама строка | 
/// скобки в строках | 
/// наличие незакрытых кавычек в строке (1 - ', 2 - ") | 
/// наличие комментария в строке (1 - однострочный, 2 - многострочный) | 
/// </summary>
struct string_info {
    int str_num;
    std::string str;
	std::string brackets; // BracketChecker(str_info) - результат проверки на скобки
	int have_unclosedquote; // UnclosedquoteChecker(str_info) - результат проверки на незакрытые кавычки
	int have_unclosed_long_comment; // UnclosedLongCommentChecker(str_info) - результат проверки на незакрытый многострочный комментарий
	int have_not_eos; // NotEosChecker(str_info) - результат проверки на отсутствие точки с запятой в конце строки
	//Далее переменные относящиеся к заданию с комментариями
    int have_comment; // CommentChecker(const string_info str_info);
};

const string_info empty_str_info = { 0, "", "", 0, 0, 0, 0};

#endif

