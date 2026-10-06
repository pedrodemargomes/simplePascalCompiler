#include "parser.hpp"
#include "lexer.hpp"
#include <iostream>
#include <expected>
#include <utility>
#include <list>

extern struct Token getToken();
struct Token token;

struct SymbolTableEntry {
	std::string name;
	enum VarType type;
	int scope;
};

// Global scope counter
int scope = -1;
std::list<struct SymbolTableEntry> symbolTable;

int removeAllFromCurrentScope() {
	for (auto it = symbolTable.begin(); it != symbolTable.end();) {
		if (it->scope == scope)
			it = symbolTable.erase(it);
		else
			++it;
	}
	return 0;
}

int addSymbolTableEntry(struct SymbolTableEntry &s) {
	for (auto &it : symbolTable) {
		if (it.name == s.name && it.scope == s.scope)
			return 1;
	}
	symbolTable.push_back(s);
	return 0;
}

struct SymbolTableEntry *getVarFromSymbolTable(std::string name) {
	for (auto it = symbolTable.rbegin(); it != symbolTable.rend(); ++it) {
		if (it->name == name)
			return &*it;
	}
	return NULL;
}

void StatementAST::print() {
	if (attribution)
		attribution->print();
	else if (conditional)
		conditional->print();
	else if (whileLoop)
		whileLoop->print();
	else if (writeLn)
		writeLn->print();
}

std::vector<VariablesAST> parseVars() {
	std::vector<VariablesAST> vars;

	if (!isTokenVar(token))
		goto out;

	token = getToken();
	do {
		if (isTokenNotAlphaNumOrReserved(token))
			goto err;
		std::string varName = token.str;

		token = getToken();
		if (!isTokenColon(token))
			goto err;

		token = getToken();
		// TODO: Support other types
		if (!isTokenInteger(token))
			goto err;

		token = getToken();
		if (!isTokenSemicolon(token))
			goto err;

		VariablesAST v;
		v.name = varName;
		v.type = INT;
		vars.emplace_back(v);

		struct SymbolTableEntry s = {
			.name = v.name,
			.type = INT,
			.scope = scope
		};
		if (addSymbolTableEntry(s)) {
			std::cout << "addSymbolTableEntry error s.name: " << s.name << " s.scope: " << s.scope << "\n";
			goto err;
		}

		token = getToken();
	} while (!isTokenBegin(token) && !isTokenFunction(token));

out:
	return vars;
err:
	std::cout << "Parser error at token:\n";
	std::cout << token.str << " " << token.type << "\n";

	return vars;

}

/*
 Precedence order (highest to lowest):
 (* /)  (+ -) (= <> < >)

expr     → | expr > addition
           | expr < addition
           | expr = addition
           | expr != addition
	   | addition

addition   → addition + term
           | addition - term
           | term

term       → term * factor
           | term / factor
           | factor

factor     → digit
           | ( expr )

	if (isTokenPlus(token))
		expr.operation = ADD;
	else if (isTokenMinus(token))
		expr.operation = SUB;
	else if (isTokenMult(token))
		expr.operation = MUL;
	else if (isTokenDiv(token))
		expr.operation = DIV;
	else if (isTokenDiff(token))
		expr.operation = DIFF;
	else if (isTokenEqu(token))
		expr.operation = EQUAL;
	else if (isTokenGreater(token))
		expr.operation = GREATER;
	else if (isTokenLess(token))
		expr.operation = LESS;

*/

std::unique_ptr<ExpressionAST> parseFactor(std::function<bool(struct Token &)> endExpr) {
	std::unique_ptr<ExpressionAST> factorExpr = std::make_unique<ExpressionAST>();

	if (isTokenTypeInteger(token)) {
		factorExpr->operation = LITERAL;
		factorExpr->intLiteral = std::stoi(token.str);
		token = getToken();
	} else if (isTokenOpenParenthesis(token)) {
		token = getToken();
		factorExpr->expressionLeft = parseExpression(isTokenCloseParenthesis);
		token = getToken();
	} else {
		//if (isTokenCloseParenthesis(token))
		//	goto out;
		if (isTokenNotAlphaNumOrReserved(token))
			goto err;

		std::string str = token.str;
		token = getToken();
		if (isTokenOpenParenthesis(token)) {
			// Is a function/procedure call
			factorExpr->operation = FUN_OR_PROC;
			factorExpr->var = str;

			// Read args
			token = getToken();
			while (!isTokenCloseParenthesis(token)) {
				std::unique_ptr<ExpressionAST> expr = parseExpression(endExpr);

				factorExpr->args.emplace_back(std::move(*expr));

				if (isTokenComma(token)) {
					token = getToken();
					continue;
				}
			}
			token = getToken();
		} else {
			factorExpr->operation = VARIABLE;
			factorExpr->var = str;
		}
	}

out:
	return factorExpr;

err:
	std::cout << "\nParser error in ParseFactor at token:\n";
	std::cout << token.str << " " << token.type << "\n";

	return factorExpr;
}

std::unique_ptr<ExpressionAST> parseTerm(std::function<bool(struct Token &)> endExpr) {
	std::unique_ptr<ExpressionAST> left = parseFactor(endExpr);

	while (isTokenMult(token) || isTokenDiv(token)) {
		std::unique_ptr<ExpressionAST> newExpr = std::make_unique<ExpressionAST>();
		if (isTokenMult(token))
			newExpr->operation = MUL;
		else if (isTokenDiv(token))
			newExpr->operation = DIV;
		token = getToken();
		std::unique_ptr<ExpressionAST> right = parseFactor(endExpr);
		newExpr->expressionLeft = std::move(left);
		newExpr->expressionRight = std::move(right);
		left = std::move(newExpr);
	}

	return left;

err:
	std::cout << "\nParser error in ParseTerm at token:\n";
	std::cout << token.str << " " << token.type << "\n";

	return left;
}

std::unique_ptr<ExpressionAST> parseAddition(std::function<bool(struct Token &)> endExpr) {
	std::unique_ptr<ExpressionAST> left = parseTerm(endExpr);

	while (isTokenMinus(token) || isTokenPlus(token)) {
		std::unique_ptr<ExpressionAST> newExpr = std::make_unique<ExpressionAST>();
		if (isTokenPlus(token))
			newExpr->operation = ADD;
		else if (isTokenMinus(token))
			newExpr->operation = SUB;
		token = getToken();
		std::unique_ptr<ExpressionAST> right = parseTerm(endExpr);
		newExpr->expressionLeft = std::move(left);
		newExpr->expressionRight = std::move(right);
		left = std::move(newExpr);
	}

	return left;

err:
	std::cout << "\nParser error in ParseAddition at token:\n";
	std::cout << token.str << " " << token.type << "\n";

	return left;
}

std::unique_ptr<ExpressionAST> parseExpression(std::function<bool(struct Token &)> endExpr) {
	std::unique_ptr<ExpressionAST> left = parseAddition(endExpr);

	while (isTokenDiff(token) || isTokenEqu(token) || isTokenGreater(token) || isTokenLess(token)) {
		std::unique_ptr<ExpressionAST> newExpr = std::make_unique<ExpressionAST>();
		if (isTokenDiff(token))
			newExpr->operation = DIFF;
		else if (isTokenEqu(token))
			newExpr->operation = EQUAL;
		else if (isTokenGreater(token))
			newExpr->operation = GREATER;
		else if (isTokenLess(token))
			newExpr->operation = LESS;
		token = getToken();
		std::unique_ptr<ExpressionAST> right = parseAddition(endExpr);
		newExpr->expressionLeft = std::move(left);
		newExpr->expressionRight = std::move(right);
		left = std::move(newExpr);
	}

	return left;

err:
	std::cout << "\nParser error in ParseExpression at token:\n";
	std::cout << token.str << " " << token.type << "\n";

	return left;
}

std::unique_ptr<CodeBlockAST> parseCodeBlock() {
	std::unique_ptr<CodeBlockAST> codeBlockAST = std::make_unique<CodeBlockAST>();

	if (!isTokenBegin(token))
		goto err;

	token = getToken();
	do {
		StatementAST statementAST = parseStatement();
		codeBlockAST->statements.push_back(std::move(statementAST));
		token = getToken();
	} while (!isTokenEnd(token));

	return codeBlockAST;

err:
	std::cout << "Parser error at token:\n";
	std::cout << token.str << " " << token.type << "\n";
	return codeBlockAST;
}

StatementAST parseStatement() {
	StatementAST statementAST;

	if (token.type == ALPHANUM) {
		// If - else
		if (isTokenIf(token)) {
			statementAST.conditional = std::make_unique<ConditionalAST>();
			token = getToken();
			statementAST.conditional->condition = parseExpression(isTokenThen);

			if (!isTokenThen(token))
				goto err;

			token = getToken(); // Consume then
			statementAST.conditional->ifCodeBlock = parseCodeBlock();
			token = getToken(); // Consume end

			if (isTokenSemicolon(token))
				goto out;

			if (!isTokenElse(token))
				goto err;

			token = getToken();
			statementAST.conditional->elseCodeBlock = parseCodeBlock();

			token = getToken();
			if (!isTokenSemicolon(token))
				goto out;
		} else if (isTokenWhile(token)) {
			// while
			statementAST.whileLoop = std::make_unique<WhileLoopAST>();
			token = getToken();
			statementAST.whileLoop->condition = parseExpression(isTokenDo);

			if (!isTokenDo(token))
				goto err;

			token = getToken(); // Consume do
			statementAST.whileLoop->loopCodeBlock = parseCodeBlock();
			token = getToken(); // Consume end

			if (isTokenSemicolon(token))
				goto out;
		} else if (isTokenWriteLn(token)) {
			token = getToken();
			if (!isTokenOpenParenthesis(token))
				goto err;	

			statementAST.writeLn = std::make_unique<WriteLnAST>();
			token = getToken();
			statementAST.writeLn->expression = parseExpression(isTokenCloseParenthesis);
			
			if (!isTokenCloseParenthesis(token))
				goto err;
			token = getToken(); // Consume )
		} else {
			if (isTokenAlphaNumReserved(token))
				goto err;

			std::string varName = token.str;

			token = getToken();
			if (isTokenAttribution(token)) {
				// Attribution
				statementAST.attribution = std::make_unique<AttributionAST>();
				statementAST.attribution->var = varName;
				struct SymbolTableEntry *s = getVarFromSymbolTable(varName);
				if (!s) {
					std::cout << "getVarFromSymbolTable error var name: " << varName << "\n";
					goto err;
				}
				token = getToken();
				statementAST.attribution->expression = parseExpression(isTokenSemicolon);
			} else if (isTokenOpenParenthesis(token)) {
				// Function call
				statementAST.funCall = std::make_unique<ExpressionAST>();
				statementAST.funCall->var = varName;
				statementAST.funCall->operation = FUN_OR_PROC;
				struct SymbolTableEntry *s = getVarFromSymbolTable(varName);
				if (!s) {
					std::cout << "getVarFromSymbolTable error var name: " << varName << "\n";
					goto err;
				}
				// Read args
				token = getToken();
				while (!isTokenCloseParenthesis(token)) {
					std::unique_ptr<ExpressionAST> expr = parseExpression(isTokenCloseParenthesis);

					statementAST.funCall->args.emplace_back(std::move(*expr));

					if (isTokenComma(token)) {
						token = getToken();
						continue;
					}
				}
				token = getToken();
			}
		}
	} else
		goto err;

out:
	return statementAST;
err:
	std::cout << "Parser error at token:\n";
	std::cout << token.str << " " << token.type << "\n";

	return statementAST;
}

std::vector<ArgumentAST> parseArgs() {
	std::vector<ArgumentAST> args;

	if (isTokenCloseParenthesis(token))
		goto out;

	for (;;) {
		if (isTokenNotAlphaNumOrReserved(token))
			goto err;
		
		std::string varName = token.str;

		token = getToken();
		if (!isTokenColon(token))
			goto err;

		token = getToken();
		if (!isTokenInteger(token))
			goto err;

		struct SymbolTableEntry s = {
			.name = varName,
			.type = INT,
			.scope = scope
		};
		if (addSymbolTableEntry(s)) {
			std::cout << "addSymbolTableEntry error s.name: " << s.name << " s.scope: " << s.scope << "\n";
			goto err;
		}
		// Add arg
		ArgumentAST arg = {
			.name = varName,
			.type = INT,
			.isRef = false
		};
		args.emplace_back(arg);

		token = getToken();
		if (isTokenCloseParenthesis(token))
			goto out;

		if (!isTokenSemicolon(token))
			goto err;

		token = getToken();
	}


out:
	return args;

err:
	std::cout << "Parser error at token:\n";
	std::cout << token.str << " " << token.type << "\n";

	return args;
}

// TODO: Add Procedure
std::vector<FunOrProcAST> parseFunOrProcs() {
	std::vector<FunOrProcAST> funOrProcAST;

	for (;;) {
		if (!isTokenFunction(token))
			goto out;

		FunOrProcAST forp;
		forp.isFun = true;

		token = getToken();
		if (isTokenNotAlphaNumOrReserved(token))
			goto err;

		forp.name = token.str;

		struct SymbolTableEntry s = {
			.name = forp.name,
			.type = FUNCTION,
			.scope = scope
		};
		if (addSymbolTableEntry(s)) {
			std::cout << "addSymbolTableEntry error s.name: " << s.name << " s.scope: " << s.scope << "\n";
			goto err;
		}

		token = getToken();
		if (!isTokenOpenParenthesis(token))
			goto err;

		scope++;
		
		token = getToken();
		forp.args = parseArgs();

		if (!isTokenCloseParenthesis(token))
			goto err;

		token = getToken();
		if (!isTokenColon(token))
			goto err;

		token = getToken();
		if (!isTokenInteger(token))
			goto err;

		forp.type = INT;

		token = getToken();
		if (!isTokenSemicolon(token))
			goto err;

		token = getToken();
		forp.vars = parseVars();
		forp.codeBlock = parseCodeBlock();

		if (!isTokenEnd(token))
			goto err;

		token = getToken();
		if (!isTokenSemicolon(token))
			goto err;

		funOrProcAST.push_back(std::move(forp));

		removeAllFromCurrentScope();
		scope--;

		token = getToken();
	}

out:
	return funOrProcAST;

err:
	std::cout << "Parser error at token:\n";
	std::cout << token.str << " " << token.type << "\n";

	return funOrProcAST;
}

std::unique_ptr<ProgramAST> parseProgram() {
	std::unique_ptr<ProgramAST> programAST;
	token = getToken();

	if (token.type == END_OF_FILE) {
		std::cout << "END OF TOKENS\n";
		goto err;
	} else if (token.type == ERROR) {
		std::cout << "LEXER ERROR\n";
		goto err;
	}

	if (!isTokenProgram(token))
		goto err;

	programAST = std::make_unique<ProgramAST>();

	token = getToken();
	if (isTokenNotAlphaNumOrReserved(token))
		goto err;

	programAST->programName = token.str;

	token = getToken();
	if (!isTokenSemicolon(token))
		goto err;

	token = getToken();
	scope++;
	programAST->vars = parseVars();
	if (isTokenFunction(token))
		programAST->funOrProcs = parseFunOrProcs();

	programAST->codeBlock = parseCodeBlock();
	programAST->print();

	return programAST;

err:
	std::cout << "Parser error at token:\n";
	std::cout << token.str << " " << token.type << "\n";

	return NULL;
}

