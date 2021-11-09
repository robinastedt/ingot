#include "Generator.hh"

#include <codegen/CodegenVisitor.hh>
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
    Generator::run(const semantics::SemanticTree& semTree) {
        llvm::IRBuilder<> builder{m_context};

        auto i64 = builder.getInt64Ty();
        auto i32 = builder.getInt32Ty();
        auto i8p = builder.getInt8PtrTy();

        auto exitSuccess = llvm::ConstantInt::get(i32, 0, true);

        auto mainModule = std::make_unique<llvm::Module>("ingot_main_module", m_context);

        // printf declaration
        llvm::FunctionType* printfPrototype = llvm::FunctionType::get(i8p, true);
        llvm::Function* printfFunc = llvm::Function::Create(printfPrototype, llvm::Function::ExternalLinkage, "printf", mainModule.get());

        // Main declaration
        llvm::FunctionType* mainPrototype = llvm::FunctionType::get(i32, false);
        llvm::Function* mainFunc = llvm::Function::Create(mainPrototype, llvm::Function::ExternalLinkage, "main", mainModule.get());
        
        // User declarations
        using UserFunction = std::pair<const ast::FunctionDefinition&, llvm::Function*>;
        std::vector<UserFunction> userFunctions;
        for (const ast::FunctionDefinition& definition : semTree) {
            llvm::FunctionType* prototype = llvm::FunctionType::get(i64, false); // TODO: Handle arguments
            std::string name = userFunctionPrefix + definition.getPrototype().getName();
            llvm::Function* function = llvm::Function::Create(prototype, llvm::Function::InternalLinkage, name, mainModule.get());
            userFunctions.emplace_back(definition, function);
        }

        for (const auto&[definition, function] : userFunctions) {
            llvm::BasicBlock* body = llvm::BasicBlock::Create(m_context, "body", function);
            builder.SetInsertPoint(body);
            CodegenVisitor visitor{builder};
            llvm::Value* value = definition.getExpression().reduce(visitor);
            builder.CreateRet(value);
        }

        // Main defintion
        llvm::BasicBlock *mainBody = llvm::BasicBlock::Create(m_context, "body", mainFunc);
        builder.SetInsertPoint(mainBody);
        llvm::Constant* helloWorldFormatStr = builder.CreateGlobalStringPtr("%s");
        llvm::Constant* helloWorldLiteral = builder.CreateGlobalStringPtr("Hello world!\n");
        builder.CreateCall(printfFunc, {helloWorldFormatStr, helloWorldLiteral});
        builder.CreateRet(exitSuccess);

        {
            // Print and execute for now... need to figure out what to do with result.
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
