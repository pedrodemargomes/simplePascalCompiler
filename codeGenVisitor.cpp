#include "codeGenVisitor.hpp"

int CodeGenVisitor::removeAllFromCurrentScope() {
	for (auto it = symbolTable.begin(); it != symbolTable.end();) {
		if (it->scope == scope)
			it = symbolTable.erase(it);
		else
			++it;
	}
	return 0;
}

int CodeGenVisitor::addSymbolTableEntry(struct LLVMSymbolTableEntry &s) {
	for (auto &it : symbolTable) {
		if (it.name == s.name && it.scope == s.scope)
			return 1;
	}
	symbolTable.push_back(s);
	return 0;
}

struct LLVMSymbolTableEntry *CodeGenVisitor::getVarFromSymbolTable(std::string name) {
	for (auto it = symbolTable.rbegin(); it != symbolTable.rend(); ++it) {
		if (it->name == name)
			return &*it;
	}
	return NULL;
}

CodeGenVisitor::CodeGenVisitor() {
	theContext = std::make_unique<llvm::LLVMContext>();
	theModule = std::make_unique<llvm::Module>("spc", *theContext);
	builder = std::make_unique<llvm::IRBuilder<>>(*theContext);
}

llvm::Value *CodeGenVisitor::visit(ExpressionAST &expressionAST) {
	llvm::Value *L, *R, *ret;
	L = R = ret = NULL;
	
	if (expressionAST.operation == VARIABLE) {
		struct LLVMSymbolTableEntry *s = getVarFromSymbolTable(expressionAST.var);
		if (s) {
			if (s->var)
				L = builder->CreateLoad(
					llvm::Type::getInt32Ty(*theContext),
					s->var, "L");
			else if (s->arg)
				L = builder->CreateLoad(
					llvm::Type::getInt32Ty(*theContext),
					s->arg, "L");
		} else if (globalVars.contains(expressionAST.var)) {
			L = builder->CreateLoad(
				llvm::Type::getInt32Ty(*theContext),
				globalVars[expressionAST.var], "L");
		} else {
			std::cout << "Variable" << expressionAST.var << " not found\n";
		}

		return L;
	} else if (expressionAST.operation == LITERAL) {
		L = builder->getInt32(expressionAST.intLiteral);
		return L;
	} else if (expressionAST.operation == FUN_OR_PROC) {
		// Call function or procedure
		std::cout << "FUNTION CALL: " << expressionAST.var << "\n";
		struct LLVMSymbolTableEntry *s = getVarFromSymbolTable(expressionAST.var);
		if (!s) {
			std::cout << "getVarFromSymbolTable error var name: " << expressionAST.var << "\n";
			return NULL;
		}
		std::vector<llvm::Value *> argsv;
		for (auto &it : expressionAST.args) {
			llvm::Value *v = visit(it);
			argsv.push_back(v);
		}

		return builder->CreateCall(s->fun, argsv, "funcall");
	}

	L = this->visit(*expressionAST.expressionLeft);

	if (!expressionAST.expressionRight)
		return L;

	R = this->visit(*expressionAST.expressionRight);

	switch (expressionAST.operation) {
		case ADD:
		ret = builder->CreateAdd(L, R, "sum");
		break;
		case SUB:
		ret = builder->CreateSub(L, R, "sub");
		break;
		case MUL:
		ret = builder->CreateMul(L, R, "mul");
		break;
		case EQUAL:
		ret = builder->CreateICmpEQ(L, R, "equ");
		break;
		case DIFF:
		ret = builder->CreateICmpNE(L, R, "diff");
		break;
		case GREATER:
		ret = builder->CreateICmpSGT(L, R, "gt");
		break;
		case LESS:
		ret = builder->CreateICmpSLT(L, R, "lt");
		break;
		default:
		std::cout << "Error CodeGen invalid expression operation\n";
		break;
	}

	return ret;
}

void CodeGenVisitor::visit(StatementAST &statementAST) {
	if (statementAST.attribution) {
		llvm::Value *v = this->visit(*statementAST.attribution->expression);
		struct LLVMSymbolTableEntry *s = getVarFromSymbolTable(statementAST.attribution->var);
		if (s) {
			if (s->var)
				builder->CreateStore(v, s->var);
			else if (s->arg)
				builder->CreateStore(v, s->arg);
		} else if (globalVars.contains(statementAST.attribution->var))
			builder->CreateStore(v, globalVars[statementAST.attribution->var]);
		else
			std::cout << "Variable " << statementAST.attribution->var << " no found\n";
	} else if (statementAST.conditional) {
		llvm::Function *fun = builder->GetInsertBlock()->getParent();
		llvm::BasicBlock *thenBlock = llvm::BasicBlock::Create(*theContext, "then", fun);
		llvm::BasicBlock *elseBlock = llvm::BasicBlock::Create(*theContext, "else", fun);
		llvm::BasicBlock *continueBlock = llvm::BasicBlock::Create(*theContext, "ifcont", fun);

		llvm::Value *expr = this->visit(*statementAST.conditional->condition);

		// If boolean expression
		if (expr->getType()->isIntegerTy(1)) {
			builder->CreateCondBr(expr, thenBlock, elseBlock);
		} else {
			llvm::Value *cond = builder->CreateICmpNE(expr, builder->getInt32(0), "not_zero");
			builder->CreateCondBr(cond, thenBlock, elseBlock);
		}

		builder->SetInsertPoint(thenBlock);
		this->visit(*statementAST.conditional->ifCodeBlock);
		builder->CreateBr(continueBlock);

		if (statementAST.conditional->elseCodeBlock) {
			builder->SetInsertPoint(elseBlock);
			this->visit(*statementAST.conditional->elseCodeBlock);
			builder->CreateBr(continueBlock);
		} else {
			// Else stub
			builder->SetInsertPoint(elseBlock);
			builder->CreateBr(continueBlock);
		}

		builder->SetInsertPoint(continueBlock);
	} else if (statementAST.whileLoop) {
		llvm::Function *fun = builder->GetInsertBlock()->getParent();
		llvm::BasicBlock *whileBlock = llvm::BasicBlock::Create(*theContext, "while", fun);
		llvm::BasicBlock *whileContinueBlock = llvm::BasicBlock::Create(*theContext, "whilecont", fun);
		llvm::BasicBlock *loop = llvm::BasicBlock::Create(*theContext, "loop", fun);

		builder->CreateBr(whileBlock);
		builder->SetInsertPoint(whileBlock);

		llvm::Value *expr = this->visit(*statementAST.whileLoop->condition);
		// If boolean expression
		if (expr->getType()->isIntegerTy(1)) {
			builder->CreateCondBr(expr, loop , whileContinueBlock);
		} else {
			llvm::Value *cond = builder->CreateICmpNE(expr, builder->getInt32(0), "not_zero");
			builder->CreateCondBr(cond, loop, whileContinueBlock);
		}
		builder->SetInsertPoint(loop);

		// loop body	
		this->visit(*statementAST.whileLoop->loopCodeBlock);
		
		builder->CreateBr(whileBlock);
		builder->SetInsertPoint(whileContinueBlock);

	} else if (statementAST.writeLn) {
		llvm::Value *v = this->visit(*statementAST.writeLn->expression);
		builder->CreateCall(printfFunc, {formatStr, v});
	} else {
		std::cout << "Error CodeGen invalid statement\n";
	}

}

void CodeGenVisitor::visit(CodeBlockAST &codeBlockAST) {
	std::cout << "CODEBLOCK\n";
	for (auto &it : codeBlockAST.statements) {
		this->visit(it);
	}
}


void CodeGenVisitor::visit(FunOrProcAST &fop) {
	std::cout << "fop.name: " << fop.name << "\n";
	// Generate function code
	std::vector<llvm::Type *> argsType;
	for (auto &it : fop.args) {
		if (it.type == INT) {
			if (!it.isRef)
				argsType.push_back(llvm::Type::getInt32Ty(*theContext));
			else
				argsType.push_back(llvm::PointerType::getUnqual(*theContext));
		}
	}
	llvm::FunctionType *funType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*theContext), argsType, false);
	llvm::Function *fun = llvm::Function::Create(funType, llvm::Function::ExternalLinkage, fop.name, theModule.get());
	llvm::BasicBlock *entry = llvm::BasicBlock::Create(*theContext, "entryFun", fun);

	builder->SetInsertPoint(entry);

	struct LLVMSymbolTableEntry sfun = {
		.var = NULL,
		.arg = NULL,
		.fun = fun,
		.name = fop.name,
		.type = FUNCTION,
		.scope = this->scope
	};
	addSymbolTableEntry(sfun);
	
	llvm::Type *Int32Ty = llvm::Type::getInt32Ty(*theContext);
	llvm::AllocaInst *ret = builder->CreateAlloca(Int32Ty, nullptr, "ret");
	struct LLVMSymbolTableEntry sret = {
		.var = NULL,
		.arg = NULL,
		.fun = NULL,
		.name = fop.name,
		.type = INT,
		.scope = this->scope+1
	};
	addSymbolTableEntry(sret);

	auto itargs = fop.args.begin();
	for (auto &it : fun->args()) {
		struct LLVMSymbolTableEntry s = {
			.var = NULL,
			.arg = &it,
			.name = std::string(itargs->name),
			.type = INT,
			.scope = this->scope+1
		};
		addSymbolTableEntry(s);
		itargs++;
	}


	for (auto &it : fop.vars) {
		// TODO: Support other types
		llvm::Type *Int32Ty = llvm::Type::getInt32Ty(*theContext);
		llvm::AllocaInst *var = builder->CreateAlloca(Int32Ty, nullptr, it.name);
		struct LLVMSymbolTableEntry s = {
			.var = var,
			.arg = NULL,
			.name = it.name,
			.type = INT,
			.scope = this->scope+1
		};
		addSymbolTableEntry(s);
	}

	this->scope++;
	std::cout << "BEGIN FUN CODEBLOCK\n";
	this->visit(*fop.codeBlock);
	std::cout << "END FUN CODEBLOCK\n";
	removeAllFromCurrentScope();
	this->scope--;

	llvm::Value *r = builder->CreateLoad(
		llvm::Type::getInt32Ty(*theContext),
		ret, "loadRet");
	builder->CreateRet(r);
}

void CodeGenVisitor::visit(ProgramAST &programAST) {
	llvm::FunctionType *mainType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*theContext), false);
	llvm::Function *main = llvm::Function::Create(mainType, llvm::Function::ExternalLinkage, "main", theModule.get());
	llvm::BasicBlock *entry = llvm::BasicBlock::Create(*theContext, "entry", main);

	builder->SetInsertPoint(entry);

	llvm::FunctionType *printfType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*theContext),
		llvm::PointerType::getUnqual(*theContext),
		true);
	printfFunc = theModule->getOrInsertFunction("printf", printfType);
	formatStr = builder->CreateGlobalStringPtr("%d\n", "fmt");

	for (auto &it : programAST.vars) {
		// TODO: Support other types
		llvm::Type* type = llvm::Type::getInt32Ty(*theContext);
		llvm::GlobalVariable* gvar = new llvm::GlobalVariable(*theModule,
			type,
			false,
			llvm::GlobalValue::CommonLinkage,
			llvm::ConstantInt::get(type, 0),
			it.name);
		gvar->setAlignment(llvm::Align(4));
		globalVars[it.name] = gvar;
	}

	for (auto &it : programAST.funOrProcs)
		this->visit(it);

	builder->SetInsertPoint(entry);

	this->visit(*programAST.codeBlock);

	builder->CreateRet(llvm::ConstantInt::get(
		llvm::Type::getInt32Ty(*theContext), 0));

	theModule->print(llvm::outs(), nullptr);
}

