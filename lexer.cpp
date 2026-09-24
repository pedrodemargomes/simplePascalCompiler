#include <iostream>
#include <fstream>
#include <cctype>
#include <cstring>
#include <expected>

enum State {
	INIT,
	ALPHANUM,
	STRING,
	INTEGER,
	SYMBOL_ONE,
	SYMBOL_MULT
};

bool isSymbolMult(char c) {
	if (strchr(":=<>", c))
		return true;
	return false;
}

bool isSymbolOne(char c) {
	if (strchr(";().-+*/", c))
		return true;
	return false;
}

struct Token {
	enum State type;
	std::string str;
};

// Global lexer variables
enum State state;
char now, next;
std::string str;
// Return 0 at EOF and 1 when error;
std::expected<struct Token, int> getToken(std::ifstream &file) {
	struct Token token;
	for (;;) {
		now = next;
		if (!file.get(next)) {
			return std::unexpected(0); // End
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
				std::cout << str <<  "|ALPHANUM\n";
				token.str = str;
				token.type = ALPHANUM;
				str = "";
				state = INIT;
				goto out;
				//continue;
			}
			if (isSymbolMult(next)) {
				str += now;
				std::cout << str <<  "|ALPHANUM\n";
				token.str = str;
				token.type = ALPHANUM;
				str = "";
				state = SYMBOL_MULT;
				goto out;
				//continue;
			}
			if (isSymbolOne(next)) {
				str += now;
				std::cout << str <<  "|ALPHANUM\n";
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
				std::cout << str << "|ALPHANUM\n";
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
				std::cout << str << "|INTEGER\n";
				token.str = str;
				token.type = INTEGER;
				str = "";
				state = INIT;
				goto out;
				//continue;
			}
			if (isSymbolMult(next)) {
				str += now;
				std::cout << str << "|INTEGER\n";
				token.str = str;
				token.type = INTEGER;
				str = "";
				state = SYMBOL_MULT;
				goto out;
				//continue;
			}
			if (isSymbolOne(next)) {
				str += now;
				std::cout << str << "|INTEGER\n";
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
				std::cout << str << "|INTEGER\n";
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
				std::cout << str << "|SYMBOL_MULT\n";
				token.str = str;
				token.type = SYMBOL_MULT;

				str = "";
				goto out;
				//continue;
			}
			if (isdigit(next)) {
				state = INTEGER;
				str += now;
				std::cout << str << "|SYMBOL_MULT\n";
				token.str = str;
				token.type = SYMBOL_MULT;
				str = "";
				goto out;
				//continue;
			}
			if (isalpha(next)) {
				state = ALPHANUM;
				str += now;
				std::cout << str << "|SYMBOL_MULT\n";
				token.str = str;
				token.type = SYMBOL_MULT;
				str = "";
				goto out;
				//continue;
			}
			if (isspace(next)) {
				state = INIT;
				str += now;
				std::cout << str << "|SYMBOL_MULT\n";
				token.str = str;
				token.type = SYMBOL_MULT;
				str = "";
				goto out;
				//continue;
			}
			if (next == '\'') {
				state = STRING;
				str += now;
				std::cout << str << "|SYMBOL_MULT\n";
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
				std::cout << str << "|SYMBOL_ONE\n";
				token.str = str;
				token.type = SYMBOL_ONE;
				str = "";
				goto out;
				//continue;
			}
			if (isalpha(next)) {
				state = ALPHANUM;
				str += now;
				std::cout << str << "|SYMBOL_ONE\n";
				token.str = str;
				token.type = SYMBOL_ONE;
				str = "";
				goto out;
				//continue;
			}
			if (isspace(next)) {
				state = INIT;
				str += now;
				std::cout << str << "|SYMBOL_ONE\n";
				token.str = str;
				token.type = SYMBOL_ONE;
				str = "";
				goto out;
				//continue;
			}
			if (isSymbolMult(next)) {
				state = SYMBOL_MULT;
				str += now;
				std::cout << str << "|SYMBOL_ONE\n";
				token.str = str;
				token.type = SYMBOL_ONE;
				str = "";
				goto out;
				//continue;
			}
			if (isSymbolOne(next)) {
				state = SYMBOL_ONE;
				str += now;
				std::cout << str << "|SYMBOL_ONE\n";
				token.str = str;
				token.type = SYMBOL_ONE;
				str = "";
				goto out;
				//continue;
			}
			if (next == '\'') {
				state = STRING;
				str += now;
				std::cout << str << "|SYMBOL_ONE\n";
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
				std::cout << str << "|STRING\n";
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
	return token;
end:
	return std::unexpected(1);
}

int main(int argc,char *argv[]) {
	if (argc != 2) {
		std::cout << "Wrong number of arguments\n";
		return 1;
	}
	std::ifstream file(argv[1]);	
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

	for (;;) {	
		auto t = getToken(file);
		if (!t) {
			if (t.error() == 0)
				break;
			else if (t.error() == 1) {
				std::cout << "Error\n";
			}
		}
		std::cout << t.str << "\n";
	}

	file.close();

	return 0;
}

