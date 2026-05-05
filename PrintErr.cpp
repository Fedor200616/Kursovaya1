#include "PrintErr.h"
#include "Main.h"

int print_error() {
	for (auto err : errors) {
		std::cout << err.line << ' ' << err.message(err.type) << ' ' << err.symbol << '\n';
		std::cout << fileLines[err.line].str << '\n';
	}

	return 0;
}

void find_quote_err(const std::vector<string_info>& result) {
	for (const auto &res : result) {
		switch (res.have_unclosedquote){
			case 1:
				errors.push_back({ res.line, '\'', err_info::err_type::UNCLOSED_QUOTE });
				break;
			case 2:
				errors.push_back({ res.line, '\"', err_info::err_type::UNCLOSED_QUOTE });
				break;
			default:
				break;
		}
	}
}
