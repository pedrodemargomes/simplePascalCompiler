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
class AttributionAST;
class ConditionalAST;
class ExpressionAST;
class WhileLoopAST;

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
		else if (operation == LESS)
			std::cout << " < ";
		else if (operation == GREATER)
			std::cout << " > ";
		else if (operation == DIFF)
			std::cout << " <> ";
		else if (operation == EQUAL)
			std::cout << " = ";
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

class WriteLnAST {
	public:
	ExpressionAST expression;
	void print() {
		std::cout << "writeln( ";
		expression.print();
		std::cout << ") ";
	}
};

class StatementAST {
	public:
	enum StatementType type;
	std::unique_ptr<AttributionAST> attribution;
	std::unique_ptr<ConditionalAST> conditional;
	std::unique_ptr<WhileLoopAST> whileLoop;
	std::unique_ptr<WriteLnAST> writeLn;
	void print();
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

class ConditionalAST {
	public:
	ExpressionAST condition;
	std::unique_ptr<CodeBlockAST> ifCodeBlock;
	std::unique_ptr<CodeBlockAST> elseCodeBlock;
	void print() {
		std::cout << "IF ";
		condition.print();
		std::cout << " THEN\nBEGIN\n";
		ifCodeBlock->print();
		if (!elseCodeBlock)
			std::cout << "END IF\n";
		else {
			std::cout << "END IF\nELSE\nBEGIN\n";
			elseCodeBlock->print();
			std::cout << "END ELSE\n";
		}
	}
};

class WhileLoopAST {
	public:
	ExpressionAST condition;
	std::unique_ptr<CodeBlockAST> loopCodeBlock;
	void print() {
		std::cout << "WHILE ";
		condition.print();
		std::cout << " DO\nBEGIN\n";
		loopCodeBlock->print();
		std::cout << "END WHILE\n";
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
std::vector<VariablesAST> parseVars();
ExpressionAST parseExpression();
std::unique_ptr<CodeBlockAST> parseCodeBlock();
StatementAST parseStatement();
std::unique_ptr<ProgramAST> parseProgram();





#endif
