#include "Analyse.h"
#include "LexerUtils.h"
#include "Main.h"
#include "PrintErr.h"

// основная функция анализа строки
void analyse(const string_info& prev_str, string_info& str_info) {
    
    recent(prev_str, str_info); //узнаем что было в предыдущей строке 
    
    State state = State::Normal; //по умолчанию нормал
    if (str_info.have_unclosed_long_comment)
        state = State::InLongComment;
    else
        str_info.have_comment = 0;
	//real prev - последний символ за исключением пробелов и комментариев, нужен для проверки на двойные операторы и т.п.
    unsigned char real_prev = '\0'; //задаем первый символ проверки как пустой 
    //конструкторы пустых информаций
    NumberParam numparam = NumberParam(); 
    QuoteInfo quote_info = QuoteInfo();
    Preproc preproc = Preproc();

	int i = 0; //заранее задаем i для использования в контексте анализа, чтобы не передавать его в функции
    AnalysisContext context = AnalysisContext(str_info, i, real_prev, state, numparam, quote_info, preproc);

    for (; i < static_cast<int>(str_info.str.size()); i++)
    {   
        context.refresh(); //обновляем контекст
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
    //
    // Строка закончилась, смотрим что мы забыли закрыть или сделать
    //
    FindErrorInQuote(context); //если кавички не закрыты
    //
    // Конец файла, проверки
    //
    if (str_info.line == fileLines.back().line) 
        if(state == State::InLongComment) // Длинный коммент не закрыт
            errors.emplace_back(pos(last_long_comment_open.line, last_long_comment_open.pos), '*', err_info::err_type::UNCLOSED_LONG_COMMENT);
}

//Основная функция, перекидывает на нужные состояния или проверяет обычный текст
void handleNormal(AnalysisContext& ctx) {
    int comment_type = CommentChecker(ctx.ch, ctx.next); // 2 - длинный, 1 - строчный, 0 - нет коммента
    if (comment_type == 2) { //Длинный коммент

        ctx.state_change(State::InLongComment);
        ctx.i++; //Перебрасываем проверку, так как уже знаем следущий символ
        ctx.str_info.have_comment = comment_type;
        ctx.str_info.have_unclosed_long_comment = 1;
        last_long_comment_open = { ctx.str_info.line, ctx.i };
        return;
    }
    else if (comment_type == 1) {//Проверка на обычный коммент
        ctx.state_change(State::InLineComment);
        ctx.str_info.have_comment = comment_type;
        return;
    }
    if (IsQuote(ctx.ch)) { // Кавычки
        ctx.state_change(State::InQuote);
        ctx.quote = QuoteInfo(ctx.ch, ctx.i);
        return;
    }

    if (IsBracket(ctx.ch)) {
        brack inf = { ctx.ch, ctx.i }; // unsigned char -> char но проверка IsBracket должна убрать UB
        if (ctx.ch == ')' && ctx.real_prev == ',')
            ctx.addError(err_info::err_type::MISSING_ARGUMENT, ctx.real_prev);
        //if (ctx.ch == '}' && ctx.real_prev == ';')
        //    ctx.addError(err_info::err_type::INVALID_CONSTRUCTION, ctx.real_prev);
        BracketChecker(ctx.str_info, inf);
    }

    if (IsOperator(ctx.ch) && IsOperator(ctx.real_prev)) {
        if (binar_oprator_checker(ctx.ch, ctx.prev, ctx.real_prev))
            ctx.addError(err_info::err_type::INVALID_CONSTRUCTION);
    }
    if (ctx.ch == ',' && ctx.real_prev == ',') {
        ctx.addError(err_info::err_type::MISSING_ARGUMENT); // Двойная запятая
    }

    if (IsInvalidChar(ctx.ch)) { 
        errors.emplace_back(pos(ctx.str_info.line, ctx.i), ctx.ch, err_info::err_type::INVALID_CHARACTER);
    }

    if (ctx.ch == '#') {
        // В C++ решетка должна быть первым значимым символом в строке.
        // Если real_prev == '\0', значит до этой решетки были только пробелы или комментарии.
        if (ctx.real_prev == '\0' || ctx.real_prev == ' ') {
            ctx.state = State::InPreprocessor;
            ctx.preproc.state = PreprocState::AfterHesh;
            ctx.preproc.preproc_name = "";
            ctx.preproc.argum_name = "";
        }
        else {
            // Если real_prev != '\0', значит перед решеткой уже был какой-то код (напр. int a; #include)
            ctx.addError(err_info::err_type::INVALID_CHARACTER);
        }
        return;
    }

    bool is_start_of_number = isdigit(ctx.ch) && !isalpha(ctx.real_prev) && ctx.real_prev != '_'; // начало числа, и это точно не название
    
    if (is_start_of_number) {
        ctx.state_change(State::IsNumber);
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

void handleQuote(AnalysisContext& ctx) {
    if (ctx.ch == '\\') { // escape-последовательность
        ctx.i++;
        ctx.quote.quote_counter++;
        return;
    }
    if (ctx.ch == ctx.quote.quote_char) { //проверка на закрытие && закрытие тем же символом
        if (ctx.quote.quote_char == '\'') {
            if (ctx.quote.quote_counter == 0) // нельзя оставлять '' пустые
                ctx.addError(err_info::err_type::EMPTY_CHAR_QUOTE);
            if (ctx.quote.quote_counter > 1)
                errors.emplace_back(pos(ctx.str_info.line, ctx.i - 1), ctx.prev, err_info::err_type::TOO_LONG_CHAR_QUOTE);
        }
        ctx.state_change(State::Normal);
    }
    else
        ctx.quote.quote_counter++;
}

void handleInLongComment(AnalysisContext& ctx) {
    ctx.str_info.have_comment = 2;
    if (ctx.ch == long_comment_end[0] && ctx.next == long_comment_end[1]) {
        ctx.state_change(State::Normal);
        ctx.i++;
		ctx.str_info.have_unclosed_long_comment = 0; // теперь длинный комментарий закрыт
    }
	if (CommentChecker(ctx.ch, ctx.next)) {
		ctx.addError(err_info::err_type::OPEN_COMM_IN_COMM, ctx.ch); //предупреждаем, что во возможно коммент был не закрыт
	}
}

void handleIsNumber(AnalysisContext& ctx) {
    bool is_dot = (ctx.ch == '.');
	bool is_exp = (tolower(ctx.ch) == 'e' && ctx.num.numtype == NumberParam::type::Dec); // Экспонента может быть только в десятичной системе
    bool is_sign_after_exp = (ctx.num.numtype == NumberParam::type::Dec &&
        (tolower(ctx.prev) == 'e') &&
        (ctx.ch == '+' || ctx.ch == '-')); // Знак после экспоненты

    bool EndOfNum = !is_dot && !is_exp && !is_sign_after_exp &&
        (isspace(ctx.ch) || IsOperator(ctx.ch) || IsBracket(ctx.ch) ||
            ctx.ch == ';' || ctx.ch == ',' || ctx.ch == '_'); 

    if (EndOfNum) {
        ctx.state_change(State::Normal);
		ctx.iminus(); // Чтобы этот символ был обработан еще раз в нормальном состоянии
        return;
    }

    bool Suffix = tolower(ctx.ch) == 'u' ||
        tolower(ctx.ch) == 'l' ||
		tolower(ctx.ch) == 'f'; // Суффиксы могут быть только в конце числа, поэтому если мы встретили суффикс, то число закончилось

    if (Suffix) {
        return;
    }
    
	switch (ctx.num.numtype) { //проверяем, что символы соответствуют типу числа.
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
    auto if_comm = [&ctx]() {
        if (CommentChecker(ctx.ch, ctx.next)) {
            ctx.iminus();
            ctx.state_change(State::Normal); // Переходим в Normal, чтобы он подхватил начало комментария.
            return 1;
        }
        else return 0;
    };

    switch (ctx.preproc.state)
    {
	case PreprocState::AfterHesh: // После решетки ожидаем название препроцессора
        if (CommentChecker(ctx.ch, ctx.next)) {
            ctx.iminus();
            ctx.state_change(State::Normal); // Переходим в Normal, чтобы он подхватил начало комментария.
        }
        else if (isalpha(ctx.ch)) {
            ctx.preproc.state = PreprocState::InName;
            ctx.iminus();
        }
        else {
            if (!isspace(ctx.ch)) {
                ctx.addError(err_info::err_type::INVALID_CHARACTER); // #include например
                ctx.preproc.state = PreprocState::ErrorConstr; //если есть ошибка то смысла проверять нет, только накапливать ошибки
            }
        }
    break;

    case PreprocState::InName:
		if (isalpha(ctx.ch)) { // название директивы
            ctx.preproc.preproc_name += ctx.ch; //записываем имя предпроцессора в буфер
        }
        else { //закончилось название директивы
            ctx.preproc.setPreproc();
			if (ctx.preproc.type == PreprocStandard::NONE) { // Если директива не распознана, то выдаем ошибку
                ctx.addError(err_info::err_type::INVALID_PREPROCESSOR_DIRECTIVE); // неизвестная директива препроцессора
                ctx.preproc.state = PreprocState::ErrorConstr;
            }
        }
    break;

    case PreprocState::AfterName:
		if (if_comm() == 1) return; // здесь и дальше используем лямбду для проверки на комментарий, чтобы не дублировать код
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
        else { // далее программу можно масштабировать под другие директивы
            ctx.iminus();
            ctx.preproc.state = PreprocState::InArg;
        }
    break;

    case PreprocState::InArg:
        if (if_comm() == 1) return;
        if (ctx.ch == '\"' && ctx.preproc.type == PreprocStandard::Include) {
            ctx.preproc.state = PreprocState::AfterArg;
        }
        else if (ctx.ch == '>' && ctx.preproc.type == PreprocStandard::Include) {
            ctx.preproc.state = PreprocState::AfterArg;
        }
        else {
            ctx.preproc.argum_name += ctx.ch;
        }
    break;

    case PreprocState::AfterArg:
        if (if_comm() == 1) return; // однострочный коментарий нельзя 
        if (isgraph(ctx.ch)) {
            ctx.addError(err_info::err_type::INVALID_CONSTRUCTION); // После аргумента не должно быть видимых символов Пример #include <file> int
            ctx.preproc.state = PreprocState::ErrorConstr;
        }
        break;

    case PreprocState::ErrorConstr:
        break;

    default:
		ctx.addError(err_info::err_type::UNDEFINE_ERROR); // Ветка по умолчанию, которая по идее и не должна срабатывать
        break;
    }
}

void PreprocChecker(AnalysisContext& ctx) {
    // Проверка после прохода всей строки
    if (ctx.preproc.state == PreprocState::InArg) {
        if (ctx.preproc.type == PreprocStandard::Include) {
            // Выдаем ошибку: ожидался закрывающий символ > или "
            char expected = (ctx.str_info.str.find('<') != std::string::npos) ? '>' : '\"';
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
    std::vector<brack>& result = str_info.brackets; // информация о (как правило открытых) скобках в предыдущих строках
    if (IsOpenBracket(bracket.bracket)) {
        result.push_back(bracket); //При открывающей скобке мы просто добавляем ее в стэк скобок
    }
    else if (IsCloseBracket(bracket.bracket)) {
        if (result.empty()) { // нет открывающей
            errors.emplace_back( pos(str_info.line, bracket.position), bracket.bracket, err_info::err_type::CLOSE_BRAKET_FIRST );
            return;
        }
        char last_open = result.back().bracket;
        if (BracketCompare(last_open, bracket.bracket)) { //если последняя открытая скобка того же типа, то все верно
            result.pop_back(); //Удаляем эту открытую скобку
        }
        else{
            if (!HaveSimOpenBrack(result, bracket.bracket)) { //Проверяем на такую же открывающуюся во всем массиве скобок
                // В буфере нет скобок такого типа
                errors.emplace_back( pos(str_info.line, bracket.position), bracket.bracket, err_info::err_type::CLOSE_BRAKET_FIRST );
            }
            else {
                // есть скобка такого типа, значит порядок нарушен
                errors.emplace_back(FindErrUnCloseBrack(str_info)); //прогоняем стек скобок в поиске нужной открытой скобки
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
    int total_lines = static_cast<int>(Info.size()) - 1; // пропускаем нулевую строку
    int num_intervals = (total_lines + interval - 1) / interval; // округление вверх

    for (int i = 0; i < num_intervals; i++) { //Проходимся по интервалу и ищем проценты комментариев
        int start = i * interval + 1; // начало не с 0 а с 1
        int end = std::min((start + interval - 1), total_lines); //по причине выше убираем один символ
        int count = 0;
        int real_size = end - start + 1; // для корректного подсчета комментов в конце файла
        for (int j = start; j <= end; j++) {
            if (Info[j].have_comment != 0)
                count++;
        }
        int percent = real_size > 0 ? count * 100 / real_size : 0;
        if (percent < ref_percent) {
            not_comp_inter.push_back({ i, percent });
        }
    }
    return not_comp_inter;
}

err_info FindErrUnCloseBrack(const string_info& str_info, const std::vector<string_info>& Lines) {
    if (Lines.empty()) {
        return { pos(- 1), ' ', err_info::err_type::UNDEFINE_ERROR};
    }

    for (int i = str_info.line; i > 0; i--) { // прогон с конца
        if (str_info.brackets == Lines[i].brackets && str_info.brackets != Lines[i - 1].brackets){ //в строке находятся какие то скобки, которые могут быть незакрыты
            if (Lines[i].brackets.size() > Lines[i - 1].brackets.size()) {
                /* Логика алгоритма:
                1 {({
                2 {({[(
                3 {({[([
                4 {({[(  ]}
                закрывает^^ лишняя
                смотрим вверх
                i = 4, 4 == 4 и 4 != 3 но 4 > 3
                i = 3, 4 != 3
                i = 2, 4 == 2 и 4 != 1 и 1 < 2
                */
                return err_info(pos(Lines[i].line,
                    Lines[i].brackets.back().position),
                    Lines[i].brackets.back().bracket,
                    err_info::err_type::UNCLOSED_BRACKET);
            }
        }   
    }
    return err_info( pos(str_info.line, str_info.brackets.back().position), 
            str_info.brackets.back().bracket, 
            err_info::err_type::UNCLOSED_BRACKET); //запасной вариант
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
