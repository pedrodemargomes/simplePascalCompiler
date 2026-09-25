#ifndef LEXER_H
#define LEXER_H

enum State {
	INIT,
	ALPHANUM,
	STRING,
	INTEGER,
	SYMBOL_ONE,
	SYMBOL_MULT,
	// For struct toker usage
	END,
	ERROR
};

struct Token {
	enum State type;
	std::string str;
};

#endif
