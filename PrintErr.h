#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <string>
#include "main.h"

#ifndef PRINTERR_H
#define PRINTERR_H


struct err_info
{
	enum class err_type {
        MISSING_CLOSE_BRACKET,
		UNCLOSED_BRACKET,
		UNCLOSED_QUOTE,
		UNCLOSED_LONG_COMMENT,
		NOT_EOS,
        CLOSE_BRAKET_FIRST,
        FILE_IS_EMPTY,
        UNDEFINE_ERROR
	};

	int line;
    char symbol;
	err_type type;


    static std::string message(err_type error)
    {
        switch (error)
        {
        case err_type::MISSING_CLOSE_BRACKET:
             return "Найдена незакрытая скобка";
        case err_type::UNCLOSED_QUOTE:
            return "Кавычки не закрыты в данной строке";
        case err_type::UNCLOSED_LONG_COMMENT:
            return "Длинный комментарий не имеет конца";
        case err_type::CLOSE_BRAKET_FIRST:
            return "Найдена закрывающаяся скобка без открытых";
            
        default:
            return std::to_string(static_cast<int>(error));
        }
    }
};

extern std::vector<err_info> errors;

/// <summary>
/// Функция печати ошибок
/// </summary>
/// <returns></returns>
int print_error();

/// <summary>
/// Функция обавления незакрытых кавычек в массив ошибок
/// </summary>
/// <param name="result">массив информации о файле</param>
void find_quote_err(const std::vector<string_info>& result);



#endif