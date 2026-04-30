#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>

#ifndef PRINTERR_H
#define PRINTERR_H


struct err_info
{
	enum class err_type {
		UNCLOSED_BRACKET = 0,
		UNCLOSED_QUOTE = 1,
		UNCLOSED_LONG_COMMENT = 2,
		NOT_EOS = 3
	};

	int line;
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
        default:
            return "Unknown error";
        }
    }
};

extern std::vector<err_info> errors;


#endif