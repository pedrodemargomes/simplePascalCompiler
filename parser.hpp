#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <vector>
#include <iostream>

enum VarType {
	INT
	// TODO: Add other types
};

enum StatementType {
	ATTRIBUTION,
	CONDITIONAL
};

enum Operation {
	NONE,
	VARIABLE,
	LITERAL,
	// Binary operations
	ADD,
	SUB,
	MUL,
	DIV,
	EQUAL,
	DIFF,
	GREATER,
	LESS
};

class ProgramAST;
class CodeBlockAST;
class AttributionAST;
class ConditionalAST;
class ExpressionAST;

class ExpressionAST {
	public:
	enum Operation operation = NONE;
	std::unique_ptr<ExpressionAST> expressionLeft;
	std::unique_ptr<ExpressionAST> expressionRight;
	std::string var;
	int intLiteral;
	void print() {
		if (operation == VARIABLE) {
			std::cout << var << " ";
			return;
		}
		if (operation == LITERAL) {
			std::cout << intLiteral << " ";
			return;
		}

		if (expressionLeft)
			expressionLeft->print();
		if (operation == NONE)
			return;
		// Print operation
		if (operation == ADD)
			std::cout << " + ";
		else if (operation == SUB)
			std::cout << " - ";
		else if (operation == MUL)
			std::cout << " * ";
		else if (operation == DIV)
			std::cout << " / ";
		else
			std::cout << " UNKNOWN OPERATION ";
		// Print expressionRight
		if (expressionRight)
			expressionRight->print();

	}
};

class AttributionAST {
	public:
	std::string var;
	ExpressionAST expression;
	// var = expression
	void print() {
		std::cout << var << " := ";
		expression.print();
		std::cout << "\n";
	}
};

class ConditionalAST {
	public:
	std::unique_ptr<ExpressionAST> condition;
	std::unique_ptr<CodeBlockAST> ifCodeBlock;
	std::unique_ptr<CodeBlockAST> elseCodeBlock;

};

class StatementAST {
	public:
	enum StatementType type;
	std::unique_ptr<AttributionAST> attribution;
	std::unique_ptr<ConditionalAST> conditional;
	void print() {
		if (attribution)
			attribution->print();
	}
};

class CodeBlockAST {
	public:
	std::vector<StatementAST> statements;
	void print() {
		for (auto &it : statements) {
			it.print();
		}
	}
};

class AST {
	public:
	std::unique_ptr<ProgramAST> program;
};

class VariablesAST {
	public:
	std::string name;
	enum VarType type;
};

class ProgramAST {
	public:
	std::string programName;
	std::vector<VariablesAST> vars;
	std::unique_ptr<CodeBlockAST> codeBlock;
	void print() {
		std::cout << "programName: " << programName << "\n";
		std::cout << "vars:\n";
		for (auto &it : vars) {
			std::cout << "	" << it.name << " " << it.type << "\n";
		}
		codeBlock->print();
	}
};

std::unique_ptr<ProgramAST> parseProgram();

#endif
