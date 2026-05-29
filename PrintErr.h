#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include "main.h"

#ifndef PRINTERR_H
#define PRINTERR_H

struct pos {
    int line;
    int num;

    pos(int l, int n = 0) : line(l), num(n) {}

};

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
        UNDEFINE_ERROR,
        INVALID_CHARACTER,
        EMPTY_CHAR_QUOTE,
        TOO_LONG_CHAR_QUOTE,
        INVALID_CONSTRUCTION
	};

    pos position;

    char symbol;
	err_type type;


    static std::string message(err_type error)
    {
        switch (error)
        {
        case err_type::MISSING_CLOSE_BRACKET:
        case err_type::UNCLOSED_BRACKET:
            return "Найдена незакрытая скобка";
        case err_type::UNCLOSED_QUOTE:
            return "Кавычки не закрыты в данной строке";
        case err_type::UNCLOSED_LONG_COMMENT:
            return "Длинный комментарий не имеет конца";
        case err_type::CLOSE_BRAKET_FIRST:
            return "Найдена закрывающаяся скобка без открытых";
        case err_type::INVALID_CHARACTER:
            return "Данный символ не может использоваться в синтаксисе";
        case err_type::EMPTY_CHAR_QUOTE:
            return "Пустые символьные кавычки";
        case err_type::TOO_LONG_CHAR_QUOTE:
            return "Больше одного символа в символьных ковычках";
        case err_type::INVALID_CONSTRUCTION:
            return "Неправильно составленная конструкция";
            
        default:
            return std::to_string(static_cast<int>(error));
        }
    }

    err_info(pos p, char s, err_type t) : position(p), symbol(s), type(t) {}

};

extern std::vector<err_info> errors;

struct comm_percent {
    int interval;
    int percent;
};

/// <summary>
/// Функция печати ошибок
/// </summary>
/// <param name="errors">Массив ошибок типа err_info</param>
/// <returns>0</returns>
int print_error(const std::vector<err_info>& errors = ::errors);

/// <summary>
/// Выводит на экран интервалы с малым количеством комментариев
/// </summary>
/// <param name="intervals">массив интервалов для вывода</param>
/// <param name="interval_size">размер интервала</param>
/// <param name="fileinfosize">длина файла</param>
/// <returns>0</returns>
int CommPercentPrint(const std::vector<comm_percent>& comm_vec, int interval_size, int fileinfosize);

/// <summary>
/// Функция удаления пробелов и табуляции перед строкой для краивого вывода
/// </summary>
/// <param name="s">строка из анализируемой программы</param>
/// <returns>строку без табуляции или пробелов перед ней</returns>
std::string delete_tab(const std::string& s);

#endif