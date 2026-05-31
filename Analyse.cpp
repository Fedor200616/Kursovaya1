#include "Main.h"
#include "Analyse.h"
#include "PrintErr.h"
#include "LexerUtils.h"

void analyse(const string_info& prev_str, string_info& str_info) {
    
    recent(prev_str, str_info);
    
    State state = State::Normal;
    if (str_info.have_unclosed_long_comment)
        state = State::InLongComment;
    else
        str_info.have_comment = 0;

    unsigned char real_prev = '\0';
    NumberParam numparam = NumberParam();
    QuoteInfo quote_info = QuoteInfo();
    Preproc preproc = Preproc();

    int i = 0;
    AnalysisContext context = AnalysisContext(str_info, i, real_prev, state, numparam, quote_info, preproc);

    for (; i < str_info.str.size(); i++)
    {   
        context.refresh();

        int comment_type = 0;
        switch (state)
        {
        case State::Normal:
            handleNormal(context);
            break;
        case State::InQuote:
            handleQuote(context);
            break;

        case State::InLongComment:
            handleInLongComment(context);
            break;
        case State::IsNumber:
            handleIsNumber(context);
            break;
        case State::InPreprocessor:
            handlePreprocessor(context);
            break;
        default:
            break;
        }

        
        context.real_prev_update();
    } // Выход из цикла

    FindErrorInQuote(context);

    if (context.state == State::InPreprocessor) {
        PreprocChecker(context);
    }

    if (str_info.line == fileLines.back().line) // Проверка что мы в конце файла
        if(state == State::InLongComment) // Длинный коммент не закрыт
            errors.emplace_back(pos(str_info.line), ' ', err_info::err_type::UNCLOSED_LONG_COMMENT);
    
}

void handleNormal(AnalysisContext& ctx) {
    int comment_type = CommentChecker(ctx.ch, ctx.next); // 2 - длинный, 1 - строчный, 0 - нет коммента
    if (comment_type == 2) { //Длинный коммент
        ctx.state = State::InLongComment;
        ctx.i++;
        ctx.str_info.have_comment = comment_type;
        ctx.str_info.have_unclosed_long_comment = 1;
        return;
    }
    else if (comment_type == 1) {//Проверка на обычный коммент
        ctx.state = State::InLineComment;
        ctx.str_info.have_comment = comment_type;
        return;
    }
    if (IsQuote(ctx.ch)) { // Кавычки
        ctx.state = State::InQuote;
        ctx.quote = QuoteInfo(ctx.ch, ctx.i);
        return;
    }

    if (IsBracket(ctx.ch)) {
        brack inf = { ctx.ch, ctx.i }; // unsigned char -> char но проверка IsBracket должна убрать UB
        if (ctx.ch == ')' && ctx.real_prev == ',')
            ctx.addError(err_info::err_type::INVALID_CONSTRUCTION, ctx.real_prev);
        if (ctx.ch == '}' && ctx.real_prev == ';')
            ctx.addError(err_info::err_type::INVALID_CONSTRUCTION, ctx.real_prev);
        BracketChecker(ctx.str_info, inf);
    }

    if (IsOperator(ctx.ch) && IsOperator(ctx.real_prev)) {
        if (binar_oprator_checker(ctx.ch, ctx.prev, ctx.real_prev))
            ctx.addError(err_info::err_type::INVALID_CONSTRUCTION);
    }

    // ПОСИМВОЛЬНО
    //===== @, $, ` (обратный апостроф), а также кириллица (если это не комментарий/строка). INVALID_CHARACTER
    //===== В C++ нельзя писать a + / b или int a = = 5; (через пробел). INVALID_CONSTRUCT
    //===== , ) не норм пустое условие в скобках
    //===== ; перед }
    // 
    // ТОКЕНЫ
    //В переменной инт не может быть запятой, 
    //В C++ не бывает if () или while () или for (). Внутри должно что-то быть. INVALID_CONSTRUCT 
    // В C++ операторы выше должны иметь скобки 
    // В C++ имя переменной или функции не может начинаться с цифры INVALID_IDENTIFIER
    // 
    // ПРЕДПРОЦЕССОР
    //Проверим все инклюд файлы на их наличие в директории???
    //

    if (IsInvalidChar(ctx.ch)) {
        errors.emplace_back(pos(ctx.str_info.line, ctx.i), ctx.ch, err_info::err_type::INVALID_CHARACTER);
    }

    if (ctx.ch == '#') {
        if (ctx.real_prev == '\0')
            ctx.state_change(State::InPreprocessor);
        else
            ctx.addError(err_info::err_type::INVALID_CHARACTER);
    }

    bool is_start_of_number = isdigit(ctx.ch) && !isalpha(ctx.real_prev) && ctx.real_prev != '_';

    if (is_start_of_number) {
        if (isdigit(ctx.ch)) {
            ctx.state = State::IsNumber;
            if (ctx.ch == '0') {
                switch (ctx.next) {
                case 'x':
                case 'X':
                    ctx.num = NumberParam(ctx.ch, NumberParam::type::Hex);
                    ctx.i++;
                    break;
                case 'b':
                case 'B':
                    ctx.num = NumberParam(ctx.ch, NumberParam::type::Bin);
                    ctx.i++;
                    break;
                default:
                    if (isdigit(ctx.next)) {
                        ctx.num = NumberParam(ctx.ch, NumberParam::type::Oct);
                    }
                    else {
                        ctx.num = NumberParam(ctx.ch, NumberParam::type::Dec);
                    }
                    break;
                }
            }
            else 
                ctx.num = NumberParam(ctx.ch, NumberParam::type::Dec);
        }
    }
}

void handleQuote(AnalysisContext& ctx) {
    if (ctx.ch == '\\') { // escape
        ctx.i++;
        ctx.quote.quote_counter++;
        return;
    }
    if (ctx.ch == ctx.quote.quote_char) {
        if (ctx.quote.quote_char == '\'') {
            if (ctx.quote.quote_counter == 0) // нельзя '' пустые
                ctx.addError(err_info::err_type::EMPTY_CHAR_QUOTE);
            if (ctx.quote.quote_counter > 1)
                errors.emplace_back(pos(ctx.str_info.line, ctx.i - 1), ctx.prev, err_info::err_type::TOO_LONG_CHAR_QUOTE);
        }
        ctx.state = State::Normal;
    }
    else
        ctx.quote.quote_counter++;
}

void handleInLongComment(AnalysisContext& ctx) {
    ctx.str_info.have_comment = 2;
    if (ctx.ch == long_comment_end[0] && ctx.next == long_comment_end[1]) {
        ctx.state = State::Normal;
        ctx.i++;
        ctx.str_info.have_unclosed_long_comment = 0;
    }
}

void handleIsNumber(AnalysisContext& ctx) {
    bool is_dot = (ctx.ch == '.');
    bool is_exp = (tolower(ctx.ch) == 'e' && ctx.num.numtype == NumberParam::type::Dec);
    bool is_sign_after_exp = (ctx.num.numtype == NumberParam::type::Dec &&
        (tolower(ctx.prev) == 'e') &&
        (ctx.ch == '+' || ctx.ch == '-'));
    bool EndOfNum = !is_dot && !is_exp && !is_sign_after_exp &&
        (isspace(ctx.ch) || IsOperator(ctx.ch) || IsBracket(ctx.ch) || ctx.ch == ';' || ctx.ch == ',');
    if (EndOfNum) {
        ctx.state = State::Normal;
        ctx.i--;
        return;
    }
    bool Suffix = tolower(ctx.ch) == 'u' ||
        tolower(ctx.ch) == 'l' ||
        tolower(ctx.ch) == 'f';
    if (Suffix) {
        return;
    }
    
    switch (ctx.num.numtype) {
    case NumberParam::type::Bin:
        if (ctx.ch != '0' && ctx.ch != '1') {
            ctx.addError(err_info::err_type::INVALID_CHARACTER);
        }
        break;
    case NumberParam::type::Oct:
        if (is_dot) {
            ctx.num.numtype = NumberParam::type::Dec;
            ctx.num.has_dot = true;
        }
        else if (ctx.ch < '0' || ctx.ch > '7') {
            ctx.addError(err_info::err_type::INVALID_CHARACTER);
        }
        break;
    case NumberParam::type::Dec:
        if (is_dot) {
            if (ctx.num.has_dot || ctx.num.in_exp) // Точка после точки или после экспоненты - ошибка
                ctx.addError(err_info::err_type::UNNECESSARY_POINT);
            ctx.num.has_dot = true;
        }
        else if (is_exp) {
            if (ctx.num.in_exp) // Вторая 'e' в числе - ошибка
                ctx.addError(err_info::err_type::INVALID_CHARACTER);
            ctx.num.in_exp = true;
        }
        else if (isdigit(ctx.ch) || is_sign_after_exp) {
            // Это нормальные части числа
        }
        else {
            // Если это не цифра, не точка, не экспонента и не суффикс - значит ошибка
            // Например: 123a
            ctx.addError(err_info::err_type::INVALID_CHARACTER);
        }
        break;
    case NumberParam::type::Hex:
        if (!isxdigit(ctx.ch)) {
            ctx.addError(err_info::err_type::INVALID_CHARACTER);
        }
        break;
    default:
        ctx.addError(err_info::err_type::INVALID_CHARACTER);
        break;

    }

}

void handlePreprocessor(AnalysisContext& ctx) {
    switch (ctx.preproc.state)
    {
    case PreprocState::AfterHesh: //Проследить за порядком
        if (isalpha(ctx.ch)) {
            ctx.preproc.state = PreprocState::InName;
            ctx.i--;
        }
        else {
            if (!isspace(ctx.ch)) {
                ctx.addError(err_info::err_type::INVALID_PREPROCESSOR_DIRECTIVE);
                ctx.preproc.state = PreprocState::ErrorConstr;
            }
        }
        break;

    case PreprocState::InName:
        if (isalpha(ctx.ch)) {
            ctx.preproc.preproc_name += ctx.ch;
        }
        else {
            ctx.preproc.setPreproc();
            if (ctx.preproc.type == PreprocStandard::NONE){
                ctx.addError(err_info::err_type::INVALID_PREPROCESSOR_DIRECTIVE);
                ctx.preproc.state = PreprocState::ErrorConstr;
            }
        }
        break;
    case PreprocState::AfterName:
        if (isspace(ctx.ch)) break; // Пропускаем пробелы после названия (напр. #include   <...)
        if (ctx.preproc.type == PreprocStandard::Include) {
            if (ctx.ch == '<' || ctx.ch == '\"') {
                ctx.preproc.state = PreprocState::InArg; // Мы не записываем эти символы, для упроцения
            }
            else {
                ctx.addError(err_info::err_type::INVALID_CHAR_AFTER_INCLUDE); // Ожидалось < или "
                ctx.preproc.state = PreprocState::ErrorConstr;
            }
        }
        else {
            ctx.i--;
            ctx.preproc.state = PreprocState::InArg;
        }
        break;
    case PreprocState::InArg:
    {
        bool normSimbol = isgraph(ctx.ch);
        if (ctx.ch == '\"' && ctx.preproc.type == PreprocStandard::Include) {
            //проверка подключенного файла
            ctx.preproc.state = PreprocState::AfterArg;
        }
        else if (ctx.ch == '>' && ctx.preproc.type == PreprocStandard::Include) {
            ctx.preproc.state = PreprocState::AfterArg;
        }
        else {
            ctx.preproc.argum_name += ctx.ch;
        }
        break;
    }
    case PreprocState::AfterArg:
        if (isgraph(ctx.ch)) {
            ctx.addError(err_info::err_type::INVALID_CONSTRUCTION);
            ctx.preproc.state = PreprocState::ErrorConstr;
        }
        break;

    case PreprocState::ErrorConstr:
        break;

    default:
        ctx.addError(err_info::err_type::UNDEFINE_ERROR);
        break;
    }
}

void PreprocChecker(AnalysisContext& ctx) {
    // Проверка после прохода всей строки
    if (ctx.preproc.state == PreprocState::InArg) {
        if (ctx.preproc.type == PreprocStandard::Include) {
            // Выдаем ошибку: ожидался закрывающий символ > или "
            char expected = (ctx.preproc.argum_name.find('<') != std::string::npos) ? '>' : '\"';
            ctx.addError(err_info::err_type::INVALID_CONSTRUCTION, expected);
        }
    }
    // Записываем тип препроцессора в str_info
    ctx.str_info.preporcstate = ctx.preproc.type;
    

}

void FindErrorInQuote(AnalysisContext& ctx) {
    if (ctx.state == State::InQuote)
        if(ctx.quote.quote_char == '\'' || ctx.quote.quote_char == '\"')
            ctx.addError(err_info::err_type::UNCLOSED_QUOTE, ctx.quote.quote_char, ctx.quote.quote_pos);
    else
        ctx.str_info.have_unclosedquote = 0;
}

void BracketChecker(string_info& str_info, const brack bracket) {
    int line = str_info.line;
    std::vector<brack>& result = str_info.brackets;
    if (IsOpenBracket(bracket.bracket)) {
        result.push_back(bracket);
    }
    else if (IsCloseBracket(bracket.bracket)) {
        if (result.empty()) {
            errors.emplace_back( pos(str_info.line, bracket.position), bracket.bracket, err_info::err_type::CLOSE_BRAKET_FIRST );
            return;
        }
        char last_open = result.back().bracket;
        if (BracketCompare(last_open, bracket.bracket)) {
            result.pop_back();
        }
        else{
            if (!HaveSimOpenBrack(result, bracket.bracket)) { //Проверяем на такую же открывающуюся во всем массиве скобок
                // В буфере нет скобок такого типа
                errors.emplace_back( pos(str_info.line, bracket.position), bracket.bracket, err_info::err_type::CLOSE_BRAKET_FIRST );
            }
            else {
                // есть скобка такого типа, значит порядок нарушен
                errors.emplace_back(FindErrUnCloseBrack(str_info));
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

std::vector<comm_percent> CommPercent(const std::vector<string_info>& Info, const int ref_percent, const int interval) {
    std::vector<comm_percent> not_comp_inter;
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
            not_comp_inter.push_back({i, percent});
        }
    }
    return not_comp_inter;
}

err_info FindErrUnCloseBrack(const string_info& str_info, const std::vector<string_info>& Lines) {
    if (Lines.empty()) {
        return { pos(- 1), ' ', err_info::err_type::UNDEFINE_ERROR};
    }

    for (int i = str_info.line; i > 0; i--) {
        if (Lines[i].brackets == str_info.brackets && Lines[i - 1].brackets != str_info.brackets)
            if (Lines[i].brackets.size() > Lines[i - 1].brackets.size())
                return err_info(pos(Lines[i].line, Lines[i].brackets.back().position),
                Lines[i].brackets.back().bracket, err_info::err_type::UNCLOSED_BRACKET);
    }
    return err_info( pos(str_info.line, str_info.brackets.back().position), 
        str_info.brackets.back().bracket, err_info::err_type::UNCLOSED_BRACKET);
}

void FindEndBrackets(const std::vector<string_info>& info) {
    if (info.back().brackets.empty())
        return;
    int line = info.back().line;
    do {
        const string_info& last_str = info[line];
        errors.emplace_back(FindErrUnCloseBrack(last_str, info));
        line = errors.back().position.line - 1;

    } while (!info[line].brackets.empty());
}
