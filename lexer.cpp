#include <iostream>
#include <fstream>
#include <cctype>
#include <cstring>

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

enum State state;

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

	char now, next;
	if (!file.get(next)) {
		std::cout << "Empty file\n";
		return 1;
	}

	std::string str = "";
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
		now = next;
		if (!file.get(next)) {
			break;
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
				str = "";
				state = INIT;
				continue;
			}
			if (isSymbolMult(next)) {
				str += now;
				std::cout << str <<  "|ALPHANUM\n";
				str = "";
				state = SYMBOL_MULT;
				continue;
			}
			if (isSymbolOne(next)) {
				str += now;
				std::cout << str <<  "|ALPHANUM\n";
				str = "";
				state = SYMBOL_ONE;
				continue;
			}
			if (next == '\'') {
				state = STRING;
				str += now;
				std::cout << str << "|ALPHANUM\n";
				str = "";
				continue;
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
				goto err;
			}
			if (isspace(next)) {
				str += now;
				std::cout << str << "|INTEGER\n";
				str = "";
				state = INIT;
				continue;
			}
			if (isSymbolMult(next)) {
				str += now;
				std::cout << str << "|INTEGER\n";
				str = "";
				state = SYMBOL_MULT;
				continue;
			}
			if (isSymbolOne(next)) {
				str += now;
				std::cout << str << "|INTEGER\n";
				str = "";
				state = SYMBOL_ONE;
				continue;
			}
			if (next == '\'') {
				state = STRING;
				str += now;
				std::cout << str << "|INTEGER\n";
				str = "";
				continue;
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
				str = "";
				continue;
			}
			if (isdigit(next)) {
				state = INTEGER;
				str += now;
				std::cout << str << "|SYMBOL_MULT\n";
				str = "";
				continue;
			}
			if (isalpha(next)) {
				state = ALPHANUM;
				str += now;
				std::cout << str << "|SYMBOL_MULT\n";
				str = "";
				continue;
			}
			if (isspace(next)) {
				state = INIT;
				str += now;
				std::cout << str << "|SYMBOL_MULT\n";
				str = "";
				continue;
			}
			if (next == '\'') {
				state = STRING;
				str += now;
				std::cout << str << "|SYMBOL_MULT\n";
				str = "";
				continue;
			}
		}
		if (state == SYMBOL_ONE) {
			if (isdigit(next)) {
				state = INTEGER;
				str += now;
				std::cout << str << "|SYMBOL_ONE\n";
				str = "";
				continue;
			}
			if (isalpha(next)) {
				state = ALPHANUM;
				str += now;
				std::cout << str << "|SYMBOL_ONE\n";
				str = "";
				continue;
			}
			if (isspace(next)) {
				state = INIT;
				str += now;
				std::cout << str << "|SYMBOL_ONE\n";
				str = "";
				continue;
			}
			if (isSymbolMult(next)) {
				state = SYMBOL_MULT;
				str += now;
				std::cout << str << "|SYMBOL_ONE\n";
				str = "";
				continue;
			}
			if (isSymbolOne(next)) {
				state = SYMBOL_ONE;
				str += now;
				std::cout << str << "|SYMBOL_ONE\n";
				str = "";
				continue;
			}
			if (next == '\'') {
				state = STRING;
				str += now;
				std::cout << str << "|SYMBOL_ONE\n";
				str = "";
				continue;
			}

		}
		if (state == STRING) {
			if (next == '\'') {
				state = INIT;
				str += now;
				str += "\'";
				std::cout << str << "|STRING\n";
				str = "";
				continue;
			} else {
				state = STRING;
				str += now;
				continue;
			}
		}
	}

	file.close();

	return 0;
err:
	return 1;
}
