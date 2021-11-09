#include "Generator.hh"

#include <semantics/SemanticTree.hh>

#include <llvm/IR/IRBuilder.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>

#include <iostream>

namespace ingot::codegen
{
    Generator::Generator()
    : m_context() {}

    void
    Generator::run(const semantics::SemanticTree& tree) {
        llvm::IRBuilder<> builder{m_context};

        auto i64 = builder.getInt64Ty();
        auto i32 = builder.getInt32Ty();
        auto i8p = builder.getInt8PtrTy();

        auto exitSuccess = llvm::ConstantInt::get(i32, 0);

        auto mainModule = std::make_unique<llvm::Module>("main_module", m_context);

        // printf declaration
        llvm::FunctionType* printfPrototype = llvm::FunctionType::get(i8p, true);
        llvm::Function* printfFunc = llvm::Function::Create(printfPrototype, llvm::Function::ExternalLinkage, "printf", mainModule.get());

        // Main declaration
        llvm::FunctionType* mainPrototype = llvm::FunctionType::get(i32, false);
        llvm::Function* mainFunc = llvm::Function::Create(mainPrototype, llvm::Function::ExternalLinkage, "main", mainModule.get());
        
        for (const ast::FunctionDefinition& fundef : tree) {
            llvm::FunctionType* prototype = llvm::FunctionType::get(i64, false); // TODO: Handle arguments
            llvm::Function* function = llvm::Function::Create(prototype, llvm::Function::InternalLinkage, fundef.getPrototype().getName(), mainModule.get());
        }

        // Main defintion
        llvm::BasicBlock *mainBody = llvm::BasicBlock::Create(m_context, "body", mainFunc);
        builder.SetInsertPoint(mainBody);
        llvm::Constant* helloWorldFormatStr = builder.CreateGlobalStringPtr("%s");
        llvm::Constant* helloWorldLiteral = builder.CreateGlobalStringPtr("Hello world!\n");
        builder.CreateCall(printfFunc, {helloWorldFormatStr, helloWorldLiteral});
        builder.CreateRet(exitSuccess);

        {
            mainModule->print(llvm::outs(), nullptr);
            llvm::Module* mainModulePtr = mainModule.get(); // Save address so that we can reclaim the module
            llvm::ExecutionEngine *executionEngine = llvm::EngineBuilder(std::move(mainModule)).setEngineKind(llvm::EngineKind::Interpreter).create();
            llvm::Function *main = executionEngine->FindFunctionNamed(llvm::StringRef("main"));
            auto result = executionEngine->runFunction(main, {});
            executionEngine->removeModule(mainModulePtr);
            mainModule.reset(mainModulePtr);
        }
        
    }
} // namespace ingot::codegen
