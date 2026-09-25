#include "parser.hpp"
#include "lexer.hpp"
#include <iostream>
#include <expected>
#include <utility>

extern struct Token getToken();
struct Token token;

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
	
		token = getToken();
	} while (!isTokenBegin(token));

out:
	return vars;
err:
	std::cout << "Parser error at token:\n";
	std::cout << token.str << " " << token.type << "\n";

	return vars;

}

ExpressionAST parseExpression() {
	ExpressionAST expr;

	// Parse only simple expression: Var/integer OP var/integer
	// TODO: Parse all expressions

	expr.expressionLeft = std::make_unique<ExpressionAST>();
	
	token = getToken();
	if (isTokenInteger(token)) {
		expr.expressionLeft->operation = LITERAL;
		expr.expressionLeft->intLiteral = std::stoi(token.str);
	} else {
		expr.expressionLeft->operation = VARIABLE;
		expr.expressionLeft->var = token.str;
	}

	token = getToken();
	if (isTokenSemicolon(token))
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

	expr.expressionRight = std::make_unique<ExpressionAST>();

	token = getToken();
	if (isTokenInteger(token)) {
		expr.expressionRight->operation = LITERAL;
		expr.expressionRight->intLiteral = std::stoi(token.str);
	} else {
		expr.expressionRight->operation = VARIABLE;
		expr.expressionRight->var = token.str;
	}

	token = getToken();
	if (!isTokenSemicolon(token))
		goto err;

	return expr;

err:
	std::cout << "Parser error at token:\n";
	std::cout << token.str << " " << token.type << "\n";

	return expr;
}

StatementAST parseStatement() {
	StatementAST statementAST;

	if (token.type == ALPHANUM) {
		// If
		if (isTokenIf(token)) {
		
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
			statementAST.attribution->expression = parseExpression();
		}	
	} else
		goto err;	
		
	return statementAST;
err:
	std::cout << "Parser error at token:\n";
	std::cout << token.str << " " << token.type << "\n";

	return statementAST;
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

