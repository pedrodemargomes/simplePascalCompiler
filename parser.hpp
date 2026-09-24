enum VarType {
	INTEGER
	// TODO: Add other types
};

enum StatementType {
	ATTRIBUTION,
	CONDITIONAL
};

enum Operation {
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
	std::vector<VariablesAST> vars;
	std::unique_ptr<CodeBlockAST> codeBlock;
};

class CodeBlockAST {
	public:
	std::vector<StatementAST> statements;
};

class StatementAST {
	public:
	enum StatementType type;
	std::unique_ptr<AttributionAST> attribution;
	std::unique_ptr<ConditionalAST> conditional;	
};

class AttributionAST {
	public:
	std::string var;
	std::unique_ptr<ExpressionAST> expression;
	// var = expression
};

class ExpressionAST {
	public:
	enum Operation operation;
	std::unique_ptr<ExpressionAST> expressionLeft;
	std::unique_ptr<ExpressionAST> expressionRight;
	std::string var;
	int intLiteral;
		
};

class ConditionalAST {
	public:
	std::unique_ptr<ExpressionAST> condition;
	std::unique_ptr<CodeBlockAST> ifCodeBlock;
	std::unique_ptr<CodeBlockAST> elseCodeBlock;
			
};
