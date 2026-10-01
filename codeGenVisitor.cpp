#include "codeGenVisitor.hpp"


CodeGenVisitor::CodeGenVisitor() {
	theContext = std::make_unique<llvm::LLVMContext>();
	theModule = std::make_unique<llvm::Module>("spc", *theContext);
	builder = std::make_unique<llvm::IRBuilder<>>(*theContext);
}

llvm::Value *CodeGenVisitor::visit(ExpressionAST &expressionAST) {
	/*
	llvm::Value *L, *R, *ret;
	L = R = ret = NULL;
	
	if (expressionAST.expressionLeft->operation == VARIABLE) {
		L = builder->CreateLoad(
			llvm::Type::getInt32Ty(*theContext),
			globalVars[expressionAST.expressionLeft->var],	"L");
	} else if (expressionAST.expressionLeft->operation == LITERAL) {
		L = builder->getInt32(expressionAST.expressionLeft->intLiteral);
	} else {
		std::cout << "Error CodeGen invalid expression operation\n";
	}

	if (!expressionAST.expressionRight)
		return L;

	if (expressionAST.expressionRight->operation == VARIABLE) {
		R = builder->CreateLoad(
			llvm::Type::getInt32Ty(*theContext),
			globalVars[expressionAST.expressionRight->var],	"R");
	} else if (expressionAST.expressionRight->operation == LITERAL) {
		R = builder->getInt32(expressionAST.expressionRight->intLiteral);
	} else {
		std::cout << "Error CodeGen invalid expression operation\n";
	}

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
	*/
}

void CodeGenVisitor::visit(StatementAST &statementAST) {
	if (statementAST.attribution) {
		llvm::Value *v = this->visit(*statementAST.attribution->expression);
		builder->CreateStore(v, globalVars[statementAST.attribution->var]);
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
	for (auto &it : codeBlockAST.statements) {
		this->visit(it);
	}
}

void CodeGenVisitor::visit(ProgramAST &programAST) {
	llvm::FunctionType *mainType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*theContext), false);
	llvm::Function *main = llvm::Function::Create(mainType, llvm::Function::ExternalLinkage, "main", theModule.get());
	llvm::BasicBlock* entry = llvm::BasicBlock::Create(*theContext, "entry", main);

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
	this->visit(*programAST.codeBlock);

	builder->CreateRet(llvm::ConstantInt::get(
		llvm::Type::getInt32Ty(*theContext), 0));

	std::cout << "\n; LLVM IR:\n\n";
	theModule->print(llvm::outs(), nullptr);
}

