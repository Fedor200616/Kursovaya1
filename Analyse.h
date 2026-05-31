#pragma once
#include <iostream>
#include <vector>
#include "main.h"
#include "PrintErr.h"
#include "LexerUtils.h"
#include "Classes.h"

#ifndef ANALYSE_H
#define ANALYSE_H

struct NumberParam
{
    enum class type {
        Bin,
        Oct,
        Dec,
        Hex
    };

    std::string value = "";
    bool has_dot = false;
    bool in_exp = false;
    type numtype = type::Dec;

    NumberParam() {}
    NumberParam(unsigned char c, type t = type::Dec) : value(1, c), numtype(t) {
        has_dot = false;
        in_exp = false;
    }
};

struct QuoteInfo {
    unsigned char quote_char;
    int quote_pos;
    int quote_counter;

    QuoteInfo() {
        quote_char = '\0';
        quote_pos = 0;
        quote_counter = 0;
    }
    QuoteInfo(unsigned char ch, int p) : quote_char(ch), quote_pos(p), quote_counter(0) {};
};


struct Preproc {
    std::string preproc_name;
    PreprocStandard type;
	PreprocState state = PreprocState::AfterHesh;

    std::string argum_name;

    Preproc () : preproc_name(""), type(PreprocStandard::NONE), argum_name("") {};

	/// <summary>
	/// Функция записи типа, функция сама меняет положение состояние на ожидание аргумента
	/// </summary>
	void setPreproc() {
        std::string name = preproc_name;
        for (auto& c : name) c = tolower((unsigned char)c);
		if (name == "include") type = PreprocStandard::Include;
		else if (name == "define") type = PreprocStandard::Define;
		else if (name == "undef") type = PreprocStandard::Undef;
		else if (name == "if") type = PreprocStandard::If;
		else if (name == "ifdef") type = PreprocStandard::Ifdef;
		else if (name == "ifndef") type = PreprocStandard::Ifndef;
		else if (name == "elif") type = PreprocStandard::Elif;
		else if (name == "else") type = PreprocStandard::Else;
		else if (name == "endif") type = PreprocStandard::Endif;
		else if (name == "error") type = PreprocStandard::Error;
		else if (name == "line") type = PreprocStandard::Line;
		else if (name == "pragma") type = PreprocStandard::Pragma;
		else type = PreprocStandard::NONE;
        state = PreprocState::AfterName;
	}


};

struct AnalysisContext {
    string_info& str_info;
    int& i;                    // Ссылка на текущий индекс в цикле
    unsigned char ch;          // Текущий символ
    unsigned char next;        // Следующий
    unsigned char prev;        // Предыдущий (физический)
    unsigned char& real_prev;  // Последний значащий символ (ссылка)
    State& state;
    NumberParam& num;          // Параметры числа
	QuoteInfo& quote;          // Параметры кавычек
    Preproc& preproc;
    bool index_minus = false;
    
    /// <summary>
    /// Конструктор, используемы в начале цикла побуквенной проверки
    /// </summary>
    /// <param name="str">Информация о строке</param>
    /// <param name="curr_i">Ссылка на текущий индекс в цикле</param>
    /// <param name="rp">Последний значащий символ (ссылка)</param>
    /// <param name="st">Состояние на данный момент</param>
    /// <param name="n">Параметры числа</param>
    /// <param name="q">Параметры кавычек</param>
    AnalysisContext(string_info& str, int& curr_i, unsigned char& rp, State& st, NumberParam& n, QuoteInfo& q, Preproc& p)
        : str_info(str), i(curr_i), real_prev(rp), state(st), num(n), quote(q), preproc(p) {
        ch = str.str[i];
        prev = (i > 0) ? str.str[i - 1] : '\0';
        next = (i + 1 < str.str.size()) ? str.str[i + 1] : '\0';

    }

    void refresh()
    {
        ch = str_info.str[i];
        prev = (i > 0) ? str_info.str[i - 1] : '\0';
        next = (i + 1 < str_info.str.size()) ? str_info.str[i + 1] : '\0';
    }

    // Вспомогательные методы, чтобы не писать длинные пути
    void addError(err_info::err_type type) {
        errors.emplace_back(pos(str_info.line, i), ch, type);
    }
    void addError(err_info::err_type type, char symbol) {
        errors.emplace_back(pos(str_info.line, i), symbol, type);
    }
    void addError(err_info::err_type type, char symbol, int position) {
        errors.emplace_back(pos(str_info.line, position), symbol, type);
    }
    void real_prev_update() {
        if (index_minus) {
            index_minus = false;
        }
        else if (ch == long_comment_end[0] && next == long_comment_end[1]){
			real_prev = ' '; // Чтобы не было ложной ошибки при проверке на операторы после комментария (напр. "a * / b")
		}
        else {
            real_prev = (!isspace(ch)) ? ch : real_prev;
        }
    }
    void iminus() {
        i--;
        index_minus = true;
    }
    
    enum class type_of_change
    {
        Nothing,
    };


    void state_change(State newState, type_of_change toc = type_of_change::Nothing) {
        //Различные функции в зависимости от начального стейта
        if (state == State::InLongComment) {
            real_prev = ' '; // Чтобы не было ложной ошибки при проверке на операторы после комментария (напр. "a * / b")
            if (preproc.type != PreprocStandard::NONE) { // Если мы были в препроцессоре до этого, то сохраняем это состояние, так как длинный комментарий может быть внутри препроцессора
                state = State::InPreprocessor;
                return;
            }
        }

        state = newState;
    }

};

/// <summary>
/// Функция анализа строки файла
/// </summary>
/// <param name="prev">Строка перед анализируемой, дает контекст для анализа</param>
/// <param name="str_info">строка, которую анализируем</param>
void analyse(const string_info& prev, string_info& str_info);


void handleNormal(AnalysisContext& ctx);

void handleInLongComment(AnalysisContext& ctx);

void handleIsNumber(AnalysisContext& ctx);

void handleQuote(AnalysisContext& ctx);

void handlePreprocessor(AnalysisContext& ctx);

void PreprocChecker(AnalysisContext& ctx);

void FindErrorInQuote(AnalysisContext& ctx);

/// <summary>
/// Проверка скобок
/// </summary>
/// <param name="str_info">Информация о строке</param>
/// <param name="bracket">Информация о скобке</param>
void BracketChecker(string_info& str_info, const brack bracket);

/// <summary>
/// Копирует нужные в str_info параметры из prev
/// </summary>
/// <param name="prev">предыдущая строка(для копирования)</param>
/// <param name="str_info">строка для вставки</param>
void recent(const string_info& prev, string_info& str_info); //не копирует номер строки,

/// <summary>
/// Находим строку с открытой скобкой, которая не была закрыта
/// </summary>
/// <param name="str_info">информация о строке, в которой найдена некорректная закрытая скобка</param>
/// <param name="Lines">информация о всем файле, в данной программе выставляется по умолчание на глобальную переменную</param>
/// <returns></returns>
err_info FindErrUnCloseBrack(const string_info& str_info, const std::vector<string_info>& Lines = fileLines);


/// <summary>
/// Находит строки, с процентов комментов ниже заданного
/// </summary>
/// <param name="Info">Информация о строках</param>
/// <param name="ref_percent">Заданный процент коментариев</param>
/// <param name="interval">Интервал, внутри которого осуществялются проверки</param>
/// <returns>массив чисел - нумера интервалов</returns>
std::vector<comm_percent> CommPercent(const std::vector<string_info>& Info, const int ref_percent, const int interval);

/// <summary>
/// Нахождение незакрытых скобок в конце
/// </summary>
/// <param name="info">информация о строках</param>
void FindEndBrackets(const std::vector<string_info>& info);

#endif // ANALYSE_H
