#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <vector>
#include <iostream>
#include <functional>

enum VarType {
	INT,
	FUNCTION,
	PROCEDURE
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
	FUN_OR_PROC,
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
	std::string var;
	int intLiteral;
	std::unique_ptr<ExpressionAST> expressionRight;
	std::unique_ptr<ExpressionAST> expressionLeft;
	std::vector<ExpressionAST> args;
	void print() {
		if (operation == FUN_OR_PROC) {
			std::cout << var << "(";
			for (auto &i : args) {
				i.print();
				std::cout << " , ";
			}
			std::cout << " )";
			return;
		}
		if (operation == VARIABLE) {
			std::cout << var;
			return;
		}
		if (operation == LITERAL) {
			std::cout << intLiteral;
			return;
		}

		if (expressionLeft) {
			std::cout << "( ";
			expressionLeft->print();
			std::cout << " )";
		}
		// Print operation
		if (operation == LESS)
			std::cout << " < ";
		else if (operation == GREATER)
			std::cout << " > ";
		else if (operation == DIFF)
			std::cout << " <> ";
		else if (operation == EQUAL)
			std::cout << " = ";
		else if (operation == ADD)
			std::cout << " + ";
		else if (operation == SUB)
			std::cout << " - ";
		else if (operation == MUL)
			std::cout << " * ";
		else if (operation == DIV)
			std::cout << " / ";
		else if (operation == NONE)
			std::cout << " NONE ";
		else
			std::cout << " UNKNOWN OPERATION ";
		// Print expressionRight
		if (expressionRight) {
			std::cout << "( ";
			expressionRight->print();
			std::cout << " )";
		}
	}
};

class AttributionAST {
	public:
	std::string var;
	std::unique_ptr<ExpressionAST> expression;
	// var = expression
	void print() {
		std::cout << "; ";
		std::cout << var << " := ";
		expression->print();
		std::cout << "\n";
	}
};

class WriteLnAST {
	public:
	std::unique_ptr<ExpressionAST> expression;
	void print() {
		std::cout << "; ";
		std::cout << "writeln( ";
		expression->print();
		std::cout << ")\n";
	}
};

class StatementAST {
	public:
	enum StatementType type;
	std::unique_ptr<AttributionAST> attribution;
	std::unique_ptr<ConditionalAST> conditional;
	std::unique_ptr<WhileLoopAST> whileLoop;
	std::unique_ptr<WriteLnAST> writeLn;
	std::unique_ptr<ExpressionAST> funCall;
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
	std::unique_ptr<ExpressionAST> condition;
	std::unique_ptr<CodeBlockAST> ifCodeBlock;
	std::unique_ptr<CodeBlockAST> elseCodeBlock;
	void print() {
		std::cout << "; ";
		std::cout << "IF ";
		condition->print();
		std::cout << " THEN\n; BEGIN\n";
		ifCodeBlock->print();
		if (!elseCodeBlock)
			std::cout << "; END IF\n";
		else {
			std::cout << "; END IF\n; ELSE\n; BEGIN\n";
			elseCodeBlock->print();
			std::cout << "; END ELSE\n";
		}
	}
};

class WhileLoopAST {
	public:
	std::unique_ptr<ExpressionAST> condition;
	std::unique_ptr<CodeBlockAST> loopCodeBlock;
	void print() {
		std::cout << "; ";
		std::cout << "WHILE ";
		condition->print();
		std::cout << "; DO\n; BEGIN\n";
		loopCodeBlock->print();
		std::cout << "; END WHILE\n";
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

class ArgumentAST {
	public:
	std::string name;
	enum VarType type;
	bool isRef;
};

class FunOrProcAST {
	public:
	bool isFun;
	enum VarType type;
	std::string name;
	std::vector<ArgumentAST> args;
	std::vector<VariablesAST> vars;
	std::unique_ptr<CodeBlockAST> codeBlock;
	void print() {
		std::cout << "\n\n";
		if (isFun)
			std::cout << "; function " << name << "(";
		else
			std::cout << "; procedure " << name << "(";
		for (auto &it : args)
			std::cout << it.name << "(isRef: " << it.isRef << "), ";
		std::cout << ") : ";
		if (type == INT)
			std::cout << "INTEGER\n";
		else
			std::cout << "NONE\n";
		std::cout << "; BEGIN\n";
		std::cout << "; vars:\n";
		for (auto &it : vars) {
			std::cout << ";	" << it.name << " " << it.type << "\n";
		}
		codeBlock->print();
		std::cout << "; END\n\n";
	}
};

class ProgramAST {
	public:
	std::string programName;
	std::vector<VariablesAST> vars;
	std::vector<FunOrProcAST> funOrProcs;
	std::unique_ptr<CodeBlockAST> codeBlock;
	void print() {
		std::cout << "; programName: " << programName << "\n";
		std::cout << "; vars:\n";
		for (auto &it : vars) {
			std::cout << ";	" << it.name << " " << it.type << "\n";
		}
		for (auto &it : funOrProcs)
			it.print();
		codeBlock->print();
	}
};

std::unique_ptr<ProgramAST> parseProgram();
std::vector<VariablesAST> parseVars();
std::unique_ptr<CodeBlockAST> parseCodeBlock();
StatementAST parseStatement();
std::unique_ptr<ProgramAST> parseProgram();
std::unique_ptr<ExpressionAST> parseExpression(std::function<bool(struct Token &)> endExpr);

#endif
