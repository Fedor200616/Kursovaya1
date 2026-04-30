#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>

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
        CLOSE_BRAKET_FIRST
	};

	int line;
    char simbol;
	err_type type;

    static std::string message(err_type error)
    {
        switch (error)
        {
        case err_type::UNCLOSED_BRACKET:
            return "Unclosed bracket error";
        case err_type::UNCLOSED_QUOTE:
            return "Unclosed quote error";
        case err_type::UNCLOSED_LONG_COMMENT:
            return "Unclosed long comment error";
        case err_type::NOT_EOS:
            return "Not end of statement error";
        case err_type::CLOSE_BRAKET_FIRST:
            return "Найдена закрывающаяся скобка без открытых";
        default:
            return "Unknown error";
        }
    }
};

extern std::vector<err_info> errors;


#endif