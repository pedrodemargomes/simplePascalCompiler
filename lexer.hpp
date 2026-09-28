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
	END_OF_FILE,
	ERROR,
};

struct Token {
	enum State type;
	std::string str;
};

bool isTokenProgram(struct Token &token);
bool isTokenVar(struct Token &token);
bool isTokenInteger(struct Token &token);
bool isTokenBegin(struct Token &token);
bool isTokenEnd(struct Token &token);
bool isTokenIf(struct Token &token);
bool isTokenElse(struct Token &token);
bool isTokenThen(struct Token &token);
bool isTokenNotAlphaNumOrReserved(struct Token &token);
bool isTokenAlphaNumReserved(struct Token &token);
bool isTokenSemicolon(struct Token &token);
bool isTokenColon(struct Token &token);
bool isTokenAttribution(struct Token &token);
bool isTokenGreater(struct Token &token);
bool isTokenLess(struct Token &token);
bool isTokenDot(struct Token &token);
bool isTokenOpenParenthesis(struct Token &token);
bool isTokenCloseParenthesis(struct Token &token);
bool isTokenBinaryOperation(struct Token &token);
bool isTokenPlus(struct Token &token);
bool isTokenMinus(struct Token &token);
bool isTokenMult(struct Token &token);
bool isTokenDiv(struct Token &token);
bool isTokenEqu(struct Token &token);
bool isTokenDiff(struct Token &token);
bool isTokenWhile(struct Token &token);
bool isTokenDo(struct Token &token);

#endif
