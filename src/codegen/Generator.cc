#include "Generator.hh"

#include <codegen/CodegenVisitor.hh>
#include <codegen/List.hh>

#include <semantics/SemanticTree.hh>

#include <Error.hh>

#include <llvm/IR/IRBuilder.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>

#include <llvm/Object/ObjectFile.h>

#include <iostream>

namespace ingot::codegen
{
    Generator::Generator()
    : m_context() {
        LLVMInitializeNativeTarget();
        LLVMInitializeNativeAsmParser();
        LLVMInitializeNativeAsmPrinter();
    }

    void
    Generator::run(const semantics::SemanticTree& semTree) {
        llvm::IRBuilder<> builder{m_context};

        auto i8  = builder.getInt8Ty();
        auto i32 = builder.getInt32Ty();
        auto i64 = builder.getInt64Ty();
        
        auto i8p = builder.getInt8PtrTy();

        auto exitSuccess = llvm::ConstantInt::get(i32, 0, true);

        auto mainModule = std::make_unique<llvm::Module>("ingot_main_module", m_context);

        using Listi8 = List<char, 15>;
        Listi8::getLLVMFunctionEmpty(*mainModule, builder, ast::Type::int8().getName());

        Listi8 list = Listi8::empty();
        llvm::StructType* stringStructType = Listi8::getLLVMType(builder, ast::Type::int8().getName());
        llvm::PointerType* stringPtrType = llvm::PointerType::get(stringStructType, 0);

        // Declare struct for [i8]
        //List listi8{builder, mainModule.get(), i8};

        // printf declaration
        llvm::FunctionType* printfPrototype = llvm::FunctionType::get(i8p, true);
        llvm::Function* printfFunc = llvm::Function::Create(printfPrototype, llvm::Function::ExternalLinkage, "printf", mainModule.get());

        // printString declaration
        llvm::FunctionType* printStringPrototype = llvm::FunctionType::get(builder.getVoidTy(), {stringPtrType}, false);
        llvm::Function* printStringFunc = llvm::Function::Create(printStringPrototype, llvm::Function::ExternalLinkage, "ingot_runtime_print_string", mainModule.get());

        // User declarations
        llvm::Function* userMain = nullptr;
        ast::Type userMainRetType;
        std::map<const ast::FunctionDefinition*, llvm::Function*> userFunctions;
        for (const ast::FunctionDefinition& definition : semTree) {
            const ast::FunctionType& funcType = definition.getFunction().getFunctionType();
            const ast::Type& retType = funcType.getReturnType();
            llvm::Type* llvmRetType = nullptr;
            if (retType == ast::Integer::getType()) {
                llvmRetType = i64;
            } else if (retType == ast::String::getType()) {
                llvmRetType = Listi8::getLLVMType(builder, ast::Type::int8().getName());
            } else {
                throw internal_error("unhandled type: " + retType.getName());
            }
            llvm::FunctionType* prototype = llvm::FunctionType::get(llvmRetType, false); // TODO: Handle arguments
            std::string userName = definition.getName();
            
            std::string name = userFunctionPrefix + userName;
            llvm::Function* function = llvm::Function::Create(prototype, llvm::Function::InternalLinkage, name, mainModule.get());
            userFunctions[&definition] = function;
            if (userName == "main") {
                userMain = function;
                userMainRetType = retType;
            }
        }

        for (const auto&[definitionPtr, function] : userFunctions) {
            llvm::BasicBlock* body = llvm::BasicBlock::Create(m_context, "entry", function);
            builder.SetInsertPoint(body);
            CodegenVisitor visitor{*mainModule, builder, semTree, userFunctions};
            CodegenVisitorInfo info = definitionPtr->getFunction().getExpression().reduce(visitor);
            builder.CreateRet(info.m_value);
        }

        if (userMain) {
            // Main declaration
            llvm::FunctionType* mainPrototype = llvm::FunctionType::get(i32, false);
            llvm::Function* mainFunc = llvm::Function::Create(mainPrototype, llvm::Function::ExternalLinkage, "main", mainModule.get());

            // Main defintion
            llvm::BasicBlock *mainBody = llvm::BasicBlock::Create(m_context, "entry", mainFunc);
            builder.SetInsertPoint(mainBody);
            if (userMainRetType == ast::Integer::getType()) {
                std::string formatStr = "%lli";
                llvm::Constant* formatStrConstant = builder.CreateGlobalStringPtr(formatStr);
                llvm::CallInst* userMainCall = builder.CreateCall(userMain);
                builder.CreateCall(printfFunc, {formatStrConstant, userMainCall});
                builder.CreateRet(exitSuccess);
            } else if (userMainRetType == ast::String::getType()) {
                llvm::CallInst* userMainCall = builder.CreateCall(userMain);
                llvm::AllocaInst* stackString = builder.CreateAlloca(stringStructType, nullptr, "stack_string");
                builder.CreateStore(userMainCall, stackString);
                builder.CreateCall(printStringFunc, stackString);
                builder.CreateRet(exitSuccess);
            } else {
                throw internal_error("unhandled type: " + userMainRetType.getName());
            }
        }
        

        {
            // Print and execute for now... need to figure out what to do with result.
            mainModule->print(llvm::outs(), nullptr);
            std::cout << "++++++++++++++++++++++" << std::endl;
            if (userMain) {
                llvm::ExecutionEngine *executionEngine = llvm::EngineBuilder(std::move(mainModule)).setEngineKind(llvm::EngineKind::JIT).create();
                llvm::Function *main = executionEngine->FindFunctionNamed(llvm::StringRef("main"));
                auto expectedObjectFile = llvm::object::ObjectFile::createObjectFile("build/src/codegen/CMakeFiles/ingotruntime.dir/List.cc.o");
                executionEngine->addObjectFile(std::move(expectedObjectFile.get()));
                auto result = executionEngine->runFunction(main, {});
            }
            
        }
        
    }
} // namespace ingot::codegen
