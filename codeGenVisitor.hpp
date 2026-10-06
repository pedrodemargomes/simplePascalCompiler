#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.hpp"
#include "lexer.hpp"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include <map>

struct LLVMSymbolTableEntry {
	llvm::AllocaInst *var;
	llvm::Argument *arg;
	llvm::Function *fun;
	std::string name;
	enum VarType type;
	int scope = -10;
};

class CodeGenVisitor {
	public:
	std::unique_ptr<llvm::LLVMContext> theContext;
	std::unique_ptr<llvm::Module> theModule;
	std::unique_ptr<llvm::IRBuilder<>> builder;
	std::map<std::string, llvm::GlobalVariable *> globalVars;
	// Global scope counter
	int scope = 0;
	std::list<struct LLVMSymbolTableEntry> symbolTable;
	llvm::FunctionCallee printfFunc;
	llvm::Value *formatStr;

	CodeGenVisitor();
	void visit(ProgramAST &programAST);
	void visit(CodeBlockAST &codeBlockAST);
	void visit(StatementAST &statementAST);
	void visit(FunOrProcAST &funOrProcAST);
	llvm::Value *visit(ExpressionAST &expressionAST);

	int addSymbolTableEntry(struct LLVMSymbolTableEntry &s);
	int removeAllFromCurrentScope();
	struct LLVMSymbolTableEntry *getVarFromSymbolTable(std::string name, bool isFun);
};

#endif
