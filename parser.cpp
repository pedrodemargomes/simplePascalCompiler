#include "parser.hpp"
#include "lexer.hpp"
#include <iostream>
#include <expected>
#include <utility>
#include <functional>
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
	} while (!isTokenBegin(token));

out:
	return vars;
err:
	std::cout << "Parser error at token:\n";
	std::cout << token.str << " " << token.type << "\n";

	return vars;

}

ExpressionAST parseExpression(std::function<bool(struct Token &)> endExpr) {
	ExpressionAST expr;

	// Parse only simple expression: Var/integer OP var/integer
	// TODO: Parse all expressions

	expr.expressionLeft = std::make_unique<ExpressionAST>();

	token = getToken();
	if (isTokenTypeInteger(token)) {
		expr.expressionLeft->operation = LITERAL;
		expr.expressionLeft->intLiteral = std::stoi(token.str);
	} else {
		expr.expressionLeft->operation = VARIABLE;
		expr.expressionLeft->var = token.str;
		struct SymbolTableEntry *s = getVarFromSymbolTable(token.str);
		if (!s) {
			std::cout << "getVarFromSymbolTable error var name: " << token.str << "\n";
			goto err;
		}
	}

	token = getToken();
	if (endExpr(token))
		return expr;

	if (!isTokenBinaryOperation(token))
		goto err;

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

	expr.expressionRight = std::make_unique<ExpressionAST>();

	token = getToken();
	if (isTokenTypeInteger(token)) {
		expr.expressionRight->operation = LITERAL;
		expr.expressionRight->intLiteral = std::stoi(token.str);
	} else {
		expr.expressionRight->operation = VARIABLE;
		expr.expressionRight->var = token.str;
		struct SymbolTableEntry *s = getVarFromSymbolTable(token.str);
		if (!s) {
			std::cout << "getVarFromSymbolTable error var name: " << token.str << "\n";
			goto err;
		}
	}

	token = getToken();
	if (!endExpr(token))
		goto err;

	return expr;

err:
	std::cout << "Parser error in ParseExpression at token:\n";
	std::cout << token.str << " " << token.type << "\n";

	return expr;
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
		} else if(isTokenWhile(token)) {
			// while
			statementAST.whileLoop = std::make_unique<WhileLoopAST>();
			statementAST.whileLoop->condition = parseExpression(isTokenDo);

			if (!isTokenDo(token))
				goto err;

			token = getToken(); // Consume do
			statementAST.whileLoop->loopCodeBlock = parseCodeBlock();
			token = getToken(); // Consume end

			if (isTokenSemicolon(token))
				goto out;
		} else {
			// Attribution
			if (isTokenAlphaNumReserved(token))
				goto err;

			std::string varName = token.str;

			token = getToken();
			if (!isTokenAttribution(token))
				goto err;

			statementAST.attribution = std::make_unique<AttributionAST>();
			statementAST.attribution->var = varName;
			struct SymbolTableEntry *s = getVarFromSymbolTable(varName);
			if (!s) {
				std::cout << "getVarFromSymbolTable error var name: " << varName << "\n";
				goto err;
			}
			statementAST.attribution->expression = parseExpression(isTokenSemicolon);
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
	programAST->codeBlock = parseCodeBlock();

	#ifdef DEBUG
	programAST->print();
	#endif

	return programAST;

err:
	std::cout << "Parser error at token:\n";
	std::cout << token.str << " " << token.type << "\n";

	return NULL;
}

