#include <iostream>
#include <fstream>
#include <cctype>
#include <cstring>
#include <expected>
#include "lexer.hpp"
#include "parser.hpp"
#include "codeGenVisitor.hpp"

bool icompare_pred(unsigned char a, unsigned char b)
{
    return std::tolower(a) == std::tolower(b);
}

bool icompare(std::string const& a, std::string const& b)
{
    if (a.length() == b.length()) {
        return std::equal(b.begin(), b.end(),
                           a.begin(), icompare_pred);
    }
    else {
        return false;
    }
}

bool isTokenProgram(struct Token &token) {
	if (token.type == ALPHANUM)
		return icompare(token.str, "program");
	return false;
}

bool isTokenVar(struct Token &token) {
	if (token.type == ALPHANUM)
		return icompare(token.str, "var");
	return false;
}

bool isTokenFunction(struct Token &token) {
	if (token.type == ALPHANUM)
		return icompare(token.str, "function");
	return false;
}
bool isTokenInteger(struct Token &token) {
	if (token.type == ALPHANUM)
		return icompare(token.str, "integer");
	return false;
}

bool isTokenWriteLn(struct Token &token) {
	if (token.type == ALPHANUM)
		return icompare(token.str, "writeLn");
	return false;
}

bool isTokenTypeInteger(struct Token &token) {
	return token.type == INTEGER;
}

bool isTokenDo(struct Token &token) {
	if (token.type == ALPHANUM)
		return icompare(token.str, "do");
	return false;
}

bool isTokenWhile(struct Token &token) {
	if (token.type == ALPHANUM)
		return icompare(token.str, "while");
	return false;
}

bool isTokenBegin(struct Token &token) {
	if (token.type == ALPHANUM)
		return icompare(token.str, "begin");
	return false;
}

bool isTokenEnd(struct Token &token) {
	if (token.type == ALPHANUM)
		return icompare(token.str, "end");
	return false;
}

bool isTokenIf(struct Token &token) {
	if (token.type == ALPHANUM)
		return icompare(token.str, "if");
	return false;
}

bool isTokenElse(struct Token &token) {
	if (token.type == ALPHANUM)
		return icompare(token.str, "else");
	return false;
}

bool isTokenThen(struct Token &token) {
	if (token.type == ALPHANUM)
		return icompare(token.str, "then");
	return false;
}

bool isTokenAlphaNumReserved(struct Token &token) {
	if (isTokenProgram(token) || isTokenVar(token) || isTokenInteger(token) || isTokenBegin(token) || isTokenEnd(token) || isTokenIf(token) || isTokenElse(token) || isTokenThen(token))
		return true;
	return false;
}

bool isTokenNotAlphaNumOrReserved(struct Token &token) {
	// If not alphanum return true to get an error
	if (token.type != ALPHANUM)
		return true;
	if (isTokenProgram(token) || isTokenVar(token) || isTokenInteger(token) || isTokenBegin(token) || isTokenEnd(token) || isTokenIf(token) || isTokenElse(token) || isTokenThen(token))
		return true;
	return false;
}

bool isTokenSemicolon(struct Token &token) {
	if (token.type == SYMBOL_ONE)
		return icompare(token.str, ";");
	return false;
}

bool isTokenComma(struct Token &token) {
	if (token.type == SYMBOL_ONE)
		return icompare(token.str, ",");
	return false;
}

bool isTokenColon(struct Token &token) {
	if (token.type == SYMBOL_MULT)
		return icompare(token.str, ":");
	return false;
}

bool isTokenAttribution(struct Token &token) {
	if (token.type == SYMBOL_MULT)
		return icompare(token.str, ":=");
	return false;
}

bool isTokenGreater(struct Token &token) {
	if (token.type == SYMBOL_MULT)
		return icompare(token.str, ">");
	return false;
}

bool isTokenLess(struct Token &token) {
	if (token.type == SYMBOL_MULT)
		return icompare(token.str, "<");
	return false;
}

bool isTokenEqu(struct Token &token) {
	if (token.type == SYMBOL_MULT)
		return icompare(token.str, "=");
	return false;
}

bool isTokenDiff(struct Token &token) {
	if (token.type == SYMBOL_MULT)
		return icompare(token.str, "<>");
	return false;
}
bool isTokenDot(struct Token &token) {
	if (token.type == SYMBOL_ONE)
		return icompare(token.str, ".");
	return false;
}

bool isTokenOpenParenthesis(struct Token &token) {
	if (token.type == SYMBOL_ONE)
		return icompare(token.str, "(");
	return false;
}

bool isTokenCloseParenthesis(struct Token &token) {
	if (token.type == SYMBOL_ONE)
		return icompare(token.str, ")");
	return false;
}

bool isTokenBinaryOperation(struct Token &token) {
	if (token.type == SYMBOL_ONE && strchr("-+*/", token.str[0]))
		return true;
	if (isTokenGreater(token) || isTokenLess(token) || isTokenEqu(token) || isTokenDiff(token))
		return true;
	return false;
}

bool isTokenPlus(struct Token &token) {
	if (token.type == SYMBOL_ONE && token.str[0] == '+')
		return true;
	return false;
}

bool isTokenMinus(struct Token &token) {
	if (token.type == SYMBOL_ONE && token.str[0] == '-')
		return true;
	return false;
}

bool isTokenMult(struct Token &token) {
	if (token.type == SYMBOL_ONE && token.str[0] == '*')
		return true;
	return false;
}

bool isTokenDiv(struct Token &token) {
	if (token.type == SYMBOL_ONE && token.str[0] == '/')
		return true;
	return false;
}

bool isSymbolMult(char c) {
	if (strchr(":=<>", c))
		return true;
	return false;
}

bool isSymbolOne(char c) {
	if (strchr(";().-+*/,", c))
		return true;
	return false;
}

// Global lexer variables
std::ifstream file;
enum State state;
char now, next;
std::string str;
// Return 0 at EOF and 1 when error;
struct Token getToken() {
	struct Token token;
	for (;;) {
		now = next;
		if (!file.get(next)) {
			token.type = END_OF_FILE;
			return token;
		}
		//std::cout << "now: " << std::hex << (int) now << " next: " << std::hex << (int) next << " state: " << state << "\n";
		// Logic here
		if (state == ALPHANUM) {
			if (isalnum(next)) {
				state = ALPHANUM;
				str += now;
				continue;
			}
			if (isspace(next)) {
				str += now;
				//std::cout << str <<  "|ALPHANUM\n";
				token.str = str;
				token.type = ALPHANUM;
				str = "";
				state = INIT;
				goto out;
				//continue;
			}
			if (isSymbolMult(next)) {
				str += now;
				//std::cout << str <<  "|ALPHANUM\n";
				token.str = str;
				token.type = ALPHANUM;
				str = "";
				state = SYMBOL_MULT;
				goto out;
				//continue;
			}
			if (isSymbolOne(next)) {
				str += now;
				//std::cout << str <<  "|ALPHANUM\n";
				token.str = str;
				token.type = ALPHANUM;
				str = "";
				state = SYMBOL_ONE;
				goto out;
				//continue;
			}
			if (next == '\'') {
				state = STRING;
				str += now;
				//std::cout << str << "|ALPHANUM\n";
				token.str = str;
				token.type = ALPHANUM;
				str = "";
				goto out;
				//continue;
			}

		}
		if (state == INIT) {
			if (isdigit(next)) {
				state = INTEGER;
				continue;
			}
			if (isalpha(next)) {
				state = ALPHANUM;
				continue;
			}
			if (isspace(next)) {
				state = INIT;
				continue;
			}
			if (isSymbolMult(next)) {
				state = SYMBOL_MULT;
				continue;
			}
			if (isSymbolOne(next)) {
				state = SYMBOL_ONE;
				continue;
			}
			if (next == '\'') {
				state = STRING;
				continue;
			}
		}
		if (state == INTEGER) {
			if (isdigit(next)) {
				state = INTEGER;
				str += now;
				continue;
			}
			if (isalpha(next)) {
				std::cout << "Error: token not integer\n";
				str += now;
				std::cout << str << next << "\n";
				goto end;
			}
			if (isspace(next)) {
				str += now;
				//std::cout << str << "|INTEGER\n";
				token.str = str;
				token.type = INTEGER;
				str = "";
				state = INIT;
				goto out;
				//continue;
			}
			if (isSymbolMult(next)) {
				str += now;
				//std::cout << str << "|INTEGER\n";
				token.str = str;
				token.type = INTEGER;
				str = "";
				state = SYMBOL_MULT;
				goto out;
				//continue;
			}
			if (isSymbolOne(next)) {
				str += now;
				//std::cout << str << "|INTEGER\n";
				token.str = str;
				token.type = INTEGER;
				str = "";
				state = SYMBOL_ONE;
				goto out;
				//continue;
			}
			if (next == '\'') {
				state = STRING;
				str += now;
				//std::cout << str << "|INTEGER\n";
				token.str = str;
				token.type = INTEGER;
				str = "";
				goto out;
				//continue;
			}
		}
		if (state == SYMBOL_MULT) {
			if (isSymbolMult(next)) {
				state = SYMBOL_MULT;
				str += now;
				continue;
			}
			if (isSymbolOne(next)) {
				state = SYMBOL_ONE;
				str += now;
				//std::cout << str << "|SYMBOL_MULT\n";
				token.str = str;
				token.type = SYMBOL_MULT;

				str = "";
				goto out;
				//continue;
			}
			if (isdigit(next)) {
				state = INTEGER;
				str += now;
				//std::cout << str << "|SYMBOL_MULT\n";
				token.str = str;
				token.type = SYMBOL_MULT;
				str = "";
				goto out;
				//continue;
			}
			if (isalpha(next)) {
				state = ALPHANUM;
				str += now;
				//std::cout << str << "|SYMBOL_MULT\n";
				token.str = str;
				token.type = SYMBOL_MULT;
				str = "";
				goto out;
				//continue;
			}
			if (isspace(next)) {
				state = INIT;
				str += now;
				//std::cout << str << "|SYMBOL_MULT\n";
				token.str = str;
				token.type = SYMBOL_MULT;
				str = "";
				goto out;
				//continue;
			}
			if (next == '\'') {
				state = STRING;
				str += now;
				//std::cout << str << "|SYMBOL_MULT\n";
				token.str = str;
				token.type = SYMBOL_MULT;
				str = "";
				goto out;
				//continue;
			}
		}
		if (state == SYMBOL_ONE) {
			if (isdigit(next)) {
				state = INTEGER;
				str += now;
				//std::cout << str << "|SYMBOL_ONE\n";
				token.str = str;
				token.type = SYMBOL_ONE;
				str = "";
				goto out;
				//continue;
			}
			if (isalpha(next)) {
				state = ALPHANUM;
				str += now;
				//std::cout << str << "|SYMBOL_ONE\n";
				token.str = str;
				token.type = SYMBOL_ONE;
				str = "";
				goto out;
				//continue;
			}
			if (isspace(next)) {
				state = INIT;
				str += now;
				//std::cout << str << "|SYMBOL_ONE\n";
				token.str = str;
				token.type = SYMBOL_ONE;
				str = "";
				goto out;
				//continue;
			}
			if (isSymbolMult(next)) {
				state = SYMBOL_MULT;
				str += now;
				//std::cout << str << "|SYMBOL_ONE\n";
				token.str = str;
				token.type = SYMBOL_ONE;
				str = "";
				goto out;
				//continue;
			}
			if (isSymbolOne(next)) {
				state = SYMBOL_ONE;
				str += now;
				//std::cout << str << "|SYMBOL_ONE\n";
				token.str = str;
				token.type = SYMBOL_ONE;
				str = "";
				goto out;
				//continue;
			}
			if (next == '\'') {
				state = STRING;
				str += now;
				//std::cout << str << "|SYMBOL_ONE\n";
				token.str = str;
				token.type = SYMBOL_ONE;
				str = "";
				goto out;
				//continue;
			}

		}
		if (state == STRING) {
			if (next == '\'') {
				state = INIT;
				str += now;
				str += "\'";
				//std::cout << str << "|STRING\n";
				token.str = str;
				token.type = STRING;
				str = "";
				goto out;
				//continue;
			} else {
				state = STRING;
				str += now;
				continue;
			}
		}
	}

out:
	#ifdef DEBUG
	std::cout << token.str << " ";
	#endif
	return token;
end:
	token.type = ERROR;
	return token;
}

int main(int argc,char *argv[]) {
	if (argc != 2) {
		std::cout << "Wrong number of arguments\n";
		return 1;
	}
	file.open(argv[1]);
	if (!file) {
		std::cout << "Unable to open file\n";
		return 1;
	}

	// Set initial state of the lexer
	if (!file.get(next)) {
		std::cout << "Empty file\n";
		return 1;
	}
	str = "";
	if (isalpha(next))
		state = ALPHANUM;
	else if (isdigit(next))
		state = INTEGER;
	else if (next == '\'')
		state = STRING;
	else if (isSymbolMult(next))
		state = SYMBOL_MULT;
	else if (isSymbolOne(next))
		state = SYMBOL_ONE;

	// Build AST
	AST ast;
	ast.program = parseProgram();


	// Generate LLVM IR code
	CodeGenVisitor cgv;
	cgv.visit(*ast.program);

	file.close();

	return 0;
}

