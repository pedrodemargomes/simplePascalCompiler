#include "parser.hpp"
#include "lexer.hpp"
#include <iostream>
#include <expected>

extern struct Token getToken();

std::unique_ptr<ProgramAST> parseProgram() {
	struct Token token = getToken();
	if (token.type == END)
		std::cout << "END OF TOKENS\n";
	else if (token.type == ERROR)
		std::cout << "LEXER ERROR\n";

	std::cout << token.str << " " << token.type << "\n";


	std::unique_ptr<ProgramAST> programAST = std::make_unique<ProgramAST>();

	return programAST;
}

