#include "codeGenVisitor.hpp"

CodeGenVisitor::CodeGenVisitor() {
	theContext = std::make_unique<llvm::LLVMContext>();
	theModule = std::make_unique<llvm::Module>("spc", *theContext);
	builder = std::make_unique<llvm::IRBuilder<>>(*theContext);
}

void CodeGenVisitor::visit(ProgramAST &programAST) {
	llvm::FunctionType *mainType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*theContext), false);
	llvm::Function *main = llvm::Function::Create(mainType, llvm::Function::ExternalLinkage, "program", theModule.get());	
	llvm::BasicBlock* entry = llvm::BasicBlock::Create(*theContext, "entry", main);

	builder->SetInsertPoint(entry);

	for (auto &it : programAST.vars) {
		// TODO: Support other types
		llvm::Type* type = llvm::Type::getInt32Ty(*theContext);
		auto* alloca = builder->CreateAlloca(
			type,
			nullptr,
			it.name
		);
	}

	std::cout << "\nLLVM IR:\n\n";
	theModule->print(llvm::outs(), nullptr);
}

